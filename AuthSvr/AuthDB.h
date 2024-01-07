#pragma once

#include "Base.h"
#include "AuthDBProtocol.h"

EXTERN_C_BEGIN

enum {
#define STATEMENT(__NAME__, __QUERY__) \
	__NAME__,
#include "AuthDBStatements.h"

	AUTHDB_STATEMENT_COUNT,
};

Void AuthDBPrepareStatements(
	DatabaseRef Database
);

StatementRef AuthDBGetStatement(
	DatabaseRef Database,
	Int32 StatementID
);

Bool AuthDBInsertAccount(
	DatabaseRef Database,
	CString Username,
	CString Email,
	UInt8* Salt,
	Int32 SaltLength,
	UInt8* Hash,
	Int32 HashLength
);

Bool AuthDBSelectAccountByID(
	DatabaseRef Database,
	Int32 AccountID,
	AUTHDB_DATA_ACCOUNT* Result
);

Bool AuthDBSelectAccountByUsername(
	DatabaseRef Database,
	CString Username,
	AUTHDB_DATA_ACCOUNT* Result
);

Bool AuthDBUpsertSession(
	DatabaseRef Database,
	Int32 AccountID,
	Bool Online,
	CString SessionKey,
	CString SessionIP
);

Bool AuthDBSelectSession(
	DatabaseRef Database,
	Int32 AccountID,
	AUTHDB_DATA_SESSION* Result
);

Bool AuthDBInsertBlacklistAccount(
	DatabaseRef Database,
	Int32 AccountID,
	CString Description
);

Bool AuthDBInsertBlacklistAddress(
	DatabaseRef Database,
	CString AddressIP,
	CString Description
);

Bool AuthDBSelectBlacklistByAccount(
	DatabaseRef Database,
	Int32 AccountID,
	AUTHDB_DATA_BLACKLIST* Result
);

Bool AuthDBSelectBlacklistByAddress(
	DatabaseRef Database,
	CString AddressIP,
	AUTHDB_DATA_BLACKLIST* Result
);

Bool AuthDBUpsertCharacter(
	DatabaseRef Database,
	Int32 AccountID,
	UInt8 WorldID,
	UInt8 CharacterCount
);

StatementRef AuthDBSelectCharacterByID(
	DatabaseRef Database,
	Int32 AccountID
);

Bool AuthDBSelectCharacterFetchNext(
	DatabaseRef Database,
	StatementRef Statement,
	AUTHDB_DATA_CHARACTER* Result
);

EXTERN_C_END