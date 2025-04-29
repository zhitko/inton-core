#include "storage.h"

#include "define.h"
#include "config.h"

#include "modules/calculation/intensity.h"

using namespace IntonCore;

std::vector<double> Storage::getIntensity()
{
    RETURN_VALUE_IF_EXIST(this->data_intensity)

    DEBUG("Get intensity")
    DEBUG("Get intensity %i", this->config->intensityFrame())
    DEBUG("Get intensity %i", this->config->intensityShift())

    auto wave = this->getWave();

    if (wave.empty()) return std::vector<double>();

    auto intensity = waveToIntensive(
        wave,
        this->config->intensityFrame(),
        this->config->intensityShift()
        );

    this->data_intensity.setValue(intensity);

    return intensity;

}

std::vector<double> Storage::getIntensityNormalized()
{
    RETURN_VALUE_IF_EXIST(this->data_intensity_normalized)

    DEBUG("Get intensity normalized")

    auto data = this->getIntensity();

    if (data.empty()) return std::vector<double>();

    auto normalized = normalizeVector(
        data,
        DATA_NORMALIZED_MIN,
        DATA_NORMALIZED_MAX
    );

    this->data_intensity_normalized.setValue(normalized);

    return normalized;
}

std::vector<double> Storage::getIntensityNormalizedSmoothed()
{
    RETURN_VALUE_IF_EXIST(this->data_intensity_normalized_smoothed)

    DEBUG("Get intensity normalized smoothed")

    auto data = this->getIntensityNormalized();

    if (data.empty()) return std::vector<double>();

    auto smoothed = linerSmoothVector(
        data,
        config->intensitySmoothFrame()
    );

    this->data_intensity_normalized_smoothed.setValue(smoothed);

    return smoothed;
}

std::vector<double> Storage::getIntensityNormalizedDoubleSmoothed()
{
    RETURN_VALUE_IF_EXIST(this->data_intensity_normalized_double_smoothed)

    DEBUG("Get intensity normalized double smoothed")

    auto data = this->getIntensityNormalizedSmoothed();

    if (data.empty()) return std::vector<double>();

    auto smoothed = linerSmoothVector(
        data,
        config->intensityDoubleSmoothFrame()
    );

    this->data_intensity_normalized_double_smoothed.setValue(smoothed);

    return smoothed;
}

void Storage::cleanIntensity()
{
    if (this->data_intensity.isExists())
        this->data_intensity.clear();
    if (this->data_intensity_normalized.isExists())
        this->data_intensity_normalized.clear();
    if (this->data_intensity_normalized_smoothed.isExists())
        this->data_intensity_normalized_smoothed.clear();
    if (this->data_intensity_normalized_double_smoothed.isExists())
        this->data_intensity_normalized_double_smoothed.clear();
}
