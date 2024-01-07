#include "Diagnostic.h"
#include "ParsePrimitives.h"
#include "Util.h"

static Char kRandomKeyTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

Timestamp GetTimestamp() {
	return (Timestamp)time(NULL);
}

Bool GetPlatformErrorMessage(
	CString Buffer,
	Int32 Length
) {
	assert(Buffer);
#if _WIN32
	DWORD MessageID = GetLastError();
	if (!MessageID) return false;
	
	LPSTR MessageBuffer = NULL;
	DWORD MessageLength = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		MessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&MessageBuffer,
		0,
		NULL
	);

	memcpy_s(Buffer, Length, MessageBuffer, MessageLength);
	LocalFree(MessageBuffer);
	return true;
#else
	FatalError("Implementation missing!");
#endif
}

Bool CreateHashAlgorithm(
    ALG_ID ID, 
    HCRYPTPROV* Provider, 
    HCRYPTHASH* Algorithm
) {
    if (!CryptAcquireContext(Provider, NULL, NULL, PROV_RSA_AES, 0)) {
        if (GetLastError() == NTE_BAD_KEYSET) {
            if (!CryptAcquireContext(Provider, NULL, NULL, PROV_RSA_AES, CRYPT_NEWKEYSET)) {
                return false;
            }
        }
    }

    if (!CryptCreateHash(*Provider, ID, 0, 0, Algorithm)) {
        CryptReleaseContext(*Provider, 0);
        *Provider = NULL;
        return false;
    }

    return true;
}

Bool CreatePasswordHash(
    CString Password,
    UInt8** Salt,
    Int32* SaltLength,
    UInt8** Hash,
    Int32* HashLength
) {
    HCRYPTPROV Provider = NULL;
    HCRYPTHASH Algorithm = NULL;
    DWORD PasswordLength = (DWORD)strlen(Password);
    DWORD SaltBufferLength = 64;
    DWORD BufferLength = SaltBufferLength + PasswordLength + 1;
    BYTE* Buffer = NULL;
    BYTE* HashData = NULL;
    
    Buffer = (BYTE*)malloc(sizeof(BYTE) * BufferLength);
    if (!Buffer) goto error;

    BYTE* SaltBuffer = &Buffer[0];
    BYTE* PasswordBuffer = &Buffer[SaltBufferLength];
    memcpy(PasswordBuffer, Password, PasswordLength);
    memset(&Buffer[SaltBufferLength + PasswordLength], 0, sizeof(BYTE));

    if (!CreateHashAlgorithm(CALG_SHA_512, &Provider, &Algorithm)) goto error;
    if (!CryptGenRandom(Provider, SaltBufferLength, SaltBuffer)) goto error;
    if (!CryptHashData(Algorithm, Buffer, BufferLength, 0)) goto error;

    DWORD HashDataLength;
    DWORD HashDataSize = sizeof(DWORD);
    if (!CryptGetHashParam(Algorithm, HP_HASHSIZE, (BYTE*)&HashDataLength, &HashDataSize, 0)) goto error;

    HashData = (BYTE*)malloc(HashDataLength);
    if (!HashData) goto error;

    if (!CryptGetHashParam(Algorithm, HP_HASHVAL, HashData, &HashDataLength, 0)) goto error;

    // We are safe to just return the SaltBuffer because we know it is pointing to the start of the Buffer memory.
    *Salt = (UInt8*)SaltBuffer; 
    *SaltLength = (Int32)SaltBufferLength;
    *Hash = (UInt8*)HashData;
    *HashLength = (Int32)HashDataLength;
    CryptDestroyHash(Algorithm);
    CryptReleaseContext(Provider, 0);
    return true;

error:
    if (Algorithm) CryptDestroyHash(Algorithm);
    if (Provider) CryptReleaseContext(Provider, 0);
    if (Buffer) free(Buffer);
    if (HashData) free(HashData);
    return false;
}

