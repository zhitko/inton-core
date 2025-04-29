#ifndef CONFIG_H
#define CONFIG_H

#include "IntonCore_global.h"

#include <string>
#include <list>

namespace IntonCore {

class Config
{
public:
    Config();

public:
    bool setIsManual(bool isManual);
    bool isManual();

    double waveCorrectionStrength();
    bool setWaveCorrectionStrength(double value);
    double waveCorrectionHighFrequency();
    bool setWaveCorrectionHighFrequency(double value);
    double waveCorrectionLowFrequency();
    bool setWaveCorrectionLowFrequency(double value);
    double waveDynamicZeroCrossingFrameSec();
    bool setWaveDynamicZeroCrossingFrameSec(double value);
    double waveDynamicZeroCrossingShiftSec();
    bool setWaveDynamicZeroCrossingShiftSec(double value);
    double waveDynamicZeroCrossingTarget();
    bool setWaveDynamicZeroCrossingTarget(double value);
    double waveDynamicStrengthTarget();
    bool setWaveDynamicStrengthTarget(double value);
    int waveCorrectionType();
    bool setWaveCorrectionType(int value);

    /**
     * @brief intensityFrame
     * @return frame length used to calculate intensity
     */
    uint32_t intensityFrame();
    bool setIntensityFrame(uint32_t value);

    /**
     * @brief intensityShift
     * @return frame shift used to calculate intensity
     */
    uint32_t intensityShift();
    bool setIntensityShift(uint32_t value);

    /**
     * @brief intensitySmoothFrame
     * @return frame used to liner smooth for intensity
     */
    uint32_t intensitySmoothFrame();
    bool setIntensitySmoothFrame(uint32_t value);

    /**
     * @brief intensityDoubleSmoothFrame
     * @return frame used to liner second smooth for intensity
     */
    uint32_t intensityDoubleSmoothFrame();
    bool setIntensityDoubleSmoothFrame(uint32_t value);

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
    bool setSegmentsByIntensityMinimumLength(uint32_t value);

    /**
     * @brief segmentsByIntensityDoubleSmoothMinimumLength
     * @return minimum length in millisec for segments calculated using double smoothed intensity
     */
    uint32_t segmentsByIntensityDoubleSmoothMinimumLength();
    uint32_t segmentsByIntensityDoubleSmoothMinimumLengthPoints(int frame_rate);
    void setSegmentsByIntensityDoubleSmoothMinimumLength(uint32_t value);

    uint32_t getPitchAlgorithmType() const;
    bool setPitchAlgorithmType(uint32_t newPitch_atype);

    uint32_t getPitchOutputType() const;
    bool setPitchOutputType(uint32_t newPitch_otype);

    uint32_t getPitchFrameShift() const;
    bool setPitchFrameShift(uint32_t newPitch_frame_shift);

    double getPitchThresholdForRapt() const;
    bool setPitchThresholdForRapt(double newPitch_thresh_rapt);

    double getPitchThresholdForSwipe() const;
    bool setPitchThresholdForSwipe(double newPitch_thresh_swipe);

    double getPitchSamplingFrequency() const;
    bool setPitchSamplingFrequency(double newPitch_sample_freq);

    double getPitchMinFrequency() const;
    bool setPitchMinFrequency(double newPitch_min_freq);

    double getPitchMaxFrequency() const;
    bool setPitchMaxFrequency(double newPitch_max_freq);

    std::list<double> getOctaves() const;
    bool setOctaves(std::list<double> octaves);

    uint32_t getSpectrumFrameLength() const;
    bool setSpectrumFrameLength(uint32_t newFrame_length);

    uint32_t getSpectrumFftLength() const;
    bool setSpectrumFftLength(uint32_t newFft_length);

    uint32_t getSpectrumOutputFormat() const;
    bool setSpectrumOutputFormat(uint32_t newOutput_format);

    double getSpectrumEpsilon() const;
    bool setSpectrumEpsilon(double newEpsilon);

    double getSpectrumRelativeFloorInDecibels() const;
    bool setSpectrumRelativeFloorInDecibels(double newRelative_floor_in_decibels);

private:
    void loadDefaults();

    /*
     * Wave settings
     */
    int wave_correction_type;
    double wave_correction_high_frequency;
    double wave_correction_low_frequency;
    double wave_correction_strength;
    double wave_dynamic_zero_crossing_frame_sec;
    double wave_dynamic_zero_crossing_shift_sec;
    double wave_dynamic_zero_crossing_target;
    double wave_dynamic_strength_target;

    /*замок для шкафчика
     * Intensity settings
     */
    uint32_t intensity_frame;
    uint32_t intensity_shift;
    uint32_t intensity_smooth_frame;
    uint32_t intensity_double_smooth_frame;

    /*
     * Segments settings
     */
    double segments_by_intensity_threshold_relative;
    double segments_by_intensity_threshold_absolute;
    uint32_t segments_by_intensity_minimum_length;
    uint32_t segments_by_intensity_double_smooth_minimum_length;

    /*
     * Pitch settings
     */
    uint32_t pitch_atype;
    uint32_t pitch_otype;
    uint32_t pitch_frame_shift;
    double pitch_thresh_rapt;
    double pitch_thresh_swipe;
    double pitch_sample_freq;
    double pitch_min_freq;
    double pitch_max_freq;

    /*
     * Spectrum settings
     */
    uint32_t spectrum_frame_length;
    uint32_t spectrum_fft_length;
    uint32_t spectrum_output_format;
    double spectrum_epsilon;
    double spectrum_relative_floor_in_decibels;

    std::list<double> octaves;

protected:
    bool manual;
};

}

#endif // CONFIG_H
