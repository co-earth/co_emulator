#include "Archive.h"
#include "Diagnostic.h"
#include "Encryption.h"
#include "FileIO.h"
#include "ParsePrimitives.h"

CString ArchiveQueryGetChildName(
    CString Query
) {
    CString Cursor = strrchr(Query, '.');
    return Cursor ? Cursor + 1 : Query;
}

Void ArchiveLoadEmpty(
    Archive* Archive
) {
    ArrayInitializeEmpty(&Archive->NameTable, sizeof(UInt8), 0x1000);
    ArrayInitializeEmpty(&Archive->DataTable, sizeof(UInt8), 0x1000);
    ArrayInitializeEmpty(&Archive->Nodes, sizeof(ArchiveNode), 0x10);
    ArrayInitializeEmpty(&Archive->Attributes, sizeof(ArchiveAttribute), 0x10);
}

Bool ArchiveLoadFromFileEncrypted(
    Archive* Archive,
    CString FilePath,
    Bool IgnoreErrors
) {
    ArchiveLoadEmpty(Archive);
    return ArchiveLoadFromFileEncryptedNoAlloc(Archive, FilePath, IgnoreErrors);
}

Bool ArchiveLoadFromFileEncryptedNoAlloc(
    Archive* Archive,
    CString FilePath,
    Bool IgnoreErrors
) {
    ArchiveLoadEmpty(Archive);

    UInt8* Buffer = NULL;
    Int32 BufferLength = 0;

    if (!EncryptionDecryptFile(FilePath, &Buffer, &BufferLength)) {
        goto error;
    }

    if (!ArchiveParseFromSource(Archive, (CString)Buffer, BufferLength, IgnoreErrors)) {
        free(Buffer);
        goto error;
    }

    free(Buffer);

    return true;

error:
    LogMessageFormat(LOG_LEVEL_ERROR, "Error loading archive: %s", FilePath);

    return false;
}

Bool ArchiveLoadFromFile(
    Archive* Archive,
    CString FilePath,
    Bool IgnoreErrors
) {
    UInt8* Source = NULL;
    Int32 SourceLength = 0;
    FileRef File = FileOpen(FilePath);
    if (!File) goto error;
    if (!FileRead(File, &Source, &SourceLength)) goto error;
    
    ArchiveLoadEmpty(Archive);

    if (!ArchiveParseFromSource(Archive, Source, SourceLength, IgnoreErrors)) {
        ArchiveUnload(Archive);
        goto error;
    }

    free(Source);

    return true;

error:
    if (File) FileClose(File);
    if (Source) free(Source);

    return false;
}

