#ifndef WAVFILE_H
#define WAVFILE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>


// The header of a wave file
struct WaveHeader{
    char chunkID[4];      // Must be "RIFF" (0x52494646)
    char dataSize[4];     // Byte count for the rest of the file (i.e. file length - 8 bytes)
    char riffType[4];     // Must be "WAVE" (0x57415645)
};

// The format chunk of a wave file
struct FormatChunk{
    char chunkID[4];                  // String: must be "fmt " (0x666D7420).
    char chunkDataSize[4];            // Unsigned 4-byte little endian int: Byte count for the remainder of the chunk: 16 + extraFormatbytes.
    char compressionCode[2];          // Unsigned 2-byte little endian int
    char numberOfChannels[2];         // Unsigned 2-byte little endian int
    char sampleRate[4];               // Unsigned 4-byte little endian int
    char averageBytesPerSecond[4];    // Unsigned 4-byte little endian int: This value indicates how many bytes of wave data must be streamed to a D/A converter per second in order to play the wave file. This information is useful when determining if data can be streamed from the source fast enough to keep up with playback. = SampleRate * BlockAlign.
    char blockAlign[2];               // Unsigned 2-byte little endian int: The number of bytes per sample slice. This value is not affected by the number of channels and can be calculated with the formula: blockAlign = significantBitsPerSample / 8 * numberOfChannels
    char significantBitsPerSample[2]; // Unsigned 2-byte little endian int
};

struct DataChunk{
    char chunkID[4];              // String: must be "data"
    char chunkDataSize[4];        // Unsigned 4-byte little endian int
    char *waveformData;
    int isWaveformDataOwned;     // Is wave form data owned by this structure
};

// CuePoint: each individual 'marker' in a wave file is represented by a cue point.
struct CuePoint{
    char cuePointID[4];        // a unique ID for the Cue Point.
    char playOrderPosition[4]; // Unsigned 4-byte little endian int: If a Playlist chunk is present in the Wave file, this the sample number at which this cue point will occur during playback of the entire play list as defined by the play list's order.  **Otherwise set to same as sample offset??***  Set to 0 when there is no playlist.
    char dataChunkID[4];       // Unsigned 4-byte little endian int: The ID of the chunk containing the sample data that corresponds to this cue point.  If there is no playlist, this should be 'data'.
    char chunkStart[4];        // Unsigned 4-byte little endian int: The byte offset into the Wave List Chunk of the chunk containing the sample that corresponds to this cue point. This is the same chunk described by the Data Chunk ID value. If no Wave List Chunk exists in the Wave file, this value is 0.
    char blockStart[4];        // Unsigned 4-byte little endian int: The byte offset into the "data" or "slnt" Chunk to the start of the block containing the sample. The start of a block is defined as the first byte in uncompressed PCM wave data or the last byte in compressed wave data where decompression can begin to find the value of the corresponding sample value.
    char frameOffset[4];       // Unsigned 4-byte little endian int: The offset into the block (specified by Block Start) for the sample that corresponds to the cue point.
};

// CuePoints are stored in a CueChunk
struct CueChunk{
    char chunkID[4];        // String: Must be "cue " (0x63756520).
    char chunkDataSize[4];  // Unsigned 4-byte little endian int: Byte count for the remainder of the chunk: 4 (size of cuePointsCount) + (24 (size of CuePoint struct) * number of CuePoints).
    char cuePointsCount[4]; // Unsigned 4-byte little endian int: Length of cuePoints[].
    CuePoint *cuePoints;
};

// LablChunk
struct LablChunk{
    char chunkID[4];        // String: Must be "labl" (0x6C61626C)
    char chunkDataSize[4];
    char cuePointID[4];
    char *text;
};

// LtxtChunk
struct LtxtChunk{
    char chunkID[4];        // String: Must be "ltxt" (0x6C747874)
    char chunkDataSize[4];
    char cuePointID[4];
    char sampleLength[4];
    char purposeID[4];
    char country[2];
    char language[2];
    char dialect[2];
    char codePage[2];
    char *text;
};

