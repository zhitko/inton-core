#ifndef CONFIG_H
#define CONFIG_H

#include "IntonCore_global.h"

#include <string>

namespace IntonCore {

class Config
{
public:
    Config();

public:
    bool setIsManual(bool isManual);
    bool isManual();

    /**
     * @brief intensityFrame
     * @return frame length used to calculate intensity
     */
    uint32_t intensityFrame();
    void setIntensityFrame(uint32_t value);

    /**
     * @brief intensityShift
     * @return frame shift used to calculate intensity
     */
    uint32_t intensityShift();
    void setIntensityShift(uint32_t value);

    /**
     * @brief intensitySmoothFrame
     * @return frame used to liner smooth for intensity
     */
    uint32_t intensitySmoothFrame();
    void setIntensitySmoothFrame(uint32_t value);

    /**
     * @brief intensityDoubleSmoothFrame
     * @return frame used to liner second smooth for intensity
     */
    uint32_t intensityDoubleSmoothFrame();
    void setIntensityDoubleSmoothFrame(uint32_t value);

    /**
     * @brief segmentsByIntensityThresholdAbsolute
     * @return absolute theshold to calculate segments using intensity
     */
    double segmentsByIntensityThresholdAbsolute();
    void setSegmentsByIntensityThresholdAbsolute(double value);

    /**
     * @brief segmentsByIntensityThresholdRelative
     * @return relative threshold to increase accuracy for segments calculated using intensity
     */
    double segmentsByIntensityThresholdRelative();
    void setSegmentsByIntensityThresholdRelative(double value);

    /**
     * @brief segmentsByIntensityMinimumLength
     * @return minimum length in millisec for segments calculated using intensity
     */
    uint32_t segmentsByIntensityMinimumLength();
    uint32_t segmentsByIntensityMinimumLengthPoints(int frame_rate);
    void setSegmentsByIntensityMinimumLength(uint32_t value);

    /**
     * @brief segmentsByIntensityDoubleSmoothMinimumLength
     * @return minimum length in millisec for segments calculated using double smoothed intensity
     */
    uint32_t segmentsByIntensityDoubleSmoothMinimumLength();
    uint32_t segmentsByIntensityDoubleSmoothMinimumLengthPoints(int frame_rate);
    void setSegmentsByIntensityDoubleSmoothMinimumLength(uint32_t value);

private:
    void loadDefaults();

    uint32_t intensity_frame;
    uint32_t intensity_shift;
    uint32_t intensity_smooth_frame;
    uint32_t intensity_double_smooth_frame;

    double segments_by_intensity_threshold_relative;
    double segments_by_intensity_threshold_absolute;
    uint32_t segments_by_intensity_minimum_length;
    uint32_t segments_by_intensity_double_smooth_minimum_length;

protected:
    bool manual;
};

}

#endif // CONFIG_H
