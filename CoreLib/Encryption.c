#include <zlib/zlib.h>

#include "Encryption.h"
#include "FileIO.h"

typedef Int32(*zlib_inflate_init_proc)(z_streamp, Int32, const Char*, Int32);
typedef Int32(*zlib_inflate_proc)(z_streamp, Int32);
typedef Int32(*zlib_inflate_end_proc)(z_streamp);

Int32 zlib_inflate_init_stub(z_streamp _0, Int32 _1, const Char* _2, Int32 _3) { return 0; }
Int32 zlib_inflate_stub(z_streamp _0, Int32 _1) { return 0; }
Int32 zlib_inflate_end_stub(z_streamp _0) { return 0; }

static HINSTANCE Library = NULL;
static zlib_inflate_init_proc InflateInit = zlib_inflate_init_stub;
static zlib_inflate_proc Inflate = zlib_inflate_stub;
static zlib_inflate_end_proc InflateEnd = zlib_inflate_end_stub;

#define ARCHIVE_LIBRARY_VERSION "1.2.8"
#define ARCHIVE_CHUNK_SIZE      16384
#define ARCHIVE_HEADER_XOR      0x57676592
#define ARCHIVE_BUFFER_SIZE     1024

Bool EncryptionLoadLibrary() {
    Library = LoadLibrary(TEXT("zlib.dll"));
    if (!Library) return false;

    zlib_inflate_init_proc InflateInitProc = (zlib_inflate_init_proc)GetProcAddress(Library, (LPCSTR)("inflateInit2_"));
    zlib_inflate_proc InflateProc = (zlib_inflate_proc)GetProcAddress(Library, (LPCSTR)("inflate"));
    zlib_inflate_end_proc InflateEndProc = (zlib_inflate_end_proc)GetProcAddress(Library, (LPCSTR)("inflateEnd"));

    if (!InflateInitProc || !InflateProc || !InflateEndProc) {
        FreeLibrary(Library);
        Library = NULL;
        return false;
    }

    InflateInit = InflateInitProc;
    Inflate = InflateProc;
    InflateEnd = InflateEndProc;
    return true;
}

Bool EncryptionUnloadLibrary() {
    Bool success = true;
    if (Library) {
        success = FreeLibrary(Library);
    }

    Library = NULL;
    return success;
}

Bool InflateDecryptBuffer(
    UInt8* source,
    Int32 sourceLength,
    UInt8** destination,
    Int32* destinationLength
) {
    z_stream stream = { 0 };
    stream.avail_in = 0;
    stream.next_in = Z_NULL;
    stream.avail_out = 0;
    stream.next_out = Z_NULL;

    Int32 status = InflateInit(&stream, -15, ARCHIVE_LIBRARY_VERSION, sizeof(stream));
    if (status != Z_OK) return false;

    Int32 outputIndex = 0;
    Int32 outputLength = ARCHIVE_BUFFER_SIZE;
    UInt8* output = malloc(outputLength);
    if (!output) goto error;

    Bool first = true;
    Int32 index = sizeof(UInt32);
    static UInt8 in[ARCHIVE_CHUNK_SIZE];
    static UInt8 out[ARCHIVE_CHUNK_SIZE];

    do {
        stream.avail_in = MIN(ARCHIVE_CHUNK_SIZE, sourceLength - index);
        if (stream.avail_in < 1) break;

        memcpy(in, &source[index], stream.avail_in);
        index += stream.avail_in;

        if (first) {
            first = false;
            *(UInt32*)in ^= ARCHIVE_HEADER_XOR;
        }

        stream.next_in = in;

        do {
            stream.avail_out = ARCHIVE_CHUNK_SIZE;
            stream.next_out = out;

            status = Inflate(&stream, Z_NO_FLUSH);
            if (status != Z_OK && status != Z_STREAM_END) goto error;

            UInt32 length = ARCHIVE_CHUNK_SIZE - stream.avail_out;
            while (outputIndex + length >= (UInt32)outputLength) {
                outputLength *= 2;
            }

            UInt8* newOutput = realloc(output, outputLength);
            if (!newOutput) goto error;
            output = newOutput;

            memcpy(&output[outputIndex], out, length);
            outputIndex += length;

        } while (stream.avail_out < 1);

    } while (status != Z_STREAM_END);

    InflateEnd(&stream);

    *destination = output;
    *destinationLength = outputIndex;

    if (outputIndex < outputLength) {
        memset(&output[outputIndex], 0, outputLength - outputIndex);
    }

    return true;

error:
    InflateEnd(&stream);

    if (output) free(output);

    *destination = NULL;
    *destinationLength = 0;

    return false;
}

Bool EncryptionDecryptFile(
    CString FilePath,
    UInt8** Destination,
    Int32* DestinationLength
) {
    UInt8* Source = NULL;
    Int32 SourceLength = 0;
    *Destination = NULL;
    *DestinationLength = 0;

    FileRef File = FileOpen(FilePath);
    if (!File) 
        goto error;

    if (!FileRead(File, &Source, &SourceLength)) 
        goto error;

    if (!InflateDecryptBuffer(Source, SourceLength, Destination, DestinationLength)) 
        goto error;

    FileClose(File);
    free(Source);

    return true;

error:
    if (File) FileClose(File);
    if (Source) free(Source);
    if (*Destination) free(*Destination);

    return false;
}
