#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef struct _Database* DatabaseRef;
typedef struct _Statement* StatementRef;

DatabaseRef DatabaseConnect(
    CString Host,
    CString Username,
    CString Password,
    CString Database,
    UInt16 Port
);

Void DatabaseDisconnect(
    DatabaseRef Database
);

Bool DatabaseBeginTransaction(
    DatabaseRef Database
);

Bool DatabaseCommitTransaction(
    DatabaseRef Database
);

Bool DatabaseRollbackTransaction(
    DatabaseRef Database
);

StatementRef DatabaseCreateStatement(
    DatabaseRef Database,
    CString Query
);

Int64 DatabaseGetLastInsertID(
    DatabaseRef Database
);

Void StatementBindParameterBinary(
    StatementRef Statement,
    Int32 Index,
    UInt8* Value,
    Int32 Length
);

Void StatementBindParameterBinary(
    StatementRef Statement,
    Int32 Index,
    UInt8* Value,
    Int32 Length
);

Void StatementBindParameterBool(
    StatementRef Statement,
    Int32 Index,
    Bool Value
);

Void StatementBindParameterString(
    StatementRef Statement,
    Int32 Index,
    CString Value
);

Void StatementBindParameterUInt8(
    StatementRef Statement,
    Int32 Index,
    UInt8 Value);

Void StatementBindParameterUInt16(
    StatementRef Statement,
    Int32 Index,
    UInt16 Value
);

Void StatementBindParameterUInt32(
    StatementRef Statement,
    Int32 Index,
    UInt32 Value
);

Void StatementBindParameterUInt64(
    StatementRef Statement,
    Int32 Index,
    UInt64 Value
);

Void StatementBindParameterInt8(
    StatementRef Statement,
    Int32 Index,
    Int8 Value
);

Void StatementBindParameterInt16(
    StatementRef Statement,
    Int32 Index,
    Int16 Value
);

Void StatementBindParameterInt32(
    StatementRef Statement,
    Int32 Index,
    Int32 Value
);

Void StatementBindParameterInt64(
    StatementRef Statement, 
    Int32 Index, 
    Int64 Value
);

Void StatementBindParameterFloat32(
    StatementRef Statement, 
    Int32 Index, 
    Float32 value
);

Void StatementBindParameterFloat64(
    StatementRef Statement, 
    Int32 Index, 
    Float64 value
);

Bool StatementExecute(
    StatementRef Statement
);

Bool StatementFetchResult(
    StatementRef Statement
);

Void StatementFlushResults(
    StatementRef Statement
);

Int64 StatementLastInsertID(
    StatementRef Statement
);

Int32 StatementGetResultLength(
    StatementRef Statement,
    Int32 Index
);

Void StatementReadResultBinary(
    StatementRef Statement,
    Int32 Index,
    Int32 MaxLength,
    UInt8* Result,
    Int32* Length
);

Void StatementReadResultBool(
    StatementRef Statement, 
    Int32 Index,
    Bool* Result
);

Void StatementReadResultString(
    StatementRef Statement, 
    Int32 Index,
    Int32 MaxLength,
    CString Result
);

Void StatementReadResultUInt8(
    StatementRef Statement, 
    Int32 Index,
    UInt8* Result
);

Void StatementReadResultUInt16(
    StatementRef Statement, 
    Int32 Index,
    UInt16* Result
);

Void StatementReadResultUInt32(
    StatementRef Statement,
    Int32 Index,
    UInt32* Result
);

Void StatementReadResultUInt64(
    StatementRef Statement,
    Int32 Index,
    UInt64* Result
);

Void StatementReadResultInt8(
    StatementRef Statement,
    Int32 Index,
    Int8* Result
);

Void StatementReadResultInt16(
    StatementRef Statement,
    Int32 Index,
    Int16* Result
);

Void StatementReadResultInt32(
    StatementRef Statement,
    Int32 Index,
    Int32* Result
);

Void StatementReadResultInt64(
    StatementRef Statement,
    Int32 Index,
    Int64* Result
);

Void StatementReadResultFloat32(
    StatementRef Statement,
    Int32 Index,
    Float32* Result
);

Void StatementReadResultFloat64(
    StatementRef Statement,
    Int32 Index,
    Float64* Result
);

Void StatementReadResultTimestamp(
    StatementRef Statement,
    Int32 Index,
    Timestamp* Result
);

EXTERN_C_END