static inline Bool ArchiveNodeWriteToFile(
    Archive* Archive,
    Int32 NodeIndex,
    FILE* File,
    Bool Prettify,
    Int32 Indentation
) {
    ArchiveString* NodeName = ArchiveNodeGetName(Archive, NodeIndex);
   
    if (Prettify)
        for (Int32 Index = 0; Index < Indentation * 4; Index++)
            fprintf(File, " ");

    fprintf(File, "<%.*s", NodeName->Length, NodeName->Data);

    ArchiveIterator* Iterator = ArchiveAttributeIteratorFirst(Archive, NodeIndex);
    while (Iterator) {
        ArchiveString* AttributeName = ArchiveAttributeGetName(Archive, Iterator->Index);
        ArchiveString* AttributeData = ArchiveAttributeGetData(Archive, Iterator->Index);

        fprintf(File, " %.*s=\"%.*s\"", AttributeName->Length, AttributeName->Data, AttributeData->Length, AttributeData->Data);

        Iterator = ArchiveAttributeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveNodeIteratorFirst(Archive, NodeIndex);
    if (!Iterator) {
        fprintf(File, " />");

        if (Prettify)
            fprintf(File, "\n");

        return true;
    }
    else {
        fprintf(File, ">");

        if (Prettify)
            fprintf(File, "\n");
    }

    while (Iterator) {
        if (!ArchiveNodeWriteToFile(Archive, Iterator->Index, File, Prettify, Indentation + 1)) return false;

        Iterator = ArchiveNodeIteratorNext(Archive, Iterator);
    }

    if (Prettify)
        for (Int32 Index = 0; Index < Indentation * 4; Index++)
            fprintf(File, " ");

    fprintf(File, "</%.*s>", NodeName->Length, NodeName->Data);

    if (Prettify)
        fprintf(File, "\n");

    return true;
}

Bool ArchiveWriteToFileHandle(
    Archive* Archive,
    FILE* File,
    Bool Prettify,
    Int32 Indentation
) {
    if (!File) goto error;

    ArchiveIterator* Iterator = ArchiveNodeIteratorFirst(Archive, -1);
    while (Iterator) {
        if (!ArchiveNodeWriteToFile(Archive, Iterator->Index, File, Prettify, Indentation)) return false;

        Iterator = ArchiveNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ArchiveWriteToFile(
    Archive* Archive,
    CString FilePath,
    Bool Prettify
) {
    FILE* File = fopen(FilePath, "w");
    if (!File) goto error;

    ArchiveIterator* Iterator = ArchiveNodeIteratorFirst(Archive, -1);
    while (Iterator) {
        if (!ArchiveNodeWriteToFile(Archive, Iterator->Index, File, Prettify, 0)) return false;

        Iterator = ArchiveNodeIteratorNext(Archive, Iterator);
    }

    fclose(File);
    return true;

error:
    if (File) fclose(File);

    return false;
}

Void ArchiveClear(
    Archive* Archive,
    Bool KeepCapacity
) {
    for (Int32 Index = 0; Index < Archive->Nodes.Count; Index++) {
        ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, Index);
        ArrayDeinitialize(&Node->AttributeIndices);
    }

    ArrayRemoveAllElements(&Archive->NameTable, KeepCapacity);
    ArrayRemoveAllElements(&Archive->DataTable, KeepCapacity);
    ArrayRemoveAllElements(&Archive->Nodes, KeepCapacity);
    ArrayRemoveAllElements(&Archive->Attributes, KeepCapacity);
}

Void ArchiveUnload(
    Archive* Archive
) {
    for (Int32 Index = 0; Index < Archive->Nodes.Count; Index++) {
        ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, Index);
        ArrayDeinitialize(&Node->AttributeIndices);
    }

    ArrayDeinitialize(&Archive->NameTable);
    ArrayDeinitialize(&Archive->DataTable);
    ArrayDeinitialize(&Archive->Nodes);
    ArrayDeinitialize(&Archive->Attributes);
}

static inline Bool ArchiveContainsNode(
    Archive* Archive,
    Int32 NodeIndex
) {
    return 0 <= NodeIndex && NodeIndex < Archive->Nodes.Count;
}

static inline Int32 ArchiveAddName(
    Archive* Archive,
    CString Name,
    Int32 Length
) {
    Int32 Index = Archive->NameTable.Count;
    Bool IsZeroTerminated = Length > 0 && Name[Length - 1] == 0;
    Int32 StringLength = IsZeroTerminated ? Length : Length + 1;

    ArrayAppendMemory(&Archive->NameTable, (UInt8*)&StringLength, sizeof(Int32));

    if (Length > 0) {
        ArrayAppendMemory(&Archive->NameTable, Name, Length);
    }

    if (!IsZeroTerminated) {
        UInt8 Zero = 0;
        ArrayAppendMemory(&Archive->NameTable, &Zero, 1);
    }

    return Index;
}

static inline Int32 ArchiveLookupName(
    Archive* Archive,
    CString Name,
    Int32 Length
) {
    UInt8* Cursor = Archive->NameTable.Memory;
    UInt8* End = Archive->NameTable.Memory + Archive->NameTable.Count;
    Bool IsZeroTerminated = Name[Length - 1] == 0;

    while (Cursor < End) {
        ArchiveString* String = (ArchiveString*)Cursor;
        if ((IsZeroTerminated && String->Length == Length) ||
            (!IsZeroTerminated && String->Length - 1 == Length)) {
            if (memcmp(Name, String->Data, MIN(String->Length, Length)) == 0) {
                return Cursor - Archive->NameTable.Memory;
            }
        }

        Cursor += sizeof(Int32) + String->Length;
    }

    return -1;
}

static inline Int32 ArchiveAddData(
    Archive* Archive,
    CString Data,
    Int32 Length
) {
    Int32 Index = Archive->DataTable.Count;
    Bool IsZeroTerminated = Length > 0 && Data[Length - 1] == 0;
    Int32 StringLength = IsZeroTerminated ? Length : Length + 1;

    ArrayAppendMemory(&Archive->DataTable, (UInt8*)&StringLength, sizeof(Int32));

    if (Length > 0) {
        ArrayAppendMemory(&Archive->DataTable, Data, Length);
    }

    if (!IsZeroTerminated) {
        UInt8 Zero = 0;
        ArrayAppendMemory(&Archive->DataTable, &Zero, 1);
    }

    return Index;
}

Int32 ArchiveAddNode(
    Archive* Archive,
    Int32 ParentIndex,
    CString Name,
    Int32 NameLength
) {
    assert(ParentIndex == -1 || ArchiveContainsNode(Archive, ParentIndex));
   
    ArchiveNode* Node = (ArchiveNode*)ArrayAppendUninitializedElement(&Archive->Nodes);
    Node->Index = Archive->Nodes.Count - 1;
    Node->ParentIndex = ParentIndex;
    Node->NameIndex = ArchiveLookupName(Archive, Name, NameLength);
    if (Node->NameIndex < 0) {
        Node->NameIndex = ArchiveAddName(Archive, Name, NameLength);
    }

    assert(0 <= Node->NameIndex);
    
    ArrayInitializeEmpty(&Node->AttributeIndices, sizeof(Int32), 0);

    return Node->Index;
}

ArchiveString* ArchiveNodeGetName(
    Archive* Archive,
    Int32 NodeIndex
) {
    ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, NodeIndex);
    return (ArchiveString*)ArrayGetElementAtIndex(&Archive->NameTable, Node->NameIndex);
}

