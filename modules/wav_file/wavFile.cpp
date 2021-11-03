#include "wavFile.h"

#include "define.h"

#include <string.h>
#include <limits.h>
#include <cfloat>

#ifdef _WIN32
#include <string.h>
#include <stringapiset.h>
#endif


#define INT32 littleEndianBytesToUInt32
#define INT16 littleEndianBytesToUInt16
#define CH4(c) c[0], c[1], c[2], c[3]
#define CH2(c) c[0], c[1]

#define INT3_MAX   8388607
#define INT3_MIN   (-8388608)
#define UINT3_MAX  16777215

uint32_t to_odd(uint32_t value)
{
    if (value % 2 == 1)
    {
        value += 1;
    }
    return value;
}

WaveFile * initWaveFile()
{
    WaveFile * waveFile = new WaveFile();

    waveFile->file = nullptr;
    waveFile->formatChunk = nullptr;
    waveFile->waveHeader = nullptr;
    waveFile->dataChunk = nullptr;
    waveFile->cueChunk = nullptr;
    waveFile->filePath = nullptr;
    waveFile->listChunks = nullptr;
    waveFile->listCount = 0;

    return waveFile;
}

WaveFile * processFile(WaveFile * waveFile);

WaveFile * waveOpenHFile(int handle)
{
    WaveFile * waveFile = initWaveFile();
    if(waveFile == nullptr)
    {
        ERROR("[waveOpenHFile] Memory Allocation Error: Could not allocate memory for Wave File\n")
        return nullptr;
    }

    waveFile->filePath = nullptr;

    waveFile->file = fdopen(handle, "rb");
    if(waveFile->file == nullptr)
    {
        ERROR("[waveOpenHFile] Could not open input file %s\n", waveFile->filePath)
        waveCloseFile(waveFile);
        return nullptr;
    }
    return processFile(waveFile);
}

#ifdef _WIN32
wchar_t* toWchar(const char* ch)
{
    int new_Len = 0;
    int fn_len_s = strlen(ch);
    if(fn_len_s==0)
        return NULL;
    wchar_t* wch = (wchar_t *) malloc(sizeof(wchar_t) * MAX_PATH);
    new_Len = MultiByteToWideChar(CP_UTF8, 0, ch, fn_len_s, wch, fn_len_s);
    if(new_Len>=MAX_PATH) return NULL;
    wch[new_Len] = L'\0';

    return wch;
}
#endif

FILE* ifopen(const char*filename, const char*mode)
{
#ifdef _WIN32
    wchar_t* path = toWchar(filename);
    wchar_t* wmode = toWchar(mode);

    FILE *f = _wfopen(path, wmode);

    free(path);
    free(wmode);
    return f;
#else
    return fopen(filename, mode);
#endif
}

WaveFile * waveOpenFile(const char* path)
{    
    WaveFile * waveFile = initWaveFile();
    if(waveFile == nullptr)
    {
        ERROR("[waveOpenFile] Memory Allocation Error: Could not allocate memory for Wave File\n")
        return nullptr;
    }

    waveFile->filePath = (char *)calloc(strlen(path)+1, sizeof(char));
    strncpy(waveFile->filePath, path, strlen(path));

    waveFile->file = ifopen(waveFile->filePath, "rb");
    if(waveFile->file == nullptr)
    {
        ERROR("[waveOpenFile] Could not open input file %s\n", waveFile->filePath)
        waveCloseFile(waveFile);
        return nullptr;
    }

    waveFile = processFile(waveFile);

    if (waveFile != nullptr && waveFile->file != nullptr)
    {
        fclose(waveFile->file);
        waveFile->file = nullptr;
    }

    return waveFile;
}

