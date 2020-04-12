#include "storage.h"

#include "define.h"
#include "config.h"

#include "modules/wav_file/wavFile.h"
#include "modules/calculation/manual_segments.h"
#include "modules/calculation/auto_segments.h"

using namespace IntonCore;

std::vector<std::pair<int, int> > Storage::getManualSegmentsP()
{
    RETURN_VALUE_IF_EXIST(this->data_manual_segments_p)

    DEBUG("Get segments P")

    WaveFile * wave_file = this->getWaveFile();

    if (!wave_file) return std::vector<std::pair<int, int> >();

    std::vector<std::pair<int, int> > segments = waveFileToManualSegmants(wave_file, WAVE_FILE_SEGMENT_PRE_NUCLEUS);

    this->data_manual_segments_p.setValue(segments);

    return segments;
}

std::vector<std::pair<int, int> > Storage::getManualSegmentsN()
{
    RETURN_VALUE_IF_EXIST(this->data_manual_segments_n)

    DEBUG("Get segments N")

    WaveFile * wave_file = this->getWaveFile();

    if (!wave_file) return std::vector<std::pair<int, int> >();

    std::vector<std::pair<int, int> > segments = waveFileToManualSegmants(wave_file, WAVE_FILE_SEGMENT_NUCLEUS);

    this->data_manual_segments_n.setValue(segments);

    return segments;
}

std::vector<std::pair<int, int> > Storage::getManualSegmentsT()
{
    RETURN_VALUE_IF_EXIST(this->data_manual_segments_t)

    DEBUG("Get segments T")

    WaveFile * wave_file = this->getWaveFile();

    if (!wave_file) return std::vector<std::pair<int, int> >();

    std::vector<std::pair<int, int> > segments = waveFileToManualSegmants(wave_file, WAVE_FILE_SEGMENT_POST_NUCLEUS);

    this->data_manual_segments_t.setValue(segments);

    return segments;
}

std::vector<std::pair<int, int> > Storage::getAutoSegmentsByIntensity()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity)

    DEBUG("Calculate segments using intensity")

    auto intensity = this->getIntensityNormalizedSmoothed();

    if (intensity.empty()) return std::vector<std::pair<int, int> >();

    std::vector<std::pair<int, int> > segments = intensityToSegments(
        intensity,
        this->config->segmentsByIntensityThresholdAbsolute(),
        this->config->segmentsByIntensityThresholdRelative(),
        this->config->segmentsByIntensityMinimumLength()
    );

    this->data_auto_segments_by_intensity.setValue(segments);

    return segments;
}

std::vector<std::pair<int, int> > Storage::getAutoSegmentsByIntensityInverted()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_inverted)

    DEBUG("Calculate segments using intensity inverted")

    auto segments = this->getAutoSegmentsByIntensity();

    if (segments.empty()) return std::vector<std::pair<int, int> >();

    std::vector<std::pair<int, int> > inverted = invertSegments(segments);

    this->data_auto_segments_by_intensity_inverted.setValue(inverted);

    return inverted;
}

std::vector<int> Storage::getAutoSegmentsByIntensityMask()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_mask)

    DEBUG("Calculate mask for segments using intensity")

    auto segments = this->getAutoSegmentsByIntensity();

    if (segments.empty()) return std::vector<int>();

    auto intensity = this->getIntensityNormalized();

    auto mask = segmentsToMask(segments, intensity.size());

    this->data_auto_segments_by_intensity_mask.setValue(mask);

    return mask;
}

std::vector<std::pair<int, int> > Storage::getAutoSegmentsByIntensitySmoothed()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_smoothed)

    DEBUG("Calculate segments using intensity smoothed")

    auto intensity = this->getIntensityNormalized();

    if (intensity.empty()) return std::vector<std::pair<int, int> >();

    auto intensity_smoothed = this->getIntensityNormalizedSmoothed();

    std::vector<std::pair<int, int> > segments = intensitySmoothedToSegments(
        intensity,
        intensity_smoothed,
        this->config->segmentsByIntensityMinimumLengthPoints(WAVE_FRAME_RATE)
        );

    this->data_auto_segments_by_intensity.setValue(segments);

    return segments;
}

std::vector<std::pair<int, int> > Storage::getAutoSegmentsByIntensitySmoothedInverted()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_smoothed_inverted)

    DEBUG("Calculate segments using intensity smoothed inverted")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return std::vector<std::pair<int, int> >();

    std::vector<std::pair<int, int> > inverted = invertSegments(segments);

    this->data_auto_segments_by_intensity_smoothed_inverted.setValue(inverted);

    return inverted;
}

std::vector<int> Storage::getAutoSegmentsByIntensitySmoothedMask()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_smoothed_mask)

    DEBUG("Calculate mask for segments using intensity smoothed")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return std::vector<int>();

    auto intensity = this->getIntensityNormalized();

    auto mask = segmentsToMask(segments, intensity.size());

    this->data_auto_segments_by_intensity_smoothed_mask.setValue(mask);

    return mask;
}
