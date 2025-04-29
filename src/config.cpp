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
    this->wave_correction_low_frequency = 0.0;
    this->wave_correction_high_frequency = 0.0;
    this->wave_correction_strength = 1.0;
    this->wave_dynamic_zero_crossing_frame_sec = 1.0;
    this->wave_dynamic_zero_crossing_shift_sec = 0.1;
    this->wave_dynamic_zero_crossing_target = 0.0;
    this->wave_dynamic_strength_target = 0.0;
    this->wave_correction_type = 0;
    this->intensity_frame = CONFIG_DEFUALT_INTENSIVE_FRAME;
    this->intensity_shift = CONFIG_DEFUALT_INTENSIVE_SHIFT;
    this->intensity_smooth_frame = CONFIG_DEFUALT_INTENSIVE_SMOOTH_FRAME;
    this->intensity_double_smooth_frame = CONFIG_DEFUALT_INTENSIVE_DOUBLE_SMOOTH_FRAME;

    this->segments_by_intensity_threshold_absolute = SEGMANTS_BY_INTENSITY_THRESHOLD_ABSOLUTE;
    this->segments_by_intensity_threshold_relative = SEGMANTS_BY_INTENSITY_THRESHOLD_RELATIVE;
    this->segments_by_intensity_minimum_length = SEGMANTS_BY_INTENSITY_MIN_LENGTH;
    this->segments_by_intensity_double_smooth_minimum_length = SEGMANTS_BY_INTENSITY_DOUBLE_SMOOTHED_MIN_LENGTH;

    this->spectrum_frame_length = SPECTRUM_FRAME_LENGTH;
    this->spectrum_fft_length = SPECTRUM_FFT_LENGTH;
    this->spectrum_output_format = SPECTRUM_OUTPUT_FORMAT;
    this->spectrum_epsilon = SPECTRUM_EPSILON;
    this->spectrum_relative_floor_in_decibels = SPECTRUM_RELATIVE_FLOOR_IN_DECIBELS;
}

std::list<double> Config::getOctaves() const
{
    return octaves;
}

