#include "manual_segments.h"

#include "define.h"
#include "modules/wav_file/wavFile.h"


std::vector<std::pair<uint32_t, uint32_t> > IntonCore::waveFileToManualSegmants(WaveFile *waveFile, char marker)
{
    std::vector<std::pair<uint32_t, uint32_t> > data;

    DEBUG("Get segments from file for %c", marker)

    ListChunk * listLablChunk = nullptr;
    ListChunk * listLtxtChunk = nullptr;

    for (uint32_t i=0; i < waveFile->listCount; i++)
    {
        ListChunk * listChunk = &(waveFile->listChunks[i]);
        if (listChunk == nullptr) continue;

        DEBUG("listChunk->lablCount %i", listChunk->lablCount)
        if (listChunk->lablChunks != nullptr && listChunk->lablCount > 0)
        {
            listLablChunk = listChunk;
        }

        DEBUG("listChunk->ltxtCount %i", listChunk->ltxtCount)
        if (listChunk->ltxtChunks != nullptr && listChunk->ltxtCount > 0)
        {
            listLtxtChunk = listChunk;
        }
    }

    if (!((waveFile->cueChunk != nullptr)
          && (littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount) > 0)
          && (listLtxtChunk != nullptr)
          && (listLtxtChunk->ltxtChunks != nullptr)
          && (listLtxtChunk->ltxtCount > 0)
          && (listLablChunk != nullptr)
          && (listLablChunk->lablChunks != nullptr)
          && (listLablChunk->lablCount > 0)))
        return data;

    int cuePointsCount = littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount);

    uint32_t *pointsFrom = new uint32_t[cuePointsCount] ();
    uint32_t *pointsLength = new uint32_t[cuePointsCount] ();

    DEBUG("cueChunks")
    for(int i=0; i<cuePointsCount; i++)
    {
        CuePoint point = waveFile->cueChunk->cuePoints[i];
        int id = littleEndianBytesToUInt16(point.cuePointID);
        int pos = littleEndianBytesToUInt16(point.frameOffset);
        pointsFrom[i]=pos;
        pointsLength[i]=0;
        DEBUG("cueChunk cuePointID %i frameOffset %i", id, pos)
    }

    DEBUG("ltxtCount")
    for(uint32_t i=0; i<listLtxtChunk->ltxtCount; i++)
    {
        LtxtChunk ltxt = listLtxtChunk->ltxtChunks[i];
        int id = littleEndianBytesToUInt16(ltxt.cuePointID);
        uint32_t length = littleEndianBytesToUInt32(ltxt.sampleLength);
        pointsLength[i] = length;
        DEBUG("ltxtChunk cuePointID %i sampleLength %i", id, length)
    }

    DEBUG("lablChunks")
    for(uint32_t i=0; i<listLablChunk->lablCount; i++)
    {
        LablChunk labl = listLablChunk->lablChunks[i];
        int id = littleEndianBytesToUInt16(labl.cuePointID);
        char * text = listLablChunk->lablChunks[i].text;
        if (toupper(text[0]) == toupper(marker))
        {
            data.push_back(std::pair<int, int>(pointsFrom[i], pointsLength[i]));
        }
        DEBUG("lablChunk cuePointID %i text %c", id, text[0])
    }

    free(pointsFrom);
    free(pointsLength);

    DEBUG("getLabelsFromFile return")

    return data;
}