// Names are stored in a ListChunk
struct ListChunk{
    char chunkID[4];        // String: Must be "list" (0x6C696E74).
    char chunkDataSize[4];  // Unsigned 4-byte little endian int: Byte count for the remainder of the chunk: 4 (size of typeID) + (24 (size of ListItem struct) * number of ListItems).
    char typeID[4];         // "adtl" (0x6164746C)
    uint32_t ltxtCount;
    LtxtChunk *ltxtChunks;
    uint32_t lablCount;
    LablChunk *lablChunks;
};

// Some chunks we don't care about the contents and will just copy them from the input file to the output,
// so this struct just stores positions of such chunks in the input file
struct ChunkLocation{
    long startOffset; // in bytes
    long size;		  // in bytes
};

// Wave file
struct WaveFile{
    char *filePath;
    FILE *file;
    WaveHeader *waveHeader;
    FormatChunk *formatChunk;
    DataChunk *dataChunk;
    CueChunk *cueChunk;
    uint32_t listCount;
    ListChunk *listChunks;
};

WaveFile * initWaveFile();

// Open WaveFile
WaveFile * waveOpenHFile(int);
WaveFile * waveOpenFile(const char*);

// Create WaveFile functions
WaveFile * makeWaveFile(
        WaveHeader *waveHeader,
        FormatChunk *formatChunk,
        DataChunk *dataChunk,
        CueChunk *cueChunk,
        LtxtChunk *ltxtChunks,
        uint32_t ltxtChunksCount,
        LablChunk *lablChunks,
        uint32_t lablChunksCount
);
WaveFile * makeWaveFileFromRawData(
        char *waveformData,
        uint32_t chunkDataSize,
        uint16_t numberOfChannels,
        uint32_t sampleRate,
        uint16_t significantBitsPerSample,
        uint32_t pointsCount,
        uint32_t *pointsOffset,
        uint32_t *pointsLenght,
        char **pointsLabels
);

// Create Chunks functions
WaveHeader * makeWaveHeader(
        uint32_t uint32
);
FormatChunk * makeFormatChunk(
        uint16_t numberOfChannels,
        uint32_t sampleRate,
        uint16_t significantBitsPerSample
);
DataChunk * makeDataChunk(
        uint32_t chunkDataSize,
        char *waveformData
);
CueChunk * makeCueChunk(
        uint32_t cuePointsCount,
        CuePoint *cuePoints
);
CuePoint * makeCuePoint(
        uint32_t cuePointID,
        uint32_t playOrderPosition,
        uint32_t chunkStart,
        uint32_t blockStart,
        uint32_t frameOffset
);
ListChunk * makeListChunk(
        uint32_t ltxtCount,
        LtxtChunk *ltxtChunks,
        uint32_t lablCount,
        LablChunk *lablChunks
);
LtxtChunk * makeLtxtChunk(
        uint32_t cuePointID,
        uint32_t sampleLength,
        uint16_t country,
        uint16_t language,
        uint16_t dialect,
        uint16_t codePage,
        char *text
);
LablChunk * makeLablChunk(
        uint32_t cuePointID,
        char *text
);

// Save WaveFile
void saveWaveFile(WaveFile *waveFile, const char *filePath);

// Close WaveFile
void waveCloseFile(WaveFile*);

// Clone WaveFile
WaveFile * waveCloneFile(WaveFile*);

// All data in a Wave file must be little endian.
// These are functions to convert 2- and 4-byte unsigned ints to and from little endian, if needed

enum HostEndiannessType {
    EndiannessUndefined = 0,
    LittleEndian,
    BigEndian
};

static enum HostEndiannessType HostEndianness = EndiannessUndefined;

enum HostEndiannessType getHostEndianness();
uint32_t littleEndianBytesToUInt32(char littleEndianBytes[4]);
void uint32ToLittleEndianBytes(uint32_t uInt32Value, char out_LittleEndianBytes[4]);
uint16_t littleEndianBytesToUInt16(char littleEndianBytes[2]);
void uint16ToLittleEndianBytes(uint16_t uInt16Value, char out_LittleEndianBytes[2]);

std::vector<double> waveformDataToVector(void *data, uint32_t byteSize, uint16_t bitDepth);
char* vectorToWaveformData(std::vector<double> data, uint16_t bitDepth);

#endif // WAVFILE_H