Int32 ArchiveNodeGetParent(
    Archive* Archive,
    Int32 NodeIndex
) {
    ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, NodeIndex);
    return Node->ParentIndex;
}

Int32 ArchiveNodeGetChildByPath(
    Archive* Archive,
    Int32 NodeIndex,
    CString Path
) {
    CString Cursor = strchr(Path, '.');
    if (!Cursor) {
        ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(
            Archive,
            NodeIndex,
            Path
        );

        return Iterator ? Iterator->Index : -1;
    }

    Int32 Length = Cursor - Path;
    if (Length < 1) return -1;

    Char Name[64];
    assert(Length < 64);
    memcpy(Name, Path, Length);
    Name[Length] = '\0';

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(
        Archive, 
        NodeIndex, 
        Name
    );

    if (!Iterator) return -1;

    Int32 Offset = Length + 1;
    Length = strlen(Path) - Offset;
    memcpy(Name, Path + Offset, Length);
    Name[Length] = '\0';

    return ArchiveNodeGetChildByPath(Archive, Iterator->Index, Name);
}

Int32 _ArchiveNodeGetAttributeByNameWithLength(
    Archive* Archive,
    Int32 NodeIndex,
    CString Name,
    Int32 NameLength
) {
    Int32 NameIndex = ArchiveLookupName(Archive, Name, NameLength);
    if (NameIndex < 0) return -1;

    ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, NodeIndex);

    for (Int32 Index = 0; Index < Node->AttributeIndices.Count; Index++) {
        Int32 AttributeIndex = *(Int32*)ArrayGetElementAtIndex(&Node->AttributeIndices, Index);

        ArchiveAttribute* Attribute = (ArchiveAttribute*)ArrayGetElementAtIndex(
            &Archive->Attributes,
            AttributeIndex
        );

        if (Attribute->NameIndex == NameIndex) {
            return Attribute->Index;
        }
    }

    return -1;
}

