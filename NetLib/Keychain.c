#include "Keychain.h"

#define BIT_CONVERT(__TYPE__, __Buffer__, __INDEX__) *((__TYPE__ *)(&__Buffer__[__INDEX__]))

Void KeychainInit(
    KeychainRef Keychain,
    Bool Client
) {
    Keychain->Flags = KEYCHAIN_FLAGS_INITIAL;
    Keychain->HeaderXor = (Client) ? 0x000EB7E2 : 0xB43CC06E;
    Keychain->Step = 0;
    Keychain->Mul = 1;
    memset(Keychain->Key, 0, 0x20000);
    KeychainGenerate(Keychain, 0x8F54C37B, 0, KEYCHAIN_XORKEY_COUNT);
    Keychain->Mask[0] = (Client) ? 0xFFFFFFFF : 0x00000000;
    Keychain->Mask[1] = (Client) ? 0xFFFFFF00 : 0x000000FF;
    Keychain->Mask[2] = (Client) ? 0xFFFF0000 : 0x0000FFFF;
    Keychain->Mask[3] = (Client) ? 0xFF000000 : 0x00FFFFFF;
    Keychain->Client = Client;
}

Void KeychainGenerate(
    KeychainRef Keychain,
    UInt32 Key,
    Int32 Position,
    Int32 Size
) {
    UInt8* Buffer = &Keychain->Key[0];
    UInt32 Perm[3];

    for (Int32 Index = Position; Index < Position + Size; Index++) {
        Perm[2] = Key * 0x2F6B6F5;
        Perm[2] += 0x14698B7;
        Perm[0] = Perm[2];
        Perm[2] >>= 0x10;
        Perm[2] *= 0x27F41C3;
        Perm[2] += 0x0B327BD;
        Perm[2] >>= 0x10;

        Perm[1] = Perm[0] * 0x2F6B6F5;
        Perm[1] += 0x14698B7;
        Key = Perm[1];
        Perm[1] >>= 0x10;
        Perm[1] *= 0x27F41C3;
        Perm[1] += 0x0B327BD;
        Perm[1] &= 0xFFFF0000;

        Perm[2] |= Perm[1];
        *((UInt32*)&Buffer[Index * 4]) = Perm[2];
    }
}

Void KeychainSeed(
    KeychainRef Keychain,
    UInt32 Key,
    UInt32 Step
) {
    Keychain->Mul = 2;
    Keychain->Step = Step - 1U;

    if ((Int32)Keychain->Step < 0) {
        Keychain->Step = (UInt32)((Int32)Keychain->Step + KEYCHAIN_XORKEY_COUNT);
    }

    KeychainGenerate(Keychain, Key, KEYCHAIN_XORKEY_COUNT, KEYCHAIN_XORKEY_COUNT);
    Keychain->HeaderXor = *(UInt32*)&Keychain->Key[Keychain->Step * Keychain->Mul * 4];
}

Void KeychainEncryptClientPacket(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
) {
    UInt32 Size = Length;
    UInt8* Buffer = malloc(Length + 4);
    assert(Buffer);
    memset(Buffer, 0, Length + 4);
    assert(Buffer);
    memcpy(Buffer, Packet, Length);

    if (Size < 0x0A) {
        free(Buffer);
        return;
    }

    Int32 V0 = BIT_CONVERT(Int32, Buffer, 0) ^ ((Keychain->Flags & KEYCHAIN_FLAGS_INITIAL) ? 0x000EB7E2 : Keychain->HeaderXor);
    memcpy(Buffer, &V0, sizeof(Int32));

    if (Keychain->Flags & KEYCHAIN_FLAGS_INITIAL) {
        Keychain->Flags &= ~KEYCHAIN_FLAGS_INITIAL;
    }

    UInt32 Key = (BIT_CONVERT(UInt32, Buffer, 0) & 0x3FFF) * (UInt32)Keychain->Mul;
    Key = BIT_CONVERT(UInt32, Keychain->Key, (Int32)(Key * 4));

    UInt32 T = (Size - 8) >> 2;
    UInt32 T1;
    UInt32 Index = 8;

    while (T > 0) {
        T1 = BIT_CONVERT(UInt32, Buffer, (Int32)Index);
        T1 = T1 ^ Key;

        memcpy(&Buffer[Index], &T1, sizeof(UInt32));

        T1 = (T1 & 0x3FFF) * (UInt32)Keychain->Mul;
        Key = BIT_CONVERT(UInt32, Keychain->Key, (Int32)(T1 * 4));
        Index += 4;
        T--;
    }

    T1 = Keychain->Mask[((Size - 8) & 3)];
    T1 = ~T1;

    UInt32 T2 = (T1 & Key) ^ BIT_CONVERT(UInt32, Buffer, (Int32)Index);
    memcpy(&Buffer[Index], &T2, sizeof(UInt32));

    T1 = (Key & 0x3FFF) * (UInt32)Keychain->Mul;
    T1 = T2 ^ BIT_CONVERT(UInt32, Keychain->Key, (Int32)(T1 * 4));
    memcpy(&Buffer[4], &T1, sizeof(UInt32));
    memcpy(Packet, Buffer, Length);
    free(Buffer);

    Keychain->Step = ((Keychain->Step + 1) & 0x3FFF);
    Keychain->HeaderXor = BIT_CONVERT(UInt32, Keychain->Key, (Int32)((Keychain->Step * Keychain->Mul) * 4));
}

