#include "config.h"

#include "define.h"
#include "error.h"

using namespace IntonCore;

Config::Config()
{
    this->loadDefaults();
}

void Config::loadDefaults()
{
    this->manual = false;
    this->intensity_frame = CONFIG_DEFUALT_INTENSIVE_FRAME;
    this->intensity_shift = CONFIG_DEFUALT_INTENSIVE_SHIFT;
    this->intensity_smooth_frame = CONFIG_DEFUALT_INTENSIVE_SMOOTH_FRAME;
    this->intensity_double_smooth_frame = CONFIG_DEFUALT_INTENSIVE_DOUBLE_SMOOTH_FRAME;

    this->segments_by_intensity_threshold_absolute = SEGMANTS_BY_INTENSITY_THRESHOLD_ABSOLUTE;
    this->segments_by_intensity_threshold_relative = SEGMANTS_BY_INTENSITY_THRESHOLD_RELATIVE;
    this->segments_by_intensity_minimum_length = SEGMANTS_BY_INTENSITY_MIN_LENGTH;
    this->segments_by_intensity_double_smooth_minimum_length = SEGMANTS_BY_INTENSITY_DOUBLE_SMOOTHED_MIN_LENGTH;
}

double Config::getPitchMaxFrequency() const
{
    return pitch_max_freq;
}

bool Config::setPitchMaxFrequency(double newPitch_max_freq)
{
    auto pitch_max_freq_old = pitch_max_freq;
    pitch_max_freq = newPitch_max_freq;
    return pitch_max_freq_old != newPitch_max_freq;
}

double Config::getPitchMinFrequency() const
{
    return pitch_min_freq;
}

bool Config::setPitchMinFrequency(double newPitch_min_freq)
{
    auto pitch_min_freq_old = pitch_min_freq;
    pitch_min_freq = newPitch_min_freq;
    return pitch_min_freq_old != newPitch_min_freq;
}

double Config::getPitchSamplingFrequency() const
{
    return pitch_sample_freq;
}

bool Config::setPitchSamplingFrequency(double newPitch_sample_freq)
{
    auto pitch_sample_freq_old = pitch_sample_freq;
    pitch_sample_freq = newPitch_sample_freq;
    return pitch_sample_freq_old != newPitch_sample_freq;
}

double Config::getPitchThresholdForSwipe() const
{
    return pitch_thresh_swipe;
}

bool Config::setPitchThresholdForSwipe(double newPitch_thresh_swipe)
{
    auto pitch_thresh_swipe_old = pitch_thresh_swipe;
    pitch_thresh_swipe = newPitch_thresh_swipe;
    return pitch_thresh_swipe_old != newPitch_thresh_swipe;
}

double Config::getPitchThresholdForRapt() const
{
    return pitch_thresh_rapt;
}

bool Config::setPitchThresholdForRapt(double newPitch_thresh_rapt)
{
    auto pitch_thresh_rapt_old = pitch_thresh_rapt;
    pitch_thresh_rapt = newPitch_thresh_rapt;
    return pitch_thresh_rapt_old != newPitch_thresh_rapt;
}

uint32_t Config::getPitchFrameShift() const
{
    return pitch_frame_shift;
}

bool Config::setPitchFrameShift(uint32_t newPitch_frame_shift)
{
    auto pitch_frame_shift_old = pitch_frame_shift;
    pitch_frame_shift = newPitch_frame_shift;
    return pitch_frame_shift_old != newPitch_frame_shift;
}

uint32_t Config::getPitchOutputType() const
{
    return pitch_otype;
}

bool Config::setPitchOutputType(uint32_t newPitch_otype)
{
    auto pitch_otype_old = pitch_otype;
    pitch_otype = newPitch_otype;
    return pitch_otype_old != newPitch_otype;
}

uint32_t Config::getPitchAlgorithmType() const
{
    return pitch_atype;
}

bool Config::setPitchAlgorithmType(uint32_t newPitch_atype)
{
    auto pitch_atype_old = pitch_atype;
    pitch_atype = newPitch_atype;
    return pitch_atype_old != newPitch_atype;
}

bool Config::setIsManual(bool isManual)
{
    return this->manual = isManual;
}

bool Config::isManual()
{
    return this->manual;
}

uint32_t Config::intensityFrame()
{
    return this->intensity_frame;
}

void Config::setIntensityFrame(uint32_t value)
{
    this->intensity_frame = value;
}

uint32_t Config::intensityShift()
{
    return this->intensity_shift;
}

void Config::setIntensityShift(uint32_t value)
{
    this->intensity_shift = value;
}

uint32_t Config::intensitySmoothFrame()
{
    return this->intensity_smooth_frame;
}

void Config::setIntensitySmoothFrame(uint32_t value)
{
    this->intensity_smooth_frame = value;
}

uint32_t Config::intensityDoubleSmoothFrame()
{
    return this->intensity_double_smooth_frame;
}

void Config::setIntensityDoubleSmoothFrame(uint32_t value)
{
    this->intensity_double_smooth_frame = value;
}

double Config::segmentsByIntensityThresholdAbsolute()
{
    return this->segments_by_intensity_threshold_absolute;
}

void Config::setSegmentsByIntensityThresholdAbsolute(double value)
{
    this->segments_by_intensity_threshold_absolute = value;
}

double Config::segmentsByIntensityThresholdRelative()
{
    return this->segments_by_intensity_threshold_relative;
}

void Config::setSegmentsByIntensityThresholdRelative(double value)
{
    this->segments_by_intensity_threshold_relative = value;
}

uint32_t Config::segmentsByIntensityMinimumLength()
{
    return this->segments_by_intensity_minimum_length;
}

uint32_t Config::segmentsByIntensityMinimumLengthPoints(int frame_rate)
{
    return (1.0 * frame_rate / this->intensity_shift) / 1000 * this->segments_by_intensity_minimum_length;
}

void Config::setSegmentsByIntensityMinimumLength(uint32_t value)
{
    this->segments_by_intensity_minimum_length = value;
}

uint32_t Config::segmentsByIntensityDoubleSmoothMinimumLength()
{
    return this->segments_by_intensity_double_smooth_minimum_length;
}

uint32_t Config::segmentsByIntensityDoubleSmoothMinimumLengthPoints(int frame_rate)
{
    return (1.0 * frame_rate / this->intensity_smooth_frame) / 1000 * this->segments_by_intensity_double_smooth_minimum_length;
}

void Config::setSegmentsByIntensityDoubleSmoothMinimumLength(uint32_t value)
{
    this->segments_by_intensity_double_smooth_minimum_length = value;
}