Int32 ArchiveNodeAddAttribute(
    Archive* Archive,
    Int32 NodeIndex,
    CString Name,
    Int32 NameLength,
    CString Data,
    Int32 DataLength
) {
    assert(ArchiveContainsNode(Archive, NodeIndex));

    ArchiveAttribute* Attribute = (ArchiveAttribute*)ArrayAppendUninitializedElement(&Archive->Attributes);
    Attribute->Index = Archive->Attributes.Count - 1;
    Attribute->NodeIndex = NodeIndex;
    Attribute->NameIndex = ArchiveLookupName(Archive, Name, NameLength);
    if (Attribute->NameIndex < 0) {
        Attribute->NameIndex = ArchiveAddName(Archive, Name, NameLength);
    }
    assert(0 <= Attribute->NameIndex);

    Attribute->DataIndex = ArchiveAddData(Archive, Data, DataLength);
    assert(0 <= Attribute->DataIndex);

    ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, NodeIndex);
    ArrayAppendElement(&Node->AttributeIndices, &Attribute->Index);

    return Attribute->Index;
}

Int32 ArchiveNodeGetAttributeByName(
    Archive* Archive,
    Int32 NodeIndex,
    CString Name
) {
    return _ArchiveNodeGetAttributeByNameWithLength(Archive, NodeIndex, Name, strlen(Name));
}

ArchiveString* ArchiveAttributeGetName(
    Archive* Archive,
    Int32 AttributeIndex
) {
    ArchiveAttribute* Attribute = (ArchiveAttribute*)ArrayGetElementAtIndex(
        &Archive->Attributes, 
        AttributeIndex
    );
    return (ArchiveString*)ArrayGetElementAtIndex(&Archive->NameTable, Attribute->NameIndex);
}

ArchiveString* ArchiveAttributeGetData(
    Archive* Archive,
    Int32 AttributeIndex
) {
    ArchiveAttribute* Attribute = (ArchiveAttribute*)ArrayGetElementAtIndex(
        &Archive->Attributes,
        AttributeIndex
    );
    return (ArchiveString*)ArrayGetElementAtIndex(&Archive->DataTable, Attribute->DataIndex);
}

Int32 ArchiveQueryNodeWithAttribute(
    Archive* Archive,
    Int32 ParentIndex,
    CString Query,
    CString AttributeName,
    CString AttributeValue
) {
    Int32 NodeIndex = ArchiveNodeGetChildByPath(Archive, ParentIndex, Query);
    if (NodeIndex < 0) return -1;

    Int32 QueryIndex = ArchiveNodeGetParent(Archive, NodeIndex);
    CString SubQuery = strrchr(Query, '.');
    if (SubQuery) {
        SubQuery += 1;
    } else {
        SubQuery = Query;
    }

    Char Value[MAX_PATH] = { 0 };

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, QueryIndex, SubQuery);
    while (Iterator) {
        if (ParseAttributeString(Archive, Iterator->Index, AttributeName, Value, MAX_PATH) &&
            strcmp(AttributeValue, Value) == 0) {
            return Iterator->Index;
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return -1;
}

ArchiveIterator* ArchiveQueryNodeIteratorByPathFirst(
    Archive* Archive,
    Int32 NodeIndex,
    CString Path
) {
    CString Cursor = strchr(Path, '.');
    if (!Cursor) {
        return ArchiveQueryNodeIteratorFirst(
            Archive,
            NodeIndex,
            Path
        );
    }

    Int32 Length = Cursor - Path;
    if (Length < 1) return NULL;

    Char Name[64];
    assert(Length < 64);
    memcpy(Name, Path, Length);
    Name[Length] = '\0';

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(
        Archive,
        NodeIndex,
        Name
    );

    if (!Iterator) return NULL;

    Int32 Offset = Length + 1;
    Length = strlen(Path) - Offset;
    memcpy(Name, Path + Offset, Length);
    Name[Length] = '\0';

    return ArchiveQueryNodeIteratorByPathFirst(Archive, Iterator->Index, Name);
}

ArchiveIterator* ArchiveQueryNodeIteratorFirst(
    Archive* Archive,
    Int32 ParentIndex,
    CString Query
) {
    Int32 NameIndex = ArchiveLookupName(Archive, Query, strlen(Query));
    if (NameIndex < 0) return NULL;

    for (Int32 Index = 0; Index < Archive->Nodes.Count; Index++) {
        ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, Index);
        if (Node->NameIndex == NameIndex && Node->ParentIndex == ParentIndex) {
            return (ArchiveIterator*)Node;
        }
    }

    return NULL;
}