// NOTE: The encryption expects that the memory is padded with zeros as the 4 byte aligned access can overflow!

Void KeychainEncryptPacket(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
) {
    if (Keychain->Client) return KeychainEncryptClientPacket(Keychain, Packet, Length);

    UInt8* Buffer = malloc(Length + 4);
    memset(Buffer, 0, Length + 4);
    assert(Buffer);
    memcpy(Buffer, Packet, Length);

    *((UInt32*)Buffer) ^= 0x7AB38CF1;

    UInt32 Token = *((UInt32*)Buffer);
    Token &= 0x3FFF;
    Token *= 4;
    Token = *((UInt32*)&Keychain->Key[Token]);

    Int32 Index, Size = (Length - 4) / 4;
    for (Index = 4; Size > 0; Index += 4, Size--) {
        UInt32 Value = *((UInt32*)&Buffer[Index]);
        Value ^= Token;
        *((UInt32*)&Buffer[Index]) = Value;

        Value &= 0x3FFF;
        Value *= 4;
        Token = *((UInt32*)&Keychain->Key[Value]);
    }

    Token &= Keychain->Mask[((Length - 4) & 3)];
    *((UInt32*)&Buffer[Index]) ^= Token;

    memcpy(Packet, Buffer, Length);
    free(Buffer);

    return 0;
}

Void KeychainDecryptClientPacket(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
) {
    UInt32 Size = (UInt32)KeychainGetPacketLength(Keychain, Packet, Length);
    UInt32 T = (UInt32)(BIT_CONVERT(Int32, Packet, 0) & 0x3FFF) * 4;
    UInt32 Key = BIT_CONVERT(UInt32, Keychain->Key, T);
    *(UInt32*)&Packet[0] ^= 0x7AB38CF1;

    UInt32 Index, R, T1;
    Size -= R = (Size - 4) & 3;

    for (Index = 4; Index < Size; Index += 4) {
        T1 = BIT_CONVERT(UInt32, Packet, Index);
        Key ^= T1;
        *(UInt32*)&Packet[Index] = Key;

        T1 &= 0x3FFF;
        Key = BIT_CONVERT(UInt32, Keychain->Key, T1 * 4);
    }

    T1 = 0xFFFFFFFF >> 8 * (4 - (Int32)R);
    T1 &= Key;
    *(UInt32*)&Packet[Index] ^= T1;
    return BIT_CONVERT(UInt32, Packet, 0);
}

Void KeychainDecryptPacket(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
) {
    if (Keychain->Client) return KeychainDecryptClientPacket(Keychain, Packet, Length);

    UInt32 Header = (UInt32)KeychainGetPacketLength(Keychain, Packet, Length);
    Header <<= 16;
    Header += 0xB7E2;

    if (Keychain->Flags & KEYCHAIN_FLAGS_INITIAL) {
        Keychain->Flags &= ~KEYCHAIN_FLAGS_INITIAL;
    }

    UInt32 Token = *((UInt32*)Packet);
    Token &= 0x3FFF;
    Token *= Keychain->Mul;
    Token *= 4;
    Token = *((UInt32*)&Keychain->Key[Token]);
    *((UInt32*)Packet) = Header;

    Int32 Index, Size = (Length - 8) / 4;

    for (Index = 8; Size > 0; Index += 4, Size--) {
        UInt32 Value = *((UInt32*)&Packet[Index]);
        Token ^= Value;
        *((UInt32*)&Packet[Index]) = Token;

        Value &= 0x3FFF;
        Value *= Keychain->Mul;
        Value *= 4;
        Token = *((UInt32*)&Keychain->Key[Value]);
    }

    Token &= Keychain->Mask[((Length - 8) & 3)];
    *((UInt32*)&Packet[Index]) ^= Token;
    *((UInt32*)&Packet[4]) = 0;

    Keychain->Step += 1;
    Keychain->Step &= 0x3FFF;
    Keychain->HeaderXor = *((UInt32*)&Keychain->Key[Keychain->Step * Keychain->Mul * 4]);
}

Int32 KeychainGetClientPacketLength(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
) {
    Int32 Size;
    UInt32 Header = *(UInt32*)&Packet[0];
    Header ^= 0x7AB38CF1;

    if (*(uint16_t*)&Header == 0xC8F3) {
        uint64_t XHeader = *(uint64_t*)&Packet[0];
        UInt32 T = *(UInt32*)&Packet[0];
        T &= 0x3FFF;
        T *= 4;
        UInt32 Key;
        Key = *(UInt32*)&Keychain->Key[T];

        UInt32* Pxh = (UInt32*)&XHeader;
        Pxh[0] ^= 0x7AB38CF1;
        Pxh[1] ^= Key;

        XHeader >>= 16;
        XHeader &= 0xFFFFFFFF;

        Size = (Int32)XHeader;
    }
    else {
        Header >>= 16;
        Size = (Int32)Header;
    }

    return Size;
}

Int32 KeychainGetPacketLength(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
) {
    if (Keychain->Client) return KeychainGetClientPacketLength(Keychain, Packet, Length);
    if (Length < sizeof(UInt32)) return 0;

    UInt32 Token = *((UInt32*)Packet);

    if (Keychain->Flags & KEYCHAIN_FLAGS_INITIAL) {
        return 0x0E;
    }

    Token ^= Keychain->HeaderXor;
    Token >>= 16;

    return (Int32)Token;
}
