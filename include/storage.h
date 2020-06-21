#ifndef STORAGE_H
#define STORAGE_H

#include "IntonCore_global.h"

#include "storagecache.h"

#include <vector>
#include <string>

#define RETURN_IF_EXIST(storage) \
    if ((storage).isExists()) return ((storage).getValue());
#define RETURN_VALUE_IF_EXIST(storage) \
    if ((storage).isExists()) return *((storage).getValue());

struct WaveFile;

namespace IntonCore {

class Config;

class Storage
{
public:
    Storage(const std::string& file_path, Config * config);
    virtual ~Storage();

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

    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_manual_segments_p;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_manual_segments_t;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_manual_segments_n;

    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity;
    StorageCache<std::vector<uint32_t>> data_auto_segments_by_intensity_mask;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity_inverted;

    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity_smoothed;
    StorageCache<std::vector<uint32_t>> data_auto_segments_by_intensity_smoothed_mask;
    StorageCache<std::vector<std::pair<uint32_t, uint32_t>>> data_auto_segments_by_intensity_smoothed_inverted;
};

}

#endif // STORAGE_H
