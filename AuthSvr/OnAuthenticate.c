#include "AuthDB.h"
#include "Constants.h"
#include "ClientProtocol.h"
#include "ClientProcs.h"
#include "Enumerations.h"
#include "Notification.h"
#include "Server.h"

Void StartAuthTimer(
    ServerContextRef Server,
    SocketRef Socket,
    ClientContextRef Client,
    SocketConnectionRef Connection,
    UInt64 Timeout
) {
    S2C_DATA_AUTHTIMER* Response = PacketInit(S2C_DATA_AUTHTIMER);
    Response->Command = S2C_AUTHTIMER;
    Response->Timeout = Timeout;
    SocketSend(Socket, Connection, Response);

    Client->Flags |= CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
    Client->DisconnectTimestamp = Server->Timestamp + Timeout;
}

Void SendURLList(
    ServerContextRef Server,
    SocketRef Socket,
    ClientContextRef Client,
    SocketConnectionRef Connection
) {
    S2C_DATA_URLLIST* Response = PacketInit(S2C_DATA_URLLIST);
    Response->Command = S2C_URLLIST;
    PacketAppendValue(UInt32, strlen(Server->Config->Links.Itemshop));
    PacketAppendCString(Server->Config->Links.Itemshop);
    PacketAppendValue(UInt32, strlen(Server->Config->Links.Unknown1));
    PacketAppendCString(Server->Config->Links.Unknown1);
    PacketAppendValue(UInt32, strlen(Server->Config->Links.Unknown2));
    PacketAppendCString(Server->Config->Links.Unknown2);
    PacketAppendValue(UInt32, strlen(Server->Config->Links.Guild));
    PacketAppendCString(Server->Config->Links.Guild);
    PacketAppendValue(UInt32, strlen(Server->Config->Links.SNS));
    PacketAppendCString(Server->Config->Links.SNS);
    PacketAppendValue(UInt32, strlen(Server->Config->Links.Unknown3));
    PacketAppendCString(Server->Config->Links.Unknown3);
    PacketAppendValue(UInt32, strlen(Server->Config->Links.Unknown4));
    PacketAppendCString(Server->Config->Links.Unknown4);

    Response->PayloadLength[0] = (
        Response->Signature.Length - 
        sizeof(S2C_DATA_URLLIST) +
        sizeof(Response->PayloadLength[1])
    );
    Response->PayloadLength[1] = (
        Response->Signature.Length -
        sizeof(S2C_DATA_URLLIST)
    );

    SocketSend(Socket, Connection, Response);
}

