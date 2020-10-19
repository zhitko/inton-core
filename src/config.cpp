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

int Config::segmentsByIntensityMinimumLength()
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