bool Config::setOctaves(std::list<double> octaves)
{
    auto is_changed = this->octaves == octaves;
    this->octaves = octaves;
    return is_changed;
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

uint32_t Config::getSpectrumFrameLength() const
{
    return spectrum_frame_length;
}

bool Config::setSpectrumFrameLength(uint32_t newFrame_length)
{
    auto spectrum_frame_length_old = spectrum_frame_length;
    spectrum_frame_length = newFrame_length;
    return spectrum_frame_length_old != newFrame_length;
}

uint32_t Config::getSpectrumFftLength() const
{
    return spectrum_fft_length;
}

bool Config::setSpectrumFftLength(uint32_t newFft_length)
{
    auto old = spectrum_fft_length;
    spectrum_fft_length = newFft_length;
    return old != newFft_length;
}

uint32_t Config::getSpectrumOutputFormat() const
{
    return spectrum_output_format;
}

bool Config::setSpectrumOutputFormat(uint32_t newOutput_format)
{
    auto old = spectrum_output_format;
    spectrum_output_format = newOutput_format;
    return old != newOutput_format;
}

double Config::getSpectrumEpsilon() const
{
    return spectrum_epsilon;
}

bool Config::setSpectrumEpsilon(double newEpsilon)
{
    auto old = spectrum_epsilon;
    spectrum_epsilon = newEpsilon;
    return old != newEpsilon;
}

double Config::getSpectrumRelativeFloorInDecibels() const
{
    return spectrum_relative_floor_in_decibels;
}

bool Config::setSpectrumRelativeFloorInDecibels(double newRelative_floor_in_decibels)
{
    auto old = spectrum_relative_floor_in_decibels;
    spectrum_relative_floor_in_decibels = newRelative_floor_in_decibels;
    return old != newRelative_floor_in_decibels;
}


bool Config::setIsManual(bool isManual)
{
    return this->manual = isManual;
}

bool Config::isManual()
{
    return this->manual;
}

double Config::waveCorrectionStrength()
{
    return this->wave_correction_strength;
}

bool Config::setWaveCorrectionStrength(double value)
{
    auto old = this->wave_correction_strength;
    this->wave_correction_strength = value;
    return old != value;
}

double Config::waveCorrectionHighFrequency()
{
    return this->wave_correction_high_frequency;
}

bool Config::setWaveCorrectionHighFrequency(double value)
{
    auto old = this->wave_correction_high_frequency;
    this->wave_correction_high_frequency = value;
    return old != value;
}

int Config::waveCorrectionType()
{
    return this->wave_correction_type;
}

bool Config::setWaveCorrectionType(int value)
{
    auto old = this->wave_correction_type;
    this->wave_correction_type = value;
    return old != value;
}

double Config::waveDynamicZeroCrossingFrameSec()
{
    return this->wave_dynamic_zero_crossing_frame_sec;
}

bool Config::setWaveDynamicZeroCrossingFrameSec(double value)
{
    auto old = this->wave_dynamic_zero_crossing_frame_sec;
    this->wave_dynamic_zero_crossing_frame_sec = value;
    return old != value;
}

double Config::waveDynamicZeroCrossingShiftSec()
{
    return this->wave_dynamic_zero_crossing_shift_sec;
}

bool Config::setWaveDynamicZeroCrossingShiftSec(double value)
{
    auto old = this->wave_dynamic_zero_crossing_shift_sec;
    this->wave_dynamic_zero_crossing_shift_sec = value;
    return old != value;
}

double Config::waveDynamicStrengthTarget()
{
    return this->wave_dynamic_strength_target;
}

bool Config::setWaveDynamicStrengthTarget(double value)
{
    auto old = this->wave_dynamic_strength_target;
    this->wave_dynamic_strength_target = value;
    return old != value;
}

double Config::waveDynamicZeroCrossingTarget()
{
    return this->wave_dynamic_zero_crossing_target;
}

bool Config::setWaveDynamicZeroCrossingTarget(double value)
{
    auto old = this->wave_dynamic_zero_crossing_target;
    this->wave_dynamic_zero_crossing_target = value;
    return old != value;
}

double Config::waveCorrectionLowFrequency()
{
    return this->wave_correction_low_frequency;
}

bool Config::setWaveCorrectionLowFrequency(double value)
{
    auto old = this->wave_correction_low_frequency;
    this->wave_correction_low_frequency = value;
    return old != value;
}

uint32_t Config::intensityFrame()
{
    return this->intensity_frame;
}

bool Config::setIntensityFrame(uint32_t value)
{
    auto old = this->intensity_frame;
    this->intensity_frame = value;
    return old != value;
}

uint32_t Config::intensityShift()
{
    return this->intensity_shift;
}

bool Config::setIntensityShift(uint32_t value)
{
    auto old = this->intensity_shift;
    this->intensity_shift = value;
    return old != value;
}

uint32_t Config::intensitySmoothFrame()
{
    return this->intensity_smooth_frame;
}

bool Config::setIntensitySmoothFrame(uint32_t value)
{
    auto old = this->intensity_smooth_frame;
    this->intensity_smooth_frame = value;
    return old != value;
}

uint32_t Config::intensityDoubleSmoothFrame()
{
    return this->intensity_double_smooth_frame;
}

bool Config::setIntensityDoubleSmoothFrame(uint32_t value)
{
    auto old = this->intensity_double_smooth_frame;
    this->intensity_double_smooth_frame = value;
    return old != value;
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
    DEBUG("segmentsByIntensityMinimumLengthPoints 1 %i", frame_rate)
    DEBUG("segmentsByIntensityMinimumLengthPoints 1 %i", this->intensity_shift)
    DEBUG("segmentsByIntensityMinimumLengthPoints 1 %i", this->segments_by_intensity_minimum_length)
    return (1.0 * frame_rate / this->intensity_shift) / 1000 * this->segments_by_intensity_minimum_length;
}

bool Config::setSegmentsByIntensityMinimumLength(uint32_t value)
{
    auto old = segments_by_intensity_minimum_length;
    segments_by_intensity_minimum_length = value;
    return old != value;
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
