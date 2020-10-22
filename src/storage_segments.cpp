#include "storage.h"

#include "define.h"
#include "config.h"

#include "modules/wav_file/wavFile.h"
#include "modules/calculation/manual_segments.h"
#include "modules/calculation/auto_segments.h"

using namespace IntonCore;

std::vector<std::pair<uint32_t, uint32_t> > Storage::getManualSegmentsP()
{
    RETURN_VALUE_IF_EXIST(this->data_manual_segments_p)

    DEBUG("Get segments P")

    WaveFile * wave_file = this->getWaveFile();

    if (!wave_file) return std::vector<std::pair<uint32_t, uint32_t> >();

    std::vector<std::pair<uint32_t, uint32_t> > segments = waveFileToManualSegmants(wave_file, WAVE_FILE_SEGMENT_PRE_NUCLEUS);

    this->data_manual_segments_p.setValue(segments);

    return segments;
}

std::vector<std::pair<uint32_t, uint32_t> > Storage::getManualSegmentsN()
{
    RETURN_VALUE_IF_EXIST(this->data_manual_segments_n)

    DEBUG("Get segments N")

    WaveFile * wave_file = this->getWaveFile();

    if (!wave_file) return std::vector<std::pair<uint32_t, uint32_t> >();

    std::vector<std::pair<uint32_t, uint32_t> > segments = waveFileToManualSegmants(wave_file, WAVE_FILE_SEGMENT_NUCLEUS);

    this->data_manual_segments_n.setValue(segments);

    return segments;
}

std::vector<std::pair<uint32_t, uint32_t> > Storage::getManualSegmentsT()
{
    RETURN_VALUE_IF_EXIST(this->data_manual_segments_t)

    DEBUG("Get segments T")

    WaveFile * wave_file = this->getWaveFile();

    if (!wave_file) return std::vector<std::pair<uint32_t, uint32_t> >();

    std::vector<std::pair<uint32_t, uint32_t> > segments = waveFileToManualSegmants(wave_file, WAVE_FILE_SEGMENT_POST_NUCLEUS);

    this->data_manual_segments_t.setValue(segments);

    return segments;
}

std::vector<std::pair<uint32_t, uint32_t> > Storage::getAutoSegmentsByIntensity()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity)

    DEBUG("Calculate segments using intensity")

    auto intensity = this->getIntensityNormalizedSmoothed();

    if (intensity.empty()) return std::vector<std::pair<uint32_t, uint32_t> >();

    std::vector<std::pair<uint32_t, uint32_t> > segments = intensityToSegments(
        intensity,
        this->config->segmentsByIntensityThresholdAbsolute(),
        this->config->segmentsByIntensityThresholdRelative(),
        this->config->segmentsByIntensityMinimumLength()
    );

    this->data_auto_segments_by_intensity.setValue(segments);

    return segments;
}

std::vector<std::pair<uint32_t, uint32_t> > Storage::getAutoSegmentsByIntensityInverted()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_inverted)

    DEBUG("Calculate segments using intensity inverted")

    auto segments = this->getAutoSegmentsByIntensity();

    if (segments.empty()) return std::vector<std::pair<uint32_t, uint32_t> >();

    std::vector<std::pair<uint32_t, uint32_t> > inverted = invertSegments(segments);

    this->data_auto_segments_by_intensity_inverted.setValue(inverted);

    return inverted;
}

std::vector<uint32_t> Storage::getAutoSegmentsByIntensityMask()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_mask)

    DEBUG("Calculate mask for segments using intensity")

    auto segments = this->getAutoSegmentsByIntensity();

    if (segments.empty()) return std::vector<uint32_t>();

    auto intensity = this->getIntensityNormalized();

    auto mask = segmentsToMask(segments, intensity.size());

    this->data_auto_segments_by_intensity_mask.setValue(mask);

    return mask;
}

std::vector<std::pair<uint32_t, uint32_t> > Storage::getAutoSegmentsByIntensitySmoothed()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_smoothed)

    DEBUG("Calculate segments using intensity smoothed")

    auto intensity = this->getIntensityNormalized();

    if (intensity.empty()) return std::vector<std::pair<uint32_t, uint32_t> >();

    auto intensity_smoothed = this->getIntensityNormalizedSmoothed();

    std::vector<std::pair<uint32_t, uint32_t> > segments = intensitySmoothedToSegments(
        intensity,
        intensity_smoothed,
        this->config->segmentsByIntensityMinimumLengthPoints(WAVE_FRAME_RATE)
        );

    this->data_auto_segments_by_intensity_smoothed.setValue(segments);

    return segments;
}

std::vector<std::pair<uint32_t, uint32_t> > Storage::getAutoSegmentsByIntensitySmoothedInverted()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_smoothed_inverted)

    DEBUG("Calculate segments using intensity smoothed inverted")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return std::vector<std::pair<uint32_t, uint32_t> >();

    std::vector<std::pair<uint32_t, uint32_t> > inverted = invertSegments(segments);

    this->data_auto_segments_by_intensity_smoothed_inverted.setValue(inverted);

    return inverted;
}

std::vector<uint32_t> Storage::getAutoSegmentsByIntensitySmoothedMask()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_smoothed_mask)

    DEBUG("Calculate mask for segments using intensity smoothed")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return std::vector<uint32_t>();

    auto intensity = this->getIntensityNormalized();

    auto mask = segmentsToMask(segments, intensity.size());

    this->data_auto_segments_by_intensity_smoothed_mask.setValue(mask);

    return mask;
}

std::vector<std::pair<uint32_t, uint32_t> > Storage::getAutoSegmentsByIntensityDoubleSmoothed()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_double_smoothed)

    DEBUG("Calculate segments using intensity double smoothed")

    auto intensity = this->getIntensityNormalizedSmoothed();

    if (intensity.empty()) return std::vector<std::pair<uint32_t, uint32_t> >();

    auto intensity_smoothed = this->getIntensityNormalizedDoubleSmoothed();

    std::vector<std::pair<uint32_t, uint32_t> > segments = intensitySmoothedToSegments(
        intensity,
        intensity_smoothed,
        this->config->segmentsByIntensityMinimumLengthPoints(WAVE_FRAME_RATE)
        );

    this->data_auto_segments_by_intensity_double_smoothed.setValue(segments);

    return segments;
}

std::vector<std::pair<uint32_t, uint32_t> > Storage::getAutoSegmentsByIntensityDoubleSmoothedInverted()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_double_smoothed_inverted)

    DEBUG("Calculate segments using intensity double smoothed inverted")

    auto segments = this->getAutoSegmentsByIntensityDoubleSmoothed();

    if (segments.empty()) return std::vector<std::pair<uint32_t, uint32_t> >();

    std::vector<std::pair<uint32_t, uint32_t> > inverted = invertSegments(segments);

    this->data_auto_segments_by_intensity_smoothed_inverted.setValue(inverted);

    return inverted;
}

std::vector<uint32_t> Storage::getAutoSegmentsByIntensityDoubleSmoothedMask()
{
    RETURN_VALUE_IF_EXIST(this->data_auto_segments_by_intensity_smoothed_mask)

    DEBUG("Calculate mask for segments using intensity double smoothed")

    auto segments = this->getAutoSegmentsByIntensityDoubleSmoothed();

    if (segments.empty()) return std::vector<uint32_t>();

    auto intensity = this->getIntensityNormalized();

    auto mask = segmentsToMask(segments, intensity.size());

    this->data_auto_segments_by_intensity_double_smoothed_mask.setValue(mask);

    return mask;
}
