#ifndef STORAGE_H
#define STORAGE_H

#include "IntonCore_global.h"

#include "storage_helpers.h"

#include "storagecache.h"

#include <vector>
#include <string>

struct WaveFile;

namespace IntonCore {

class Config;

class Storage
{
public:
    Storage(const std::string& file_path, Config * config);
    Storage(WaveFile *file, Config * config);
    virtual ~Storage();

    void clear();

protected:
    WaveFile* getWaveFile();

public:
    /**
     * @brief getWave
     * @return vector of wave signal values
     */
    std::vector<double> getWave();


    uint32_t getWaveFrameRate();

    /**
     * @brief getWaveNormalized
     * @return normalized vector of wave signal values in range -1 to 1
     */
    std::vector<double> getWaveNormalized();

    /**
     * @brief getIntensity
     * @return return vector of intensity values using frame and shift from config
     */
    std::vector<double> getIntensity();
    std::vector<double> getIntensityNormalized();
    std::vector<double> getIntensityNormalizedSmoothed();
    std::vector<double> getIntensityNormalizedDoubleSmoothed();
    void cleanIntensity();

    /**
     * @brief getPitch
     * @return return pitch
     */
    std::vector<double> getPitch();
    std::vector<double> getPitchNormalized();
    std::vector<double> getPitchOctavesNormalized();
    std::vector<double> getPitchCutted();
    std::vector<double> getPitchNormalizedCutted();
    std::vector<double> getPitchOctavesNormalizedCutted();
    std::vector<double> getPitchDerivative();
    std::vector<double> getPitchNormalizedDerivative();
    std::vector<double> getPitchOctavesNormalizedDerivative();
    std::vector<double> getPitchCuttedDerivative();
    std::vector<double> getPitchNormalizedCuttedDerivative();
    std::vector<double> getPitchOctavesNormalizedCuttedDerivative();
    void cleanPitch();

    /**
     * @brief getManualSegmentsP
     * @return vector of pairs with section start position and section length
     */
    std::vector<std::pair<uint32_t, uint32_t>> getManualSegmentsP();
    std::vector<std::pair<uint32_t, uint32_t>> getManualSegmentsN();
    std::vector<std::pair<uint32_t, uint32_t>> getManualSegmentsT();

    std::vector<std::pair<uint32_t, uint32_t>> getAutoSegmentsByIntensity();
    std::vector<std::pair<uint32_t, uint32_t>> getAutoSegmentsByIntensityInverted();
    std::vector<uint32_t> getAutoSegmentsByIntensityMask();

    std::vector<std::pair<uint32_t, uint32_t>> getAutoSegmentsByIntensitySmoothed();
    std::vector<std::pair<uint32_t, uint32_t>> getAutoSegmentsByIntensitySmoothedInverted();
    std::vector<uint32_t> getAutoSegmentsByIntensitySmoothedMask();

    std::vector<std::pair<uint32_t, uint32_t>> getAutoSegmentsByIntensityDoubleSmoothed();
    std::vector<std::pair<uint32_t, uint32_t>> getAutoSegmentsByIntensityDoubleSmoothedInverted();
    std::vector<uint32_t> getAutoSegmentsByIntensityDoubleSmoothedMask();

    void cleanSegments();

    /**
     * Metrics API
     */

    long getConsonantsAndSilenceCount();
    double getConsonantsAndSilenceLength();
    double getConsonantsAndSilenceLengthMean();
    double getConsonantsAndSilenceLengthGeneralizedMean(double d);
    double getConsonantsAndSilenceLengthSquareMean();
    double getConsonantsAndSilenceLengthVariance();
    double getConsonantsAndSilenceLengthSkewness();
    double getConsonantsAndSilenceLengthKurtosis();

    long getSilenceCount();
    double getSilenceLength();
    double getSilenceLengthMean();
    double getSilenceLengthVariance();
    double getSilenceLengthSkewness();
    double getSilenceLengthKurtosis();

    long getVowelsCount();
    double getVowelsLength();
    double getVowelsLengthMean();
    double getVowelsLengthGeneralizedMean(double d);
    double getVowelsLengthSquareMean();
    double getVowelsLengthVariance();
    double getVowelsLengthSkewness();
    double getVowelsLengthKurtosis();

    /**
     * Utils
     */

    uint32_t convertIntensityPointsToWavePoints(uint32_t value);
    double convertIntensityPointsToSec(uint32_t value);
    double convertWavePointsToSec(uint32_t value);

private:
    Config * config;
    std::string file_path;

    StorageCache<WaveFile> wave_file;

    double seconds;

    StorageCache<std::vector<double>> data_wave;
    StorageCache<std::vector<double>> data_wave_normalized;

    StorageCache<std::vector<double>> data_intensity;
    StorageCache<std::vector<double>> data_intensity_normalized;
    StorageCache<std::vector<double>> data_intensity_normalized_smoothed;
    StorageCache<std::vector<double>> data_intensity_normalized_double_smoothed;

    StorageCache<std::vector<double>> data_pitch;
    StorageCache<std::vector<double>> data_pitch_normalized;
    StorageCache<std::vector<double>> data_pitch_octaves_normalized;
    StorageCache<std::vector<double>> data_pitch_cutted;
    StorageCache<std::vector<double>> data_pitch_normalized_cutted;
    StorageCache<std::vector<double>> data_pitch_octaves_normalized_cutted;
    StorageCache<std::vector<double>> data_pitch_derivative;
    StorageCache<std::vector<double>> data_pitch_normalized_derivative;
    StorageCache<std::vector<double>> data_pitch_octaves_normalized_derivative;
    StorageCache<std::vector<double>> data_pitch_cutted_derivative;
    StorageCache<std::vector<double>> data_pitch_normalized_cutted_derivative;
    StorageCache<std::vector<double>> data_pitch_octaves_normalized_cutted_derivative;

    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_manual_segments_p;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_manual_segments_t;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_manual_segments_n;

    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity;
    StorageCache<std::vector<uint32_t>> data_auto_segments_by_intensity_mask;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity_inverted;

    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity_smoothed;
    StorageCache<std::vector<uint32_t>> data_auto_segments_by_intensity_smoothed_mask;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity_smoothed_inverted;

    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity_double_smoothed;
    StorageCache<std::vector<uint32_t>> data_auto_segments_by_intensity_double_smoothed_mask;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity_double_smoothed_inverted;

    StorageCache<DistributionMoments> data_consonants_and_silence_length_distribution_moments;
    StorageCache<double> data_consonants_and_silence_length_generalized_mean;
    StorageCache<double> data_consonants_and_silence_length_square_mean;
    StorageCache<DistributionMoments> data_silence_length_distribution_moments;
    StorageCache<DistributionMoments> data_vowels_length_distribution_moments;
    StorageCache<double> data_vowels_length_generalized_mean;
    StorageCache<double> data_vowels_length_square_mean;
};

}

#endif // STORAGE_H