ArchiveIterator* ArchiveQueryNodeIteratorNext(
    Archive* Archive,
    ArchiveIterator* Iterator
) {
    assert(Iterator);

    ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, Iterator->Index);

    for (Int32 Index = Iterator->Index + 1; Index < Archive->Nodes.Count; Index++) {
        ArchiveNode* Next = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, Index);
        if (Next->NameIndex == Node->NameIndex && Next->ParentIndex == Node->ParentIndex) {
            return (ArchiveIterator*)Next;
        }
    }

    return NULL;
}

ArchiveIterator* ArchiveNodeIteratorFirst(
    Archive* Archive,
    Int32 ParentIndex
) {
    for (Int32 Index = 0; Index < Archive->Nodes.Count; Index++) {
        ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, Index);
        if (Node->ParentIndex == ParentIndex) {
            return (ArchiveIterator*)Node;
        }
    }

    return NULL;
}

ArchiveIterator* ArchiveNodeIteratorNext(
    Archive* Archive,
    ArchiveIterator* Iterator
) {
    assert(Iterator);

    ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, Iterator->Index);
    for (Int32 Index = Iterator->Index + 1; Index < Archive->Nodes.Count; Index++) {
        ArchiveNode* Next = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, Index);
        if (Next->ParentIndex == Node->ParentIndex) {
            return (ArchiveIterator*)Next;
        }
    }

    return NULL;
}

ArchiveIterator* ArchiveAttributeIteratorFirst(
    Archive* Archive,
    Int32 NodeIndex
) {
    ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(&Archive->Nodes, NodeIndex);

    if (Node->AttributeIndices.Count < 1) return NULL;

    return (ArchiveIterator*)ArrayGetElementAtIndex(&Node->AttributeIndices, 0);
}

ArchiveIterator* ArchiveAttributeIteratorNext(
    Archive* Archive,
    ArchiveIterator* Iterator
) {
    assert(Iterator);

    ArchiveAttribute* Attribute = (ArchiveAttribute*)ArrayGetElementAtIndex(
        &Archive->Attributes,
        Iterator->Index
    );
    ArchiveNode* Node = (ArchiveNode*)ArrayGetElementAtIndex(
        &Archive->Nodes, 
        Attribute->NodeIndex
    );

    Int32 FirstAttributeIndex = *(Int32*)ArrayGetElementAtIndex(&Node->AttributeIndices, 0);
    ArchiveAttribute* FirstAttribute = (ArchiveAttribute*)ArrayGetElementAtIndex(
        &Archive->Attributes,
        FirstAttributeIndex
    );

    Int32 NextIndex = ((Int32)Attribute - (Int32)FirstAttribute) / sizeof(ArchiveAttribute) + 1;

    if (NextIndex >= Node->AttributeIndices.Count) return NULL;

    return (ArchiveIterator*)ArrayGetElementAtIndex(&Node->AttributeIndices, NextIndex);
}
