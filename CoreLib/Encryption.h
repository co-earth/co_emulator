#include "Base.h"

EXTERN_C_BEGIN

Bool EncryptionLoadLibrary();

Bool EncryptionUnloadLibrary();

Bool EncryptionDecryptFile(
    CString filepath,
    UInt8** destination,
    Int32* destinationLength
);

EXTERN_C_END