Void SendMessageLoginSuccess(
    ServerContextRef Server,
    SocketRef Socket,
    ClientContextRef Client,
    SocketConnectionRef Connection
) {
    S2C_DATA_SYSTEMMESSAGE* Response = PacketInit(S2C_DATA_SYSTEMMESSAGE);
    Response->Command = S2C_SYSTEMMESSAGE;
    Response->Message = SYSTEM_MESSAGE_LOGIN_SUCCESS;
    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(OnAuthenticate, C2S_AUTHENTICATE, C2S_DATA_AUTHENTICATE) {
    if (!(Client->Flags & CLIENT_FLAGS_AUTHORIZED)) {
        return SocketDisconnect(Socket, Connection);
    }

    assert(Client->RSA);
    Int32 Length = RSA_size(Client->RSA);
    Int32 DecryptedPayloadLength = RSA_private_decrypt(
        Length,
        Packet->Payload,
        Client->RSAPayloadBuffer,
        Client->RSA,
        RSA_PKCS1_OAEP_PADDING
    );

    if (CLIENT_RSA_PAYLOAD_LENGTH != DecryptedPayloadLength) goto error;

    CString Username = (CString)&Client->RSAPayloadBuffer[0];
    Int32 UsernameLength = strlen(Username);
    if (UsernameLength > MAX_USERNAME_LENGTH) goto error;

    CString Password = (CString)&Client->RSAPayloadBuffer[129];
    Int32 PasswordLength = strlen(Password);
    if (PasswordLength > MAX_PASSWORD_LENGTH) goto error;

    S2C_DATA_AUTHENTICATE* Response = PacketInit(S2C_DATA_AUTHENTICATE);
    Response->Command = S2C_AUTHENTICATE;
    Response->KeepAlive = 1;
    Response->LoginStatus = LOGIN_STATUS_ERROR;
    Response->AccountStatus = ACCOUNT_STATUS_NORMAL;

    AUTHDB_DATA_ACCOUNT Account = { 0 };
    if (AuthDBSelectAccountByUsername(Server->Database, Username, &Account)) {
        if (Account.DeletedAt > 0) {
            Response->AccountStatus = ACCOUNT_STATUS_ACCOUNT_IS_DELETED;
        }
        else if (!Account.EmailVerified) {
            Response->AccountStatus = ACCOUNT_STATUS_ACCOUNT_NOT_VERIFIED;
        }
        else {
            Bool InvalidCredentials = !ValidatePasswordHash(
                Password,
                Account.Salt,
                MAX_PASSWORD_SALT_LENGTH,
                Account.Hash,
                MAX_PASSWORD_HASH_LENGTH
            );

            if (InvalidCredentials) {
                Response->AccountStatus = ACCOUNT_STATUS_INVALID_CREDENTIALS;
            }
            else {
                Response->AccountStatus = ACCOUNT_STATUS_NORMAL;
                Response->LoginStatus = LOGIN_STATUS_SUCCESS;

                AUTHDB_DATA_SESSION Session = { 0 };
                if (AuthDBSelectSession(Server->Database, Account.ID, &Session) && Session.Online) {
                    Response->AccountStatus = ACCOUNT_STATUS_ALREADY_LOGGED_IN;
                }
            }

            AUTHDB_DATA_BLACKLIST Blacklist = { 0 };
            if (AuthDBSelectBlacklistByAccount(Server->Database, Account.ID, &Blacklist)) {
                Response->AccountStatus = ACCOUNT_STATUS_ACCOUNT_BANNED;
            }

            if (AuthDBSelectBlacklistByAddress(Server, Connection->AddressIP, &Blacklist)) {
                Response->AccountStatus = ACCOUNT_STATUS_IP_BANNED;
            }
        }
    }
    else {
        Response->AccountStatus = ACCOUNT_STATUS_INVALID_CREDENTIALS;
       
        // TODO: Add error handling to Database to differentiate "not found" from real errors
        // Response->AccountStatus = ACCOUNT_STATUS_OUT_OF_SERVICE;
    }

    Client->AccountID = Account.ID;
    Client->AccountStatus = Response->AccountStatus;
    Client->LoginStatus = Response->LoginStatus;

    SocketSend(Socket, Connection, Response);

    if (Response->LoginStatus != LOGIN_STATUS_SUCCESS) goto error;

    Client->Flags |= CLIENT_FLAGS_AUTHENTICATED;

    StartAuthTimer(Server, Socket, Client, Connection, Server->Config->Auth.AutoDisconnectDelay);
    SendURLList(Server, Socket, Client, Connection);

    Response = PacketInit(S2C_DATA_AUTHENTICATE);
    Response->Command = S2C_AUTHENTICATE;
    Response->KeepAlive = 1;
    Response->LoginStatus = Client->LoginStatus;
    Response->AccountStatus = Client->AccountStatus;
    Response->Extended = 0x11;

    S2C_DATA_AUTHENTICATE_EXTENSION* Extension = PacketAppendStruct(S2C_DATA_AUTHENTICATE_EXTENSION);

    srand(time(NULL));
    GenerateRandomKey(Extension->AuthKey, sizeof(Extension->AuthKey));

    AuthDBUpsertSession(
        Server->Database,
        Client->AccountID,
        false,
        Extension->AuthKey,
        Connection->AddressIP
    );

    StatementRef Statement = AuthDBSelectCharacterByID(Server->Database, Client->AccountID);
    if (Statement) {
        Int32 Index = 0;
        AUTHDB_DATA_CHARACTER Character = { 0 };
        while (Index < MAX_SERVER_COUNT && AuthDBSelectCharacterFetchNext(Server->Database, Statement, &Character)) {
            Extension->Characters[Index].ServerID = Character.ServerID;
            Extension->Characters[Index].Count = Character.CharacterCount;
        }
    }

    SocketSend(Socket, Connection, Response);

    StartAuthTimer(Server, Socket, Client, Connection, Server->Config->Auth.AutoDisconnectDelay);
    SendMessageLoginSuccess(Server, Socket, Client, Connection);

    BroadcastWorldListToConnection(Server, Connection);
    
    // Just clearing the payload buffer to avoid keeping sensitive data in memory!
    memset(Client->RSAPayloadBuffer, 0, sizeof(Client->RSAPayloadBuffer));
    return;

error:
    // Just clearing the payload buffer to avoid keeping sensitive data in memory!
    memset(Client->RSAPayloadBuffer, 0, sizeof(Client->RSAPayloadBuffer));

    SocketDisconnect(Socket, Connection);
}