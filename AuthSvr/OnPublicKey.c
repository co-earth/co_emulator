#include "ClientProtocol.h"
#include "ClientProcs.h"
#include "ClientSocket.h"
#include "Server.h"

Bool GeneratePublicKey(
    RSA** GeneratedRSA,
    BIO** GeneratedKeyIO,
    UInt8** GeneratedKey,
    Int32* GeneratedKeyLength
) {
    RSA* RSA = RSA_new();
    BIGNUM* Exponent = BN_new();
    BIO* BIO = BIO_new(BIO_s_mem());

    if (!RSA || !Exponent || !BIO) goto error;

    BN_set_word(Exponent, RSA_F4);

    if (RSA_generate_key_ex(RSA, 2048, Exponent, NULL) != 1) goto error;

    i2d_RSAPublicKey_bio(BIO, RSA);

    UInt8* Key = NULL;
    Int32 Length = BIO_get_mem_data(BIO, &Key);
    if (!Key) goto error;

    BN_free(Exponent);

    *GeneratedRSA = RSA;
    *GeneratedKeyIO = BIO;
    *GeneratedKey = Key;
    *GeneratedKeyLength = Length;
    
    return true;

error:
    if (RSA) RSA_free(RSA);
    if (Exponent) BN_free(Exponent);
    if (BIO) BIO_free(BIO);

    return false;
}

CLIENT_PROCEDURE_BINDING(OnPublicKey, C2S_PUBLICKEY, C2S_DATA_PUBLICKEY) {
    if (!(Client->Flags & CLIENT_FLAGS_USERNAME_CHECKED)) {
        return SocketDisconnect(Socket, Connection);
    }

    RSA* RSA = NULL;
    BIO* BIO = NULL;
    UInt8* Key = NULL;
    Int32 KeyLength = 0;

    if (!GeneratePublicKey(&RSA, &BIO, &Key, &KeyLength)) {
        return SocketDisconnect(Socket, Connection);
    }

    if (Client->RSA) RSA_free(Client->RSA);
    
    Client->RSA = RSA;
    Client->Flags |= CLIENT_FLAGS_PUBLICKEY_INITIALIZED;

    S2C_DATA_PUBLICKEY* Response = PacketInit(S2C_DATA_PUBLICKEY);
    Response->Command = S2C_PUBLICKEY;
    Response->Unknown1 = 1;
    Response->PublicKeyLength = (UInt16)KeyLength;
    PacketAppendMemoryCopy(Key, KeyLength);
    SocketSend(Socket, Connection, Response);

    BIO_free(BIO);
}