WaveFile * processFile(WaveFile * waveFile)
{
    ChunkLocation formatChunkExtraBytes = {0,0};
    ChunkLocation dataChunkLocation = {0,0};
    ChunkLocation otherChunkLocation = {0,0};

    waveFile->waveHeader = (WaveHeader *) malloc(sizeof(WaveHeader));
    if(waveFile->waveHeader == nullptr)
    {
        ERROR("[processFile] Memory Allocation Error: Could not allocate memory for Wave File Header\n")
        waveCloseFile(waveFile);
        return nullptr;
    }
    fread(waveFile->waveHeader, sizeof(WaveHeader), 1, waveFile->file);
    if (ferror(waveFile->file) != 0)
    {
        ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
        waveCloseFile(waveFile);
        return nullptr;
    }

    if (strncmp(&(waveFile->waveHeader->chunkID[0]), "RIFF", 4) != 0)
    {
        ERROR("[processFile] Input file is not a RIFF file\n")
        waveCloseFile(waveFile);
        return nullptr;
    }

    if (strncmp(&(waveFile->waveHeader->riffType[0]), "WAVE", 4) != 0)
    {
        ERROR("[processFile] Input file is not a WAVE file\n")
        waveCloseFile(waveFile);
        return nullptr;
    }

    uint32_t remainingFileSize = littleEndianBytesToUInt32(waveFile->waveHeader->dataSize);
    int size = remainingFileSize - sizeof(waveFile->waveHeader->riffType); // dataSize does not counf the chunkID or the dataSize, so remove the riffType size to get the length of the rest of the file.

    if (size <= 0)
    {
        ERROR("[processFile] Input file is an empty WAVE file\n")
        waveCloseFile(waveFile);
        return nullptr;
    }

    waveFile->listCount = 0;
    waveFile->listChunks = nullptr;

    while (1)
    {
        char nextChunkID[4];

        fread(&nextChunkID[0], sizeof(nextChunkID), 1, waveFile->file);
        if (feof(waveFile->file))
        {
            break;
        }

        if (ferror(waveFile->file) != 0)
        {
            ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
            waveCloseFile(waveFile);
            return nullptr;
        }

        if (strncmp(&nextChunkID[0], "fmt ", 4) == 0)
        {
            INFO("[processFile] Found FMT chunk\n")
            waveFile->formatChunk = (FormatChunk *)malloc(sizeof(FormatChunk));
            if (waveFile->formatChunk == NULL)
            {
                ERROR("[processFile] Memory Allocation Error: Could not allocate memory for Wave File Format Chunk\n")
                waveCloseFile(waveFile);
                return nullptr;
            }

            fseek(waveFile->file, -4, SEEK_CUR);
            fread(waveFile->formatChunk, sizeof(FormatChunk), 1, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            if (littleEndianBytesToUInt16(waveFile->formatChunk->compressionCode) != (uint16_t)1)
            {
                ERROR("[processFile] Compressed audio formats are not supported\n")
                waveCloseFile(waveFile);
                return nullptr;
            }

            // Note: For compressed audio data there may be extra bytes appended to the format chunk,
            // but as we are only handling uncompressed data we shouldn't encounter them

            // There may or may not be extra data at the end of the fomat chunk.  For uncompressed audio there should be no need, but some files may still have it.
            // if formatChunk.chunkDataSize > 16 (16 = the number of bytes for the format chunk, not counting the 4 byte ID and the chunkDataSize itself) there is extra data
            uint32_t extraFormatBytesCount = littleEndianBytesToUInt32(waveFile->formatChunk->chunkDataSize) - 16;
            if (extraFormatBytesCount > 0)
            {
                formatChunkExtraBytes.startOffset = ftell(waveFile->file);
                formatChunkExtraBytes.size = extraFormatBytesCount;
                fseek(waveFile->file, extraFormatBytesCount, SEEK_CUR);
                if (extraFormatBytesCount % 2 != 0)
                {
                    fseek(waveFile->file, 1, SEEK_CUR);
                }
            }
        }

        else if (strncmp(&nextChunkID[0], "data", 4) == 0)
        {
            INFO("[processFile] Found DATA chunk\n")

            waveFile->dataChunk = (DataChunk *)malloc(sizeof(DataChunk));
            if (waveFile->dataChunk == nullptr)
            {
                ERROR("[processFile] Memory Allocation Error: Could not allocate memory for Wave File Data Chunk\n")
                waveCloseFile(waveFile);
                return nullptr;
            }

            waveFile->dataChunk->chunkID[0] = 'd';
            waveFile->dataChunk->chunkID[1] = 'a';
            waveFile->dataChunk->chunkID[2] = 't';
            waveFile->dataChunk->chunkID[3] = 'a';

            dataChunkLocation.startOffset = ftell(waveFile->file) - sizeof(nextChunkID);

            // The next 4 bytes are the chunk data size - the size of the sample data
            char sampleDataSizeBytes[4];
            fread(sampleDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }
            uint32_t sampleDataSize = littleEndianBytesToUInt32(sampleDataSizeBytes);

            dataChunkLocation.size = sizeof(nextChunkID) + sizeof(sampleDataSizeBytes) + sampleDataSize;

            uint32ToLittleEndianBytes(sampleDataSize, waveFile->dataChunk->chunkDataSize);

            // TODO: check if no errors in allocate and read data
            waveFile->dataChunk->waveformData = (char *) malloc(sampleDataSize);
            fread(waveFile->dataChunk->waveformData, sampleDataSize, 1, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            // Skip to the end of the chunk.  Chunks must be aligned to 2 byte boundaries, but any padding at the end of a chunk is not included in the chunkDataSize
            if (sampleDataSize % 2 != 0)
            {
                fseek(waveFile->file, 1, SEEK_CUR);
            }
        }

        else if (strncmp(&nextChunkID[0], "cue ", 4) == 0)
        {
            INFO("[processFile] Found CUE chunk\n")

            char cueChunkDataSizeBytes[4];
            fread(cueChunkDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }
            uint32_t cueChunkDataSize = littleEndianBytesToUInt32(cueChunkDataSizeBytes);

            char cuePointsCountBytes[4];
            fread(cuePointsCountBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }
            uint32_t cuePointsCount = littleEndianBytesToUInt32(cuePointsCountBytes);

            // Read in the existing cue points into CuePoint Structs
            CuePoint *existingCuePoints = (CuePoint *)malloc(sizeof(CuePoint) * cuePointsCount);
            for (uint32_t cuePointIndex = 0; cuePointIndex < cuePointsCount; cuePointIndex++)
            {
                CuePoint * cue_point = &existingCuePoints[cuePointIndex];
                INFO("[processFile] Found CUE POINT %i\n", cuePointIndex)
                fread(cue_point, sizeof(CuePoint), 1, waveFile->file);
                if (ferror(waveFile->file) != 0)
                {
                    ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                    waveCloseFile(waveFile);
                    free(existingCuePoints);
                    return nullptr;
                }
                DEBUG("[processFile] Read CUE POINT cuePointID %i\n", INT32(cue_point->cuePointID))
                DEBUG("[processFile] Read CUE POINT playOrderPosition %i\n", INT32(cue_point->playOrderPosition))
                DEBUG("[processFile] Read CUE POINT dataChunkID %c%c%c%c\n", CH4(cue_point->dataChunkID))
                DEBUG("[processFile] Read CUE POINT chunkStart %i\n", INT32(cue_point->chunkStart))
                DEBUG("[processFile] Read CUE POINT blockStart %i\n", INT32(cue_point->blockStart))
                DEBUG("[processFile] Read CUE POINT frameOffset %i\n", INT32(cue_point->frameOffset))
            }

            // Populate the existingCueChunk struct
            waveFile->cueChunk = (CueChunk *) malloc(sizeof(CueChunk));
            if (waveFile->dataChunk == nullptr)
            {
                ERROR("[processFile] Memory Allocation Error: Could not allocate memory for Wave File Cue Chunk\n")
                waveCloseFile(waveFile);
                free(existingCuePoints);
                return nullptr;
            }
            waveFile->cueChunk->chunkID[0] = 'c';
            waveFile->cueChunk->chunkID[1] = 'u';
            waveFile->cueChunk->chunkID[2] = 'e';
            waveFile->cueChunk->chunkID[3] = ' ';
            uint32ToLittleEndianBytes(cueChunkDataSize, waveFile->cueChunk->chunkDataSize);
            uint32ToLittleEndianBytes(cuePointsCount, waveFile->cueChunk->cuePointsCount);
            waveFile->cueChunk->cuePoints = existingCuePoints;

            DEBUG("[processFile] Read CUE chunkID %c%c%c%c\n", CH4(waveFile->cueChunk->chunkID))
            DEBUG("[processFile] Read CUE chunkDataSize %i\n", INT32(waveFile->cueChunk->chunkDataSize))
            DEBUG("[processFile] Read CUE cuePointsCount %i\n", INT32(waveFile->cueChunk->cuePointsCount))
        }
        else if (strncmp(&nextChunkID[0], "LIST", 4) == 0)
        {
            INFO("[processFile] Found LIST chunk\n")
            // We found an existing List Chunk
            waveFile->listChunks = (ListChunk*)realloc(waveFile->listChunks, sizeof(ListChunk)*(waveFile->listCount+1));

            ListChunk * listChunk = &waveFile->listChunks[waveFile->listCount];

            waveFile->listCount++;

            if (waveFile->dataChunk == nullptr)
            {
                ERROR("[processFile] Memory Allocation Error: Could not allocate memory for Wave File Cue Chunk\n")
                waveCloseFile(waveFile);
                return nullptr;
            }
            listChunk->chunkID[0] = 'L';
            listChunk->chunkID[1] = 'I';
            listChunk->chunkID[2] = 'S';
            listChunk->chunkID[3] = 'T';

            listChunk->lablChunks = nullptr;
            listChunk->ltxtChunks = nullptr;
            listChunk->lablCount = 0;
            listChunk->ltxtCount = 0;

            char listChunkDataSizeBytes[4];
            fread(listChunkDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }
            uint32_t listChunkDataSize = littleEndianBytesToUInt32(listChunkDataSizeBytes);
            uint32ToLittleEndianBytes(listChunkDataSize, listChunk->chunkDataSize);

            fread(listChunk->typeID, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }
            DEBUG("[processFile] Read LIST chunkID %c%c%c%c\n", CH4(listChunk->chunkID))
            DEBUG("[processFile] Read LIST chunkDataSize %i\n", INT32(listChunk->chunkDataSize))
            DEBUG("[processFile] Read LIST typeID %c%c%c%c\n", CH4(listChunk->typeID))
        }
        else if (strncmp(&nextChunkID[0], "ltxt", 4) == 0)
        {
            INFO("[processFile] Found LTXT chunk\n")
            ListChunk * listChunk = &(waveFile->listChunks[waveFile->listCount-1]);
            INFO("[processFile] listCount %i\n", waveFile->listCount)

            // We found an existing LTXT chunk in LIST chunk
            listChunk->ltxtChunks = (LtxtChunk *)realloc(listChunk->ltxtChunks, sizeof(LtxtChunk)*(listChunk->ltxtCount+1));

            LtxtChunk * ltxtChunk = &(listChunk->ltxtChunks[listChunk->ltxtCount]);

            listChunk->ltxtCount++;

            if (ltxtChunk == nullptr)
            {
                ERROR("[processFile] Memory Allocation Error: Could not allocate memory for Wave File LABL chunk\n")
                waveCloseFile(waveFile);
                return nullptr;
            }
            ltxtChunk->chunkID[0] = 'l';
            ltxtChunk->chunkID[1] = 't';
            ltxtChunk->chunkID[2] = 'x';
            ltxtChunk->chunkID[3] = 't';

            char ltxtChunkDataSizeBytes[4];
            fread(ltxtChunkDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }
            uint32_t ltxtChunkDataSize = littleEndianBytesToUInt32(ltxtChunkDataSizeBytes);
            uint32ToLittleEndianBytes(ltxtChunkDataSize, ltxtChunk->chunkDataSize);

            fread(ltxtChunk->cuePointID, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            fread(ltxtChunk->sampleLength, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            fread(ltxtChunk->purposeID, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            fread(ltxtChunk->country, sizeof(char), 2, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            fread(ltxtChunk->language, sizeof(char), 2, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            fread(ltxtChunk->dialect, sizeof(char), 2, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            fread(ltxtChunk->codePage, sizeof(char), 2, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            DEBUG("[processFile] LTXT dataSize %d\n", ltxtChunkDataSize)

            ltxtChunkDataSize = to_odd(ltxtChunkDataSize);

            if(ltxtChunkDataSize > 20) {
                ltxtChunk->text = (char *) malloc(ltxtChunkDataSize-20);
                fread(ltxtChunk->text, ltxtChunkDataSize-20, 1, waveFile->file);

                DEBUG("[processFile] LTXT text %s\n", ltxtChunk->text)
            } else {
                ltxtChunk->text = nullptr;

                WARN("[processFile] LTXT text <skip>\n")
            }
            DEBUG("[processFile] Read LTXT chunkID %c%c%c%c\n", CH4(ltxtChunk->chunkID))
            DEBUG("[processFile] Read LTXT chunkDataSize %i\n", INT32(ltxtChunk->chunkDataSize))
            DEBUG("[processFile] Read LTXT cuePointID %i\n", INT32(ltxtChunk->cuePointID))
            DEBUG("[processFile] Read LTXT sampleLength %i\n", INT32(ltxtChunk->sampleLength))
            DEBUG("[processFile] Read LTXT purposeID %c%c%c%c\n", CH4(ltxtChunk->purposeID))
            DEBUG("[processFile] Read LTXT country %i\n", INT16(ltxtChunk->country))
            DEBUG("[processFile] Read LTXT language %i\n", INT16(ltxtChunk->language))
            DEBUG("[processFile] Read LTXT dialect %i\n", INT16(ltxtChunk->dialect))
            DEBUG("[processFile] Read LTXT codePage %i\n", INT16(ltxtChunk->codePage))
        }
        else if (strncmp(&nextChunkID[0], "labl", 4) == 0)
        {
            INFO("[processFile] Found LABL chunk\n")
            ListChunk * listChunk = &waveFile->listChunks[waveFile->listCount-1];

            // We found an existing LABL chunk in LIST chunk
            listChunk->lablChunks = (LablChunk *)realloc(listChunk->lablChunks, sizeof(LablChunk)*(listChunk->lablCount+1));

            LablChunk * lablChunk = &listChunk->lablChunks[listChunk->lablCount];

            listChunk->lablCount++;

            if (lablChunk == nullptr)
            {
                ERROR("[processFile] Memory Allocation Error: Could not allocate memory for Wave File LABL chunk\n")
                waveCloseFile(waveFile);
                return nullptr;
            }
            lablChunk->chunkID[0] = 'l';
            lablChunk->chunkID[1] = 'a';
            lablChunk->chunkID[2] = 'b';
            lablChunk->chunkID[3] = 'l';

            char lablChunkDataSizeBytes[4];
            fread(lablChunkDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }
            uint32_t lablChunkDataSize = littleEndianBytesToUInt32(lablChunkDataSizeBytes);
            uint32ToLittleEndianBytes(lablChunkDataSize, lablChunk->chunkDataSize);

            fread(lablChunk->cuePointID, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }

            DEBUG("[processFile] LABL dataSize %d\n", lablChunkDataSize)

            lablChunkDataSize = to_odd(lablChunkDataSize);

            if (lablChunkDataSize > 4)
            {
                lablChunk->text = (char *) malloc(lablChunkDataSize - 4);
                fread(lablChunk->text, sizeof(char), lablChunkDataSize - 4, waveFile->file);

                DEBUG("[processFile] LABL text %s\n", lablChunk->text)
            } else {
                lablChunk->text = nullptr;

                WARN("[processFile] LABL text <skip>\n");
            }
            DEBUG("[processFile] Read LABL chunkID %c%c%c%c\n", CH4(lablChunk->chunkID))
            DEBUG("[processFile] Read LABL chunkDataSize %i\n", INT32(lablChunk->chunkDataSize))
            DEBUG("[processFile] Read LABL cuePointID %i\n", INT32(lablChunk->cuePointID))
        }
        else
        {
            WARN("[processFile] Found unsuppoted chunk %s\n", nextChunkID)
            // We have found a chunk type that we are not going to work with.
            // ISFTL
            // IENGL
            // ICRDL
            otherChunkLocation.startOffset = ftell(waveFile->file) - sizeof(nextChunkID);

            char chunkDataSizeBytes[4] = {0};
            fread(chunkDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                ERROR("[processFile] Error reading input file %s\n", waveFile->filePath)
                waveCloseFile(waveFile);
                return nullptr;
            }
            uint32_t chunkDataSize = littleEndianBytesToUInt32(chunkDataSizeBytes);

            otherChunkLocation.size = sizeof(nextChunkID) + sizeof(chunkDataSizeBytes) + chunkDataSize;

            // Skip over the chunk's data, and any padding byte
            fseek(waveFile->file, chunkDataSize, SEEK_CUR);
            if (chunkDataSize % 2 != 0)
            {
                fseek(waveFile->file, 1, SEEK_CUR);
            }
        }
    }

    return waveFile;
}

void waveCloseFile(WaveFile *waveFile)
{
    // TODO: free allocated memory in stucts
    if(waveFile == nullptr) return;
    if(waveFile->file != nullptr)
    {
        fclose(waveFile->file);
        waveFile->file = nullptr;
    }
    if(waveFile->formatChunk != nullptr) free(waveFile->formatChunk);
    if(waveFile->waveHeader != nullptr) free(waveFile->waveHeader);
    if(waveFile->dataChunk != nullptr)
    {
        if (waveFile->dataChunk->isWaveformDataOwned == 1)
        {
            free(waveFile->dataChunk->waveformData);
        }
        free(waveFile->dataChunk);
    }
    if(waveFile->cueChunk != nullptr) free(waveFile->cueChunk);
    if(waveFile->listChunks != nullptr)
    {
        free(waveFile->listChunks);
    }
    if(waveFile->filePath != nullptr) free(waveFile->filePath);
    free(waveFile);
}

WaveFile * waveCloneFile(WaveFile *waveFile)
{
    WaveFile *clone = initWaveFile();

    clone->waveHeader = makeWaveHeader(
        INT32(waveFile->waveHeader->dataSize)
    );

    clone->formatChunk = makeFormatChunk(
        INT16(waveFile->formatChunk->numberOfChannels),
        INT32(waveFile->formatChunk->sampleRate),
        INT16(waveFile->formatChunk->significantBitsPerSample)
    );

    char * data = (char*) malloc(INT32(waveFile->dataChunk->chunkDataSize));
    memcpy(data, waveFile->dataChunk->waveformData, INT32(waveFile->dataChunk->chunkDataSize));
    clone->dataChunk = makeDataChunk(
        INT32(waveFile->dataChunk->chunkDataSize),
        data
    );

    CuePoint * cuePoints = (CuePoint *) malloc(sizeof(CuePoint) * INT32(waveFile->cueChunk->cuePointsCount));
    for (int i=0; i<INT32(waveFile->cueChunk->cuePointsCount); i++)
    {
        cuePoints[i] = *makeCuePoint(
            INT32(waveFile->cueChunk->cuePoints[i].cuePointID),
            INT32(waveFile->cueChunk->cuePoints[i].playOrderPosition),
            INT32(waveFile->cueChunk->cuePoints[i].chunkStart),
            INT32(waveFile->cueChunk->cuePoints[i].blockStart),
            INT32(waveFile->cueChunk->cuePoints[i].frameOffset)
        );
    }
    clone->cueChunk = makeCueChunk(
        INT32(waveFile->cueChunk->cuePointsCount),
        cuePoints
    );

    LtxtChunk * ltxtChunks = (LtxtChunk *) malloc(sizeof(LtxtChunk) * INT32(waveFile->cueChunk->cuePointsCount));
    LablChunk * lablChunks = (LablChunk *) malloc(sizeof(LablChunk) * INT32(waveFile->cueChunk->cuePointsCount));
    for (int i=0; i<INT32(waveFile->cueChunk->cuePointsCount); i++)
    {
        ltxtChunks[i] = *makeLtxtChunk(
            INT32(waveFile->listChunks[0].ltxtChunks[i].cuePointID),
            INT32(waveFile->listChunks[0].ltxtChunks[i].sampleLength),
            INT16(waveFile->listChunks[0].ltxtChunks[i].country),
            INT16(waveFile->listChunks[0].ltxtChunks[i].language),
            INT16(waveFile->listChunks[0].ltxtChunks[i].dialect),
            INT16(waveFile->listChunks[0].ltxtChunks[i].codePage),
            waveFile->listChunks[0].ltxtChunks[i].text
        );
        lablChunks[i] = *makeLablChunk(
            INT32(waveFile->listChunks[0].lablChunks[i].cuePointID),
            waveFile->listChunks[0].lablChunks[i].text
        );
    }
    clone->listCount = 1;
    clone->listChunks = makeListChunk(
        INT32(waveFile->cueChunk->cuePointsCount),
        ltxtChunks,
        INT32(waveFile->cueChunk->cuePointsCount),
        lablChunks
    );

    return clone;
}

WaveHeader * makeWaveHeader(uint32_t dataSize)
{
    WaveHeader * header = (WaveHeader *) malloc(sizeof(WaveHeader));
    uint32ToLittleEndianBytes(dataSize, header->dataSize);
    header->chunkID[0] = 'R';
    header->chunkID[1] = 'I';
    header->chunkID[2] = 'F';
    header->chunkID[3] = 'F';
    header->riffType[0] = 'W';
    header->riffType[1] = 'A';
    header->riffType[2] = 'V';
    header->riffType[3] = 'E';
    return header;
}

FormatChunk * makeFormatChunk(
        uint16_t numberOfChannels,
        uint32_t sampleRate,
        uint16_t significantBitsPerSample
) {
    FormatChunk * format = (FormatChunk *) malloc(sizeof(FormatChunk));
    format->chunkID[0] = 'f';
    format->chunkID[1] = 'm';
    format->chunkID[2] = 't';
    format->chunkID[3] = ' ';
    // Unsigned 4-byte little endian int: Byte count for the remainder of the chunk: 16 + extraFormatbytes.
    uint32ToLittleEndianBytes(16, format->chunkDataSize);
    // Unsigned 2-byte little endian int
    uint16ToLittleEndianBytes(1, format->compressionCode);
    // Unsigned 2-byte little endian int
    uint16ToLittleEndianBytes(numberOfChannels, format->numberOfChannels);
    // Unsigned 4-byte little endian int
    uint32ToLittleEndianBytes(sampleRate, format->sampleRate);
    // Unsigned 2-byte little endian int: The number of bytes per sample slice. This value is not affected by the
    // number of channels and can be calculated with the formula: blockAlign = significantBitsPerSample / 8 * numberOfChannels
    uint16_t blockAlign = significantBitsPerSample / 8 * numberOfChannels;
    uint16ToLittleEndianBytes(blockAlign, format->blockAlign);
    // Unsigned 4-byte little endian int: This value indicates how many bytes of wave data must be streamed to a
    // D/A converter per second in order to play the wave file. This information is useful when determining if data
    // can be streamed from the source fast enough to keep up with playback. = SampleRate * BlockAlign.
    uint32ToLittleEndianBytes(sampleRate * blockAlign, format->averageBytesPerSecond);
    // Unsigned 2-byte little endian int
    uint16ToLittleEndianBytes(significantBitsPerSample, format->significantBitsPerSample);
    return format;
}

DataChunk * makeDataChunk(
        uint32_t waveformDataSize,
        char *waveformData
) {
    DataChunk * data = (DataChunk *) malloc(sizeof(DataChunk));
    data->chunkID[0] = 'd';
    data->chunkID[1] = 'a';
    data->chunkID[2] = 't';
    data->chunkID[3] = 'a';
    uint32ToLittleEndianBytes(waveformDataSize, data->chunkDataSize);
    data->waveformData = (char *)malloc(waveformDataSize);
    memcpy(data->waveformData, waveformData, waveformDataSize);
    data->isWaveformDataOwned = 1;
    return data;
}

CueChunk * makeCueChunk(
        uint32_t cuePointsCount,
        CuePoint *cuePoints
) {
    CueChunk * chunk = (CueChunk *) malloc(sizeof(CueChunk));
    chunk->chunkID[0] = 'c';
    chunk->chunkID[1] = 'u';
    chunk->chunkID[2] = 'e';
    chunk->chunkID[3] = ' ';
    uint32_t chunkDataSize = 4 + 24 * cuePointsCount;
    uint32ToLittleEndianBytes(chunkDataSize, chunk->chunkDataSize);
    uint32ToLittleEndianBytes(cuePointsCount, chunk->cuePointsCount);
    chunk->cuePoints = cuePoints;
    return chunk;
}

CuePoint * makeCuePoint(
        uint32_t cuePointID,
        uint32_t playOrderPosition,
        uint32_t chunkStart,
        uint32_t blockStart,
        uint32_t frameOffset
) {
    CuePoint * chunk = (CuePoint *) malloc(sizeof(CuePoint));
    uint32ToLittleEndianBytes(cuePointID, chunk->cuePointID);
    uint32ToLittleEndianBytes(playOrderPosition, chunk->playOrderPosition);
    chunk->dataChunkID[0] = 'd';
    chunk->dataChunkID[1] = 'a';
    chunk->dataChunkID[2] = 't';
    chunk->dataChunkID[3] = 'a';
    uint32ToLittleEndianBytes(chunkStart, chunk->chunkStart);
    uint32ToLittleEndianBytes(blockStart, chunk->blockStart);
    uint32ToLittleEndianBytes(frameOffset, chunk->frameOffset);
    return chunk;
}

ListChunk * makeListChunk(
        uint32_t ltxtCount,
        LtxtChunk *ltxtChunks,
        uint32_t lablCount,
        LablChunk *lablChunks
) {
    ListChunk * chunk = (ListChunk *) malloc(sizeof(ListChunk));

    chunk->ltxtCount = 0;
    chunk->ltxtChunks = nullptr;
    chunk->lablCount = 0;
    chunk->lablChunks = nullptr;

    chunk->chunkID[0] = 'L';
    chunk->chunkID[1] = 'I';
    chunk->chunkID[2] = 'S';
    chunk->chunkID[3] = 'T';

    chunk->typeID[0] = 'a';
    chunk->typeID[1] = 'd';
    chunk->typeID[2] = 't';
    chunk->typeID[3] = 'l';

    uint32_t chunkDataSize = 4;

    if (ltxtCount > 0)
    {
        for (int i=0; i<ltxtCount; i++)
        {
            chunkDataSize += 8 + littleEndianBytesToUInt32(ltxtChunks[i].chunkDataSize);
        }
        uint32ToLittleEndianBytes(chunkDataSize, chunk->chunkDataSize);
        chunk->ltxtCount = ltxtCount;
        chunk->ltxtChunks = ltxtChunks;
    }

    if (lablCount > 0)
    {
        for (int i=0; i<lablCount; i++)
        {
            chunkDataSize += 8 + littleEndianBytesToUInt32(lablChunks[i].chunkDataSize);
        }
        uint32ToLittleEndianBytes(chunkDataSize, chunk->chunkDataSize);
        chunk->lablCount = lablCount;
        chunk->lablChunks = lablChunks;
    }

    return chunk;
}

LtxtChunk * makeLtxtChunk(
        uint32_t cuePointID,
        uint32_t sampleLength,
        uint16_t country,
        uint16_t language,
        uint16_t dialect,
        uint16_t codePage,
        char *text
) {
    LtxtChunk * chunk = (LtxtChunk *) malloc(sizeof(LtxtChunk));
    chunk->chunkID[0] = 'l';
    chunk->chunkID[1] = 't';
    chunk->chunkID[2] = 'x';
    chunk->chunkID[3] = 't';
    chunk->purposeID[0] = 'r';
    chunk->purposeID[1] = 'g';
    chunk->purposeID[2] = 'n';
    chunk->purposeID[3] = ' ';
    uint32ToLittleEndianBytes(cuePointID, chunk->cuePointID);
    uint32ToLittleEndianBytes(sampleLength, chunk->sampleLength);
    if (country != 0) uint16ToLittleEndianBytes(country, chunk->country);
    else uint16ToLittleEndianBytes(0, chunk->country);
    if (language != 0) uint16ToLittleEndianBytes(language, chunk->language);
    else uint16ToLittleEndianBytes(0, chunk->language);
    if (dialect != 0) uint16ToLittleEndianBytes(dialect, chunk->dialect);
    else uint16ToLittleEndianBytes(0, chunk->dialect);
    if (codePage != 0) uint16ToLittleEndianBytes(codePage, chunk->codePage);
    else uint16ToLittleEndianBytes(0, chunk->codePage);
    uint32_t chunkDataSize = 20;
    if (text)
    {
        int text_size = to_odd((unsigned)strlen(text)+1);
        chunk->text = (char*) malloc(sizeof(char)*text_size);
        chunk->text[text_size-1] = 0;
        chunk->text[text_size-2] = 0;
        strcpy(chunk->text, text);

        chunkDataSize += sizeof(char) * text_size;
    } else {
        chunk->text = nullptr;
    }
    uint32ToLittleEndianBytes(chunkDataSize, chunk->chunkDataSize);
    return chunk;
}

LablChunk * makeLablChunk(
        uint32_t cuePointID,
        char *text
) {
    LablChunk * chunk = (LablChunk *) malloc(sizeof(LablChunk));
    chunk->chunkID[0] = 'l';
    chunk->chunkID[1] = 'a';
    chunk->chunkID[2] = 'b';
    chunk->chunkID[3] = 'l';
    uint32_t chunkDataSize = 4;
    uint32ToLittleEndianBytes(cuePointID, chunk->cuePointID);
    if (text)
    {
        int text_size = to_odd((unsigned)strlen(text)+1);
        chunk->text = (char*) malloc(sizeof(char)*text_size);
        chunk->text[text_size-1] = 0;
        chunk->text[text_size-2] = 0;
        strcpy(chunk->text, text);

        chunkDataSize += sizeof(char) * text_size;
    } else {
        chunk->text = nullptr;
    }
    uint32ToLittleEndianBytes(chunkDataSize, chunk->chunkDataSize);
    return chunk;
}

WaveFile * makeWaveFile(
        WaveHeader *waveHeader,
        FormatChunk *formatChunk,
        DataChunk *dataChunk,
        CueChunk *cueChunk,
        LtxtChunk *ltxtChunks,
        uint32_t ltxtChunksCount,
        LablChunk *lablChunks,
        uint32_t lablChunksCount
) {
    WaveFile *wave = initWaveFile();

    wave->file = nullptr;
    wave->waveHeader = waveHeader;
    wave->formatChunk = formatChunk;
    wave->dataChunk = dataChunk;
    wave->cueChunk = cueChunk;
    wave->listCount = 1;
    wave->listChunks = makeListChunk(ltxtChunksCount, ltxtChunks, lablChunksCount, lablChunks);

    uint32_t fileSize = 4;
    if(formatChunk != nullptr)
        fileSize += 8 + INT32(formatChunk->chunkDataSize);
    if(dataChunk != nullptr)
        fileSize += 8 + INT32(dataChunk->chunkDataSize);
    if(cueChunk != nullptr)
        fileSize += 8 + INT32(cueChunk->chunkDataSize);
    if(wave->listChunks != nullptr)
        fileSize += 8 + INT32(wave->listChunks->chunkDataSize);
    uint32ToLittleEndianBytes(fileSize, waveHeader->dataSize);

    return wave;
}

WaveFile * makeWaveFileFromRawData(
        char *waveformData,
        uint32_t waveformDataSize,
        uint16_t numberOfChannels,
        uint32_t sampleRate,
        uint16_t significantBitsPerSample,
        uint32_t pointsCount,
        uint32_t *pointsOffset,
        uint32_t *pointsLenght,
        char **pointsLabels
) {
    WaveHeader *headerChunk = makeWaveHeader(0);
    FormatChunk *formatChunk = makeFormatChunk(numberOfChannels, sampleRate, significantBitsPerSample);
    DataChunk *dataChunk = makeDataChunk(waveformDataSize, waveformData);
    dataChunk->isWaveformDataOwned = 0;

    CueChunk *cueChunk = nullptr;
    LtxtChunk *ltxtChunks = nullptr;
    uint32_t ltxtChunksCount = 0;
    LablChunk *lablChunks = nullptr;
    uint32_t lablChunksCount = 0;

    if (pointsCount > 0)
    {
        ltxtChunksCount = pointsCount;
        lablChunksCount = pointsCount;
        CuePoint * cuePoints = (CuePoint *) malloc(sizeof(CuePoint) * pointsCount);
        ltxtChunks = (LtxtChunk *) malloc(sizeof(LtxtChunk) * pointsCount);
        lablChunks = (LablChunk *) malloc(sizeof(LablChunk) * pointsCount);

        for (uint32_t i=0; i<pointsCount; i++)
        {
            uint32_t pintId = i + 1;
            CuePoint * cuePoint = makeCuePoint(
                        pintId,
                        pointsOffset[i],
                        0,
                        0,
                        pointsOffset[i]
            );
            cuePoints[i] = *cuePoint;
            free(cuePoint);

            LtxtChunk * ltxtChunk = makeLtxtChunk(
                        pintId,
                        pointsLenght[i],
                        0,
                        0,
                        0,
                        0,
                        nullptr
            );
            ltxtChunks[i] = *ltxtChunk;
            free(ltxtChunk);

            LablChunk * lablChunk = makeLablChunk(
                        pintId,
                        pointsLabels[i]
            );
            lablChunks[i] = *lablChunk;
            free(lablChunk);
        }

        cueChunk = makeCueChunk(pointsCount, cuePoints);
    }

    return makeWaveFile(
                headerChunk,
                formatChunk,
                dataChunk,
                cueChunk,
                ltxtChunks,
                ltxtChunksCount,
                lablChunks,
                lablChunksCount
    );
}

void saveWaveFile(WaveFile *waveFile, const char *filePath)
{
    if (filePath != nullptr)
    {
        waveFile->filePath = (char *)malloc(1 + strlen(filePath));
        strcpy(waveFile->filePath, filePath);
    }
    if (waveFile->file == nullptr) waveFile->file = ifopen(filePath, "wb");
    if (waveFile->file == nullptr)
    {
        ERROR("[saveWaveFile] Could not open output file %s\n", filePath)
        return;
    }
    if (waveFile->waveHeader != nullptr)
    {
        INFO("[saveWaveFile] Write HEADER chunk\n")
        if (fwrite(waveFile->waveHeader, sizeof(WaveHeader), 1, waveFile->file) < 1)
        {
            ERROR("[saveWaveFile] Error writing header to output file\n")
            return;
        }
    }
    if (waveFile->formatChunk != nullptr)
    {
        INFO("[saveWaveFile] Write FMT chunk\n")
        if (fwrite(waveFile->formatChunk, sizeof(FormatChunk), 1, waveFile->file) < 1)
        {
            ERROR("[saveWaveFile] Error writing format chunk to output file\n")
            return;
        }
    }
    if (waveFile->dataChunk != nullptr)
    {
        INFO("[saveWaveFile] Write DATA chunk\n")
        uint32_t dataChunkSize = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
        if (fwrite(waveFile->dataChunk->chunkID, sizeof(char), 4, waveFile->file) < 1)
        {
            ERROR("[saveWaveFile] Error writing data chunk (chunkID) to output file\n")
            return;
        }
        if (fwrite(waveFile->dataChunk->chunkDataSize, sizeof(char), 4, waveFile->file) < 1)
        {
            ERROR("[saveWaveFile] Error writing data chunk (chunkDataSize) to output file\n")
            return;
        }
        if (fwrite(waveFile->dataChunk->waveformData, dataChunkSize, 1, waveFile->file) < 1)
        {
            DEBUG("[saveWaveFile] Size data chunk (waveformData) %d\n", dataChunkSize)
            ERROR("[saveWaveFile] Error writing data chunk (waveformData) to output file\n")
            return;
        }
        if (dataChunkSize % 2 != 0)
        {
            if (fwrite("\0", sizeof(char), 1, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing padding character to output file\n")
                return;

            }
        }
    }
    if  (waveFile->cueChunk != nullptr)
    {
        INFO("[saveWaveFile] Write CUE chunk\n")
        CueChunk * cue_chunk = waveFile->cueChunk;

        DEBUG("[saveWaveFile] Write CUE chunkID %c%c%c%c\n", CH4(cue_chunk->chunkID))
        if (fwrite(cue_chunk->chunkID, sizeof(char), 4, waveFile->file) < 1)
        {
            ERROR("[saveWaveFile] Error writing cue chunk (chunkID) to output file\n")
            return;
        }

        DEBUG("[saveWaveFile] Write CUE chunkDataSize %i\n", INT32(cue_chunk->chunkDataSize))
        if (fwrite(cue_chunk->chunkDataSize, sizeof(char), 4, waveFile->file) < 1)
        {
            ERROR("[saveWaveFile] Error writing cue chunk (chunkDataSize) to output file\n")
            return;
        }

        DEBUG("[saveWaveFile] Write CUE cuePointsCount %i\n", INT32(cue_chunk->cuePointsCount))
        if (fwrite(cue_chunk->cuePointsCount, sizeof(char), 4, waveFile->file) < 1)
        {
            ERROR("[saveWaveFile] Error writing cue chunk (cuePointsCount) to output file\n")
            return;
        }

        uint32_t cuePointsCount = littleEndianBytesToUInt32(cue_chunk->cuePointsCount);
        for (uint32_t i=0; i<cuePointsCount; i++)
        {
            INFO("[saveWaveFile] Write CUE POINT %i\n", i)
            CuePoint * cue_point = &cue_chunk->cuePoints[i];

            DEBUG("[saveWaveFile] Write CUE POINT cuePointID %i\n", INT32(cue_point->cuePointID))
            if (fwrite(cue_point->cuePointID, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing cue point (cuePointID) to output file\n")
                return;
            }

            DEBUG("[saveWaveFile] Write CUE POINT playOrderPosition %i\n", INT32(cue_point->playOrderPosition))
            if (fwrite(cue_point->playOrderPosition, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing cue point (playOrderPosition) to output file\n")
                return;
            }

            DEBUG("[saveWaveFile] Write CUE POINT dataChunkID %c%c%c%c\n", CH4(cue_point->dataChunkID))
            if (fwrite(cue_point->dataChunkID, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing cue point (dataChunkID) to output file\n")
                return;
            }

            DEBUG("[saveWaveFile] Write CUE POINT chunkStart %i\n", INT32(cue_point->chunkStart))
            if (fwrite(cue_point->chunkStart, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing cue point (chunkStart) to output file\n")
                return;
            }

            DEBUG("[saveWaveFile] Write CUE POINT blockStart %i\n", INT32(cue_point->blockStart))
            if (fwrite(cue_point->blockStart, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing cue point (blockStart) to output file\n")
                return;
            }

            DEBUG("[saveWaveFile] Write CUE POINT frameOffset %i\n", INT32(cue_point->frameOffset))
            if (fwrite(cue_point->frameOffset, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing cue point (frameOffset) to output file\n")
                return;
            }
        }
    }
    if  (waveFile->listChunks != nullptr)
    {
        INFO("[saveWaveFile] Write LIST chunk\n")
        for (int listChunkIndex=0; listChunkIndex<waveFile->listCount; listChunkIndex++)
        {
            ListChunk * list_chunk = &waveFile->listChunks[listChunkIndex];

            if (list_chunk->lablCount == 0 && list_chunk->ltxtCount == 0) continue;

            DEBUG("[saveWaveFile] Write LIST chunkID %c%c%c%c\n", CH4(list_chunk->chunkID))
            if (fwrite(list_chunk->chunkID, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing list chunk (chunkID) to output file\n")
                return;
            }

            DEBUG("[saveWaveFile] Write LIST chunkDataSize %i\n", INT32(list_chunk->chunkDataSize))
            if (fwrite(list_chunk->chunkDataSize, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing list chunk (chunkDataSize) to output file\n")
                return;
            }

            DEBUG("[saveWaveFile] Write LIST typeID %c%c%c%c\n", CH4(list_chunk->typeID))
            if (fwrite(list_chunk->typeID, sizeof(char), 4, waveFile->file) < 1)
            {
                ERROR("[saveWaveFile] Error writing list chunk (typeID) to output file\n")
                return;
            }

            for( int i=0; i<list_chunk->ltxtCount; i++)
            {
                INFO("[saveWaveFile] Write LTXT chunk %i\n", i)
                LtxtChunk * ltxt_chank = &list_chunk->ltxtChunks[i];

                DEBUG("[saveWaveFile] Write LTXT chunkID %c%c%c%c\n", CH4(ltxt_chank->chunkID))
                if (fwrite(ltxt_chank->chunkID, sizeof(char), 4, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (chunkID) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LTXT chunkDataSize %i\n", INT32(ltxt_chank->chunkDataSize))
                if (fwrite(ltxt_chank->chunkDataSize, sizeof(char), 4, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (chunkDataSize) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LTXT cuePointID %i\n", INT32(ltxt_chank->cuePointID))
                if (fwrite(ltxt_chank->cuePointID, sizeof(char), 4, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (cuePointID) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LTXT sampleLength %i\n", INT32(ltxt_chank->sampleLength))
                if (fwrite(ltxt_chank->sampleLength, sizeof(char), 4, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (sampleLength) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LTXT purposeID %c%c%c%c\n", CH4(ltxt_chank->purposeID))
                if (fwrite(ltxt_chank->purposeID, sizeof(char), 4, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (purposeID) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LTXT country %i\n", INT16(ltxt_chank->country))
                if (fwrite(ltxt_chank->country, sizeof(char), 2, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (country) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LTXT language %i\n", INT16(ltxt_chank->language))
                if (fwrite(ltxt_chank->language, sizeof(char), 2, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (language) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LTXT dialect %i\n", INT16(ltxt_chank->dialect))
                if (fwrite(ltxt_chank->dialect, sizeof(char), 2, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (dialect) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LTXT codePage %i\n", INT16(ltxt_chank->codePage))
                if (fwrite(ltxt_chank->codePage, sizeof(char), 2, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LTXT chunk (codePage) to output file\n")
                    return;
                }

                if (ltxt_chank->text)
                {
                    INFO("[saveWaveFile] Write LTXT text %s\n", ltxt_chank->text)
                    uint32_t size = littleEndianBytesToUInt32(ltxt_chank->chunkDataSize) - 20;
                    if (fwrite(ltxt_chank->text, sizeof(char), size, waveFile->file) < 1)
                    {
                        ERROR("[saveWaveFile] Error writing LTXT chunk (text) to output file\n")
                        return;
                    }
                } else {
                    WARN("[saveWaveFile] LTXT text <skip>\n")
                }
            }

            for(int i=0; i<list_chunk->lablCount; i++)
            {
                INFO("[saveWaveFile] Write LABL chunk %i\n", i)
                LablChunk * labl_chank = &list_chunk->lablChunks[i];

                DEBUG("[saveWaveFile] Write LABL chunkID %c%c%c%c\n", CH4(labl_chank->chunkID))
                if (fwrite(labl_chank->chunkID, sizeof(char), 4, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LABL chunk (chunkID) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LABL chunkDataSize %i\n", INT32(labl_chank->chunkDataSize))
                if (fwrite(labl_chank->chunkDataSize, sizeof(char), 4, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LABL chunk (chunkDataSize) to output file\n")
                    return;
                }

                DEBUG("[saveWaveFile] Write LABL cuePointID %i\n", INT32(labl_chank->cuePointID))
                if (fwrite(labl_chank->cuePointID, sizeof(char), 4, waveFile->file) < 1)
                {
                    ERROR("[saveWaveFile] Error writing LABL chunk (cuePointID) to output file\n")
                    return;
                }

                if (labl_chank->text)
                {
                    INFO("[saveWaveFile] Write LABL text %s\n", labl_chank->text)
                    uint32_t size = littleEndianBytesToUInt32(labl_chank->chunkDataSize) - 4;
                    if(fwrite(labl_chank->text, sizeof(char), size, waveFile->file) < 1)
                    {
                        ERROR("[saveWaveFile] Error writing LABL chunk (text) to output file\n")
                        return;
                    }
                } else {
                    WARN("[saveWaveFile] LABL text <skip>\n")
                }
            }
        }
    }
    fclose(waveFile->file);
    waveFile->file = nullptr;
}

enum HostEndiannessType getHostEndianness()
{
    int i = 1;
    char *p = (char *)&i;

    if (p[0] == 1)
        return LittleEndian;
    else
        return BigEndian;
}

uint32_t littleEndianBytesToUInt32(char littleEndianBytes[4])
{
    if (HostEndianness == EndiannessUndefined)
    {
        HostEndianness = getHostEndianness();
    }

    uint32_t uInt32Value;
    char *uintValueBytes = (char *)&uInt32Value;

    if (HostEndianness == LittleEndian)
    {
        uintValueBytes[0] = littleEndianBytes[0];
        uintValueBytes[1] = littleEndianBytes[1];
        uintValueBytes[2] = littleEndianBytes[2];
        uintValueBytes[3] = littleEndianBytes[3];
    }
    else
    {
        uintValueBytes[0] = littleEndianBytes[3];
        uintValueBytes[1] = littleEndianBytes[2];
        uintValueBytes[2] = littleEndianBytes[1];
        uintValueBytes[3] = littleEndianBytes[0];
    }

    return uInt32Value;
}

void uint32ToLittleEndianBytes(uint32_t uInt32Value, char out_LittleEndianBytes[4])
{
    if (HostEndianness == EndiannessUndefined)
    {
        HostEndianness = getHostEndianness();
    }

    char *uintValueBytes = (char *)&uInt32Value;

    if (HostEndianness == LittleEndian)
    {
        out_LittleEndianBytes[0] = uintValueBytes[0];
        out_LittleEndianBytes[1] = uintValueBytes[1];
        out_LittleEndianBytes[2] = uintValueBytes[2];
        out_LittleEndianBytes[3] = uintValueBytes[3];
    }
    else
    {
        out_LittleEndianBytes[0] = uintValueBytes[3];
        out_LittleEndianBytes[1] = uintValueBytes[2];
        out_LittleEndianBytes[2] = uintValueBytes[1];
        out_LittleEndianBytes[3] = uintValueBytes[0];
    }
}

uint16_t littleEndianBytesToUInt16(char littleEndianBytes[2])
{
    if (HostEndianness == EndiannessUndefined)
    {
        HostEndianness = getHostEndianness();
    }

    uint32_t uInt16Value;
    char *uintValueBytes = (char *)&uInt16Value;

    if (HostEndianness == LittleEndian)
    {
        uintValueBytes[0] = littleEndianBytes[0];
        uintValueBytes[1] = littleEndianBytes[1];
    }
    else
    {
        uintValueBytes[0] = littleEndianBytes[1];
        uintValueBytes[1] = littleEndianBytes[0];
    }

    return uInt16Value;
}

void uint16ToLittleEndianBytes(uint16_t uInt16Value, char out_LittleEndianBytes[2])
{
    if (HostEndianness == EndiannessUndefined)
    {
        HostEndianness = getHostEndianness();
    }

    char *uintValueBytes = (char *)&uInt16Value;

    if (HostEndianness == LittleEndian)
    {
        out_LittleEndianBytes[0] = uintValueBytes[0];
        out_LittleEndianBytes[1] = uintValueBytes[1];
    }
    else
    {
        out_LittleEndianBytes[0] = uintValueBytes[1];
        out_LittleEndianBytes[1] = uintValueBytes[0];
    }
}

std::vector<double> waveformDataToVector(void *data, uint32_t byteSize, uint16_t bitDepth)
{
    char c1 = 's',
         c2 = 'd';
    unsigned int i;
    unsigned int len = byteSize / (bitDepth/CHAR_BIT);
    double x2;
    bool clip = true;
    signed short int *vals = static_cast<signed short int *>(data);

    void x2x(void *x1, void *x2, char c1, char c2, int clip);

    std::vector<double> res;

    i = 0;
    while (len > i) {
       x2x(vals, &x2, c1, c2, clip);
       res.push_back(x2);
       i++;
       vals++;
    }

    return res;
}

void x2x(void *x1, void *x2, char c1, char c2, int clip)
{
    double r = 0.0;

   long long xl = 0;
   unsigned long long xul = 0;
   long double xd = 0.0;
   int y = 0;
   switch (c1) {
   case 's':
      xl = *(short *) x1;
      break;
   case 'i':
      xl = *(int *) x1;
      break;
   case 'l':
      xl = *(long *) x1;
      break;
   case 'u':
      xl = *(long long *) x1;
      break;
   case 'S':
      xul = *(unsigned short *) x1;
      break;
   case 'I':
      xul = *(unsigned int *) x1;
      break;
   case 'L':
      xul = *(unsigned long *) x1;
      break;
   case 'U':
      xul = *(unsigned long long *) x1;
      break;
   case 'f':
      xd = *(float *) x1;
      break;
   case 'd':
      xd = *(double *) x1;
      break;
   case 'v':
      xd = *(long double *) x1;
      break;
   case 'c':
      xl = *(char *) x1;
      break;
   case 'C':
      xul = *(unsigned char *) x1;
      break;
   case 't':                   /* 3byte, signed */
      y = *(int *) x1 & 0x00FFFFFF;
      if (y >> 23 == 1)
         y = y | 0xFF000000;
      xl = y;
      break;
   case 'T':                   /* 3byte, unsigned */
      xul = *(unsigned int *) x1 & 0x00FFFFFF;
      break;
   }

   if (clip) {
      switch (c2) {
      case 's':
         if (xl > SHRT_MAX || xl < SHRT_MIN || xul > SHRT_MAX || xd > SHRT_MAX || xd < SHRT_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'short'!\n");
         xl = ((xl < SHRT_MIN) ? SHRT_MIN : ((xl > SHRT_MAX) ? SHRT_MAX : xl));
         xul = ((xul > SHRT_MAX) ? SHRT_MAX : xul);
         xd = ((xd < SHRT_MIN) ? SHRT_MIN : ((xd > SHRT_MAX) ? SHRT_MAX : xd));
         break;
      case 'i':
         if (xl > INT_MAX || xl < INT_MIN || xul > INT_MAX || xd > INT_MAX || xd < INT_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'int'!\n");
         xl = ((xl < INT_MIN) ? INT_MIN : ((xl > INT_MAX) ? INT_MAX : xl));
         xul = ((xul > INT_MAX) ? INT_MAX : xul);
         xd = ((xd < INT_MIN) ? INT_MIN : ((xd > INT_MAX) ? INT_MAX : xd));
         break;
      case 'l':
         if (xl > LONG_MAX || xl < LONG_MIN || xul > LONG_MAX || xd > LONG_MAX || xd < LONG_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'long'!\n");
         xl = ((xl < LONG_MIN) ? LONG_MIN : ((xl > LONG_MAX) ? LONG_MAX : xl));
         xul = ((xul > LONG_MAX) ? LONG_MAX : xul);
         xd = ((xd < LONG_MIN) ? LONG_MIN : ((xd > LONG_MAX) ? LONG_MAX : xd));
         break;
      case 'u':
         if (xul > LLONG_MAX || xd > LLONG_MAX || xd < LLONG_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'long long'!\n");
         xul = ((xul > LLONG_MAX) ? LLONG_MAX : xul);
         xd = ((xd <
                LLONG_MIN) ? LLONG_MIN : ((xd > LLONG_MAX) ? LLONG_MAX : xd));
         break;
      case 'S':
         if (xl > USHRT_MAX || xl < 0 || xul > USHRT_MAX || xd > USHRT_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned short'!\n");
         xl = ((xl < 0) ? 0 : ((xl > USHRT_MAX) ? USHRT_MAX : xl));
         xul = ((xul > USHRT_MAX) ? USHRT_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > USHRT_MAX) ? USHRT_MAX : xd));
         break;
      case 'I':
         if (xl > UINT_MAX || xl < 0 || xul > UINT_MAX || xd > UINT_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned int'!\n");
         xl = ((xl < 0) ? 0 : ((xl > UINT_MAX) ? UINT_MAX : xl));
         xul = ((xul > UINT_MAX) ? UINT_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > UINT_MAX) ? UINT_MAX : xd));
         break;
      case 'L':
         if (xl > ULONG_MAX || xl < 0 || xul > ULONG_MAX || xd > ULONG_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned long'!\n");
         xl = ((xl < 0) ? 0 : ((xl > ULONG_MAX) ? ULONG_MAX : xl));
         xul = ((xul > ULONG_MAX) ? ULONG_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > ULONG_MAX) ? ULONG_MAX : xd));
         break;
      case 'U':
         if (xl < 0 || xd > ULLONG_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned long long'!\n");
         xl = ((xl < 0) ? 0 : xl);
         xd = ((xd < 0.0) ? 0.0 : ((xd > ULLONG_MAX) ? ULLONG_MAX : xd));
         break;
      case 'f':
         if (xd > FLT_MAX || xd < (FLT_MAX * (-1)))
            fprintf(stderr, "warning: input data is over the range of type 'float'!\n");
         xd = ((xd <
                (FLT_MAX * (-1))) ? (FLT_MAX * (-1)) : ((xd >
                                                         FLT_MAX) ? FLT_MAX :
                                                        xd));
         break;
      case 'd':
         if (xd > DBL_MAX || xd < (DBL_MAX * (-1)))
            fprintf(stderr, "warning: input data is over the range of type 'double'!\n");
         xd = ((xd <
                (DBL_MAX * (-1))) ? (DBL_MAX * (-1)) : ((xd >
                                                         DBL_MAX) ? DBL_MAX :
                                                        xd));
         break;
      case 'v':
         if (xd > LDBL_MAX || xd < (LDBL_MAX * (-1)))
            fprintf(stderr, "warning: input data is over the range of type 'long double'!\n");
         break;
      case 'c':
         if (xl > CHAR_MAX || xl < CHAR_MIN || xul > CHAR_MAX || xd > CHAR_MAX
             || xd < CHAR_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'char'!\n");
         xl = ((xl < CHAR_MIN) ? CHAR_MIN : ((xl > CHAR_MAX) ? CHAR_MAX : xl));
         xul = ((xul > CHAR_MAX) ? CHAR_MAX : xul);
         xd = ((xd < CHAR_MIN) ? CHAR_MIN : ((xd > CHAR_MAX) ? CHAR_MAX : xd));
         break;
      case 'C':
         if (xl > UCHAR_MAX || xl < 0.0 || xul > UCHAR_MAX || xd > UCHAR_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned char'!\n");
         xl = ((xl < 0) ? 0 : ((xl > UCHAR_MAX) ? UCHAR_MAX : xl));
         xul = ((xul > UCHAR_MAX) ? UCHAR_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > UCHAR_MAX) ? UCHAR_MAX : xd));
         break;
      case 't':
         if (xl > INT3_MAX || xl < INT3_MIN || xul > INT3_MAX || xd > INT3_MAX || xd < INT3_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'int(3byte)'!\n");
         xl = ((xl < INT3_MIN) ? INT3_MIN : ((xl > INT3_MAX) ? INT3_MAX : xl));
         xul = ((xul > INT3_MAX) ? INT3_MAX : xul);
         xd = ((xd < INT3_MIN) ? INT3_MIN : ((xd > INT3_MAX) ? INT3_MAX : xd));
         break;
      case 'T':
         if (xl > UINT3_MAX || xl < 0 || xul > UINT3_MAX || xd > UINT3_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned int(3byte)'!\n");
         xl = ((xl < 0) ? 0 : ((xl > UINT3_MAX) ? UINT3_MAX : xl));
         xul = ((xul > UINT3_MAX) ? UINT3_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > UINT3_MAX) ? UINT3_MAX : xd));
         break;
      }
   } else {
      switch (c2) {
      case 's':
         if (xl > SHRT_MAX || xl < SHRT_MIN || xul > SHRT_MAX || xd > SHRT_MAX || xd < SHRT_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'short'!\n");
            return;
         }
         break;
      case 'i':
         if (xl > INT_MAX || xl < INT_MIN || xul > INT_MAX || xd > INT_MAX || xd < INT_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'int'!\n");
            return;
         }
         break;
      case 'l':
         if (xl > LONG_MAX || xl < LONG_MIN || xul > LONG_MAX || xd > LONG_MAX || xd < LONG_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'long'!\n");
            return;
         }
         break;
      case 'u':
         if (xul > LLONG_MAX || xd > LLONG_MAX || xd < LLONG_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'long long'!\n");
            return;
         }
         break;
      case 'S':
         if (xl > USHRT_MAX || xl < 0 || xul > USHRT_MAX || xd > USHRT_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned short'!\n");
            return;
         }
         break;
      case 'I':
         if (xl > UINT_MAX || xl < 0 || xul > UINT_MAX || xd > UINT_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned int'!\n");
            return;
         }
         break;
      case 'L':
         if (xl > ULONG_MAX || xl < 0 || xul > ULONG_MAX || xd > ULONG_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned long'!\n");
            return;
         }
         break;
      case 'U':
         if (xl < 0 || xd > ULLONG_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned long long'!\n");
            return;
         }
         break;
      case 'f':
         if (xd > FLT_MAX || xd < (FLT_MAX * (-1))) {
            fprintf(stderr, "error: input data is over the range of type 'float'!\n");
            return;
         }
         break;
      case 'd':
         if (xd > DBL_MAX || xd < (DBL_MAX * (-1))) {
            fprintf(stderr, "error: input data is over the range of type 'double'!\n");
            return;
         }
         break;
      case 'v':
         if (xd > LDBL_MAX || xd < (LDBL_MAX * (-1))) {
            fprintf(stderr, "error: input data is over the range of type 'long double'!\n");
            return;
         }
         break;
      case 'c':
         if (xl > CHAR_MAX || xl < CHAR_MIN || xul > CHAR_MAX || xd > CHAR_MAX || xd < CHAR_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'char'!\n");
            return;
         }
         break;
      case 'C':
         if (xl > UCHAR_MAX || xl < 0.0 || xul > UCHAR_MAX || xd > UCHAR_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned char'!\n");
            return;
         }
         break;
      case 't':
         if (xl > INT3_MAX || xl < INT3_MIN || xul > INT3_MAX || xd > INT3_MAX || xd < INT3_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'int(3byte)'!\n");
            return;
         }
         break;
      case 'T':
         if (xl > UINT3_MAX || xl < 0 || xul > UINT3_MAX || xd > UINT3_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned int(3byte)'!\n");
            return;
         }
         break;
      }
   }

   switch (c2) {
   case 's':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(short *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(short *) x2 = xul;
      else {
         if (xd > 0)
            *(short *) x2 = xd + r;
         else
            *(short *) x2 = xd - r;
      }
      break;
   case 'i':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(int *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(int *) x2 = xul;
      else {
         if (xd > 0)
            *(int *) x2 = xd + r;
         else
            *(int *) x2 = xd - r;
      }
      break;
   case 'l':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(long *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(long *) x2 = xul;
      else {
         if (xd > 0)
            *(long *) x2 = xd + r;
         else
            *(long *) x2 = xd - r;
      }
      break;
   case 'u':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(long long *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(long long *) x2 = xul;
      else {
         if (xd > 0)
            *(long long *) x2 = xd + r;
         else
            *(long long *) x2 = xd - r;
      }
      break;
   case 'S':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned short *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned short *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned short *) x2 = xd + r;
         else
            *(unsigned short *) x2 = xd - r;
      }
      break;
   case 'I':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned int *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned int *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned int *) x2 = xd + r;
         else
            *(unsigned int *) x2 = xd - r;
      }
      break;
   case 'L':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned long *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned long *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned long *) x2 = xd + r;
         else
            *(unsigned long *) x2 = xd - r;
      }
      break;
   case 'U':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned long long *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned long long *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned long long *) x2 = xd + r;
         else
            *(unsigned long long *) x2 = xd - r;
      }
      break;
   case 'f':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(float *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(float *) x2 = xul;
      else {
         if (xd > 0)
            *(float *) x2 = xd;
         else
            *(float *) x2 = xd;
      }
      break;
   case 'd':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(double *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(double *) x2 = xul;
      else {
         if (xd > 0)
            *(double *) x2 = xd;
         else
            *(double *) x2 = xd;
      }
      break;
   case 'v':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(long double *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(long double *) x2 = xul;
      else {
         if (xd > 0)
            *(long double *) x2 = xd;
         else
            *(long double *) x2 = xd;
      }
      break;
   case 'c':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(char *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(char *) x2 = xul;
      else {
         if (xd > 0)
            *(char *) x2 = xd + r;
         else
            *(char *) x2 = xd - r;
      }
      break;
   case 'C':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned char *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned char *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned char *) x2 = xd + r;
         else
            *(unsigned char *) x2 = xd - r;
      }
      break;
   case 't':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(int *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(int *) x2 = xul;
      else {
         if (xd > 0)
            *(int *) x2 = xd + r;
         else
            *(int *) x2 = xd - r;
      }
      break;
   case 'T':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned int *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned int *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned int *) x2 = xd + r;
         else
            *(unsigned int *) x2 = xd - r;
      }
      break;
   }
   return;
}