Bool ValidatePasswordHash(
    CString Password,
    UInt8* Salt,
    Int32 SaltLength,
    UInt8* Hash,
    Int32 HashLength
) {
    HCRYPTPROV Provider = NULL;
    HCRYPTHASH Algorithm = NULL;
    DWORD PasswordLength = (DWORD)strlen(Password);
    DWORD SaltBufferLength = (DWORD)SaltLength;
    DWORD BufferLength = SaltBufferLength + PasswordLength + 1;
    BYTE* Buffer = NULL;
    BYTE* HashData = NULL;

    Buffer = (BYTE*)malloc(sizeof(BYTE) * BufferLength);
    if (!Buffer) goto error;

    BYTE* SaltBuffer = &Buffer[0];
    BYTE* PasswordBuffer = &Buffer[SaltBufferLength];
    memcpy(SaltBuffer, Salt, SaltBufferLength);
    memcpy(PasswordBuffer, Password, PasswordLength);
    memset(&Buffer[SaltBufferLength + PasswordLength], 0, sizeof(BYTE));

    if (!CreateHashAlgorithm(CALG_SHA_512, &Provider, &Algorithm)) goto error;
    if (!CryptHashData(Algorithm, Buffer, BufferLength, 0)) goto error;

    DWORD HashDataLength;
    DWORD HashDataSize = sizeof(DWORD);
    if (!CryptGetHashParam(Algorithm, HP_HASHSIZE, (BYTE*)&HashDataLength, &HashDataSize, 0)) goto error;

    HashData = (BYTE*)malloc(HashDataLength);
    if (!HashData) goto error;
    if (!CryptGetHashParam(Algorithm, HP_HASHVAL, HashData, &HashDataLength, 0)) goto error;

    if (HashLength != HashDataLength) goto error;
    if (memcmp(Hash, HashData, HashDataLength) != 0) goto error;

    CryptDestroyHash(Algorithm);
    CryptReleaseContext(Provider, 0);
    free(Buffer);
    free(HashData);
    return true;

error:
    if (Algorithm) CryptDestroyHash(Algorithm);
    if (Provider) CryptReleaseContext(Provider, 0);
    if (Buffer) free(Buffer);
    if (HashData) free(HashData);
    return false;
}

Void GenerateRandomKey(
    CString Buffer,
    Int32 Length
) {
    for (Int32 Index = 0; Index < Length; Index++) {
        Buffer[Index] = kRandomKeyTable[rand() % (sizeof(kRandomKeyTable) - 1)];
    }
}

UInt32 SwapUInt32(
    UInt32 Value
) {
    Value = ((Value << 8) & 0xFF00FF00) | ((Value >> 8) & 0xFF00FF);
    return (Value << 16) | (Value >> 16);
}

Int32 Random(Int32* Seed) {
    *Seed = ((*Seed * 1103515245) + 12345) & 0x7fffffff;
    return *Seed;
}

Int32 RandomRange(Int32* Seed, Int32 Min, Int32 Max) {
    Int32 Value = Random(Seed);
    return Min + Value % (Max - Min + 1);
}

Void ReadConfigString(
    CString FilePath,
    CString KeyPath,
    CString Default,
    CString *Result,
    Int32 ResultLength
) {
    Char AppName[MAX_PATH] = { 0 };

    Int32 AppNameLength = 0;
    CString Cursor = strchr(KeyPath, '.');
    if (Cursor) {
        AppNameLength = Cursor - KeyPath;
        memcpy(AppName, KeyPath, AppNameLength);
        AppName[AppNameLength] = '\0';
    }

    Int32 KeyNameOffset = AppNameLength + 1;
    Int32 KeyNameLength = strlen(KeyPath) - KeyNameOffset;
    Char KeyName[MAX_PATH] = { 0 };
    memcpy(KeyName, KeyPath + KeyNameOffset, KeyNameLength);
    KeyName[KeyNameLength] = '\0';

    GetPrivateProfileStringA(
        AppName,
        KeyName,
        Default,
        Result,
        ResultLength,
        FilePath
    );
}

Void ReadConfigCharArray(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Char* Result,
    Int32 ResultLength
) {
    ReadConfigString(FilePath, KeyPath, Default, Result, ResultLength);
}

Void ReadConfigBool(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Bool* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseBool(Buffer, Result);
}

Void ReadConfigInt8(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int8* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseInt8(Buffer, Result);
}

Void ReadConfigInt16(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int16* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseInt16(Buffer, Result);
}

Void ReadConfigInt32(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int32* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseInt32(Buffer, Result);
}

Void ReadConfigInt64(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int64* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseInt64(Buffer, Result);
}

Void ReadConfigUInt8(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt8* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseUInt8(Buffer, Result);
}

Void ReadConfigUInt16(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt16* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseUInt16(Buffer, Result);
}

Void ReadConfigUInt32(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt32* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseUInt32(Buffer, Result);
}

Void ReadConfigUInt64(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt64* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseUInt64(Buffer, Result);
}
