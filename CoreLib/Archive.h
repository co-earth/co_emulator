#pragma once

#include "Base.h"
#include "Array.h"

typedef struct {
    UInt8* Memory; 
    Int32 Length;
} ArchiveMemory;

typedef struct {
    Int32 Index;
    Int32 NameIndex;
    Int32 ParentIndex;
    Array AttributeIndices;
} ArchiveNode;

typedef struct {
    Int32 Index;
    Int32 NodeIndex;
    Int32 NameIndex;
    Int32 DataIndex;
} ArchiveAttribute;

typedef struct {
    Int32 Length;
    Char Data[0];
} ArchiveString;

typedef struct {
    Array NameTable;
    Array DataTable;
    Array Nodes;
    Array Attributes;
} Archive;

typedef struct {
    Int32 Index;
} ArchiveIterator;

CString ArchiveQueryGetChildName(
    CString Query
);

Void ArchiveLoadEmpty(
    Archive* Archive
);

Bool ArchiveLoadFromFileEncrypted(
    Archive* Archive,
    CString FilePath,
    Bool IgnoreErrors
);

Bool ArchiveLoadFromFileEncryptedNoAlloc(
    Archive* Archive,
    CString FilePath,
    Bool IgnoreErrors
);

Bool ArchiveLoadFromFile(
    Archive* Archive,
    CString FilePath,
    Bool IgnoreErrors
);

Bool ArchiveWriteToFileHandle(
    Archive* Archive,
    FILE* File,
    Bool Prettify,
    Int32 Indentation
);

Bool ArchiveWriteToFile(
    Archive* Archive,
    CString FilePath,
    Bool Prettify
);

Bool ArchiveParseFromSource(
    Archive* Archive,
    CString Source,
    Int32 Length,
    Bool IgnoreErrors
);

Void ArchiveClear(
    Archive* Archive,
    Bool KeepCapacity
);

Void ArchiveUnload(
    Archive* Archive
);

Int32 ArchiveAddNode(
    Archive* Archive,
    Int32 ParentIndex,
    CString Name,
    Int32 NameLength
);

ArchiveString* ArchiveNodeGetName(
    Archive* Archive,
    Int32 NodeIndex
);

Int32 ArchiveNodeGetParent(
    Archive* Archive,
    Int32 NodeIndex
);

Int32 ArchiveNodeGetChildByPath(
    Archive* Archive,
    Int32 NodeIndex,
    CString Path
);

Int32 ArchiveNodeAddAttribute(
    Archive* Archive,
    Int32 NodeIndex,
    CString Name,
    Int32 NameLength,
    CString Data,
    Int32 DataLength
);

Int32 ArchiveNodeGetAttributeByName(
    Archive* Archive,
    Int32 NodeIndex,
    CString Name
);

ArchiveString* ArchiveAttributeGetName(
    Archive* Archive,
    Int32 AttributeIndex
);

ArchiveString* ArchiveAttributeGetData(
    Archive* Archive,
    Int32 AttributeIndex
);

Int32 ArchiveQueryNodeWithAttribute(
    Archive* Archive,
    Int32 ParentIndex,
    CString Query,
    CString AttributeName,
    CString AttributeValue
);

ArchiveIterator* ArchiveQueryNodeIteratorByPathFirst(
    Archive* Archive,
    Int32 NodeIndex,
    CString Path
);

ArchiveIterator* ArchiveQueryNodeIteratorFirst(
    Archive* Archive,
    Int32 ParentIndex,
    CString Query
);

ArchiveIterator* ArchiveQueryNodeIteratorNext(
    Archive* Archive,
    ArchiveIterator* Iterator
);

ArchiveIterator* ArchiveNodeIteratorFirst(
    Archive* Archive,
    Int32 ParentIndex
);

ArchiveIterator* ArchiveNodeIteratorNext(
    Archive* Archive,
    ArchiveIterator* Iterator
);

ArchiveIterator* ArchiveAttributeIteratorFirst(
    Archive* Archive,
    Int32 NodeIndex
);

ArchiveIterator* ArchiveAttributeIteratorNext(
    Archive* Archive,
    ArchiveIterator* Iterator
);