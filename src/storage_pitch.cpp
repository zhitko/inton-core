#include "storage.h"

#include "define.h"
#include "config.h"

#include "modules/calculation/pitch.h"

using namespace IntonCore;

std::vector<double> Storage::getPitch()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch)

    DEBUG("Get pitch")

    auto data = this->getWave();

    if (data.empty()) return std::vector<double>();

    auto result = waveToPitch(
        data,
        config->getPitchAlgorithmType(),
        config->getPitchOutputType(),
        config->getPitchFrameShift(),
        config->getPitchThresholdForRapt(),
        config->getPitchThresholdForSwipe(),
        config->getPitchSamplingFrequency(),
        config->getPitchMinFrequency(),
        config->getPitchMaxFrequency()
    );

    this->data_pitch.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchNormalized()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_normalized)

    DEBUG("Get pitch normalized")

    auto data = this->getPitch();

    if (data.empty()) return std::vector<double>();

    auto result = normalizeVector(data);

    this->data_pitch_normalized.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchOctavesNormalized()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_octaves_normalized)

    DEBUG("Get pitch octaves normalized")

    auto data = this->getPitch();

    if (data.empty()) return std::vector<double>();

    auto result = normalizeVectorByOctaves(data, this->config->getOctaves());

    this->data_pitch_octaves_normalized.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchCutted()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_cutted)

    DEBUG("Get pitch cutted")

    auto data = this->getPitch();

    if (data.empty()) return std::vector<double>();

    auto result = cutVectorBySegments(
        data,
        this->getAutoSegmentsByIntensitySmoothed(),
        this->getIntensity().size()
    );

    this->data_pitch_cutted.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchNormalizedCutted()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_normalized_cutted)

    DEBUG("Get pitch normalized cutted")

    auto data = this->getPitchNormalized();

    if (data.empty()) return std::vector<double>();

    auto result = cutVectorBySegments(
        data,
        this->getAutoSegmentsByIntensitySmoothed(),
        this->getIntensity().size()
    );

    this->data_pitch_normalized_cutted.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchOctavesNormalizedCutted()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_octaves_normalized_cutted)

    DEBUG("Get pitch octaves normalized cutted")

    auto data = this->getPitchOctavesNormalized();

    if (data.empty()) return std::vector<double>();

    auto result = cutVectorBySegments(
        data,
        this->getAutoSegmentsByIntensitySmoothed(),
        this->getIntensity().size()
    );

    this->data_pitch_octaves_normalized_cutted.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchDerivative()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_derivative)

    DEBUG("Get pitch derivative")

    auto data = this->getPitch();

    if (data.empty()) return std::vector<double>();

    auto result = calculateVectorDerivative(
        data
    );

    this->data_pitch_derivative.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchNormalizedDerivative()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_normalized_derivative)

    DEBUG("Get pitch normalized derivative")

    auto data = this->getPitchNormalized();

    if (data.empty()) return std::vector<double>();

    auto result = calculateVectorDerivative(
        data
    );

    this->data_pitch_normalized_derivative.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchOctavesNormalizedDerivative()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_octaves_normalized_derivative)

    DEBUG("Get pitch octaves normalized derivative")

    auto data = this->getPitchOctavesNormalized();

    if (data.empty()) return std::vector<double>();

    auto result = calculateVectorDerivative(
        data
    );

    this->data_pitch_octaves_normalized_derivative.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchCuttedDerivative()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_cutted_derivative)

    DEBUG("Get pitch cutted derivative")

    auto data = this->getPitchCutted();

    if (data.empty()) return std::vector<double>();

    auto result = calculateVectorDerivative(
        data
    );

    this->data_pitch_cutted_derivative.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchNormalizedCuttedDerivative()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_normalized_cutted_derivative)

    DEBUG("Get pitch normalized cutted derivative")

    auto data = this->getPitchNormalizedCutted();

    if (data.empty()) return std::vector<double>();

    auto result = calculateVectorDerivative(
        data
    );

    this->data_pitch_normalized_cutted_derivative.setValue(result);

    return result;
}

std::vector<double> Storage::getPitchOctavesNormalizedCuttedDerivative()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch_octaves_normalized_cutted_derivative)

    DEBUG("Get pitch octaves normalized cutted derivative")

    auto data = this->getPitchOctavesNormalizedCutted();

    if (data.empty()) return std::vector<double>();

    auto result = calculateVectorDerivative(
        data
    );

    this->data_pitch_octaves_normalized_cutted_derivative.setValue(result);

    return result;
}


void Storage::cleanPitch()
{
    if (this->data_pitch.isExists())
        this->data_pitch.clear();
    if (this->data_pitch_normalized.isExists())
        this->data_pitch_normalized.clear();
    if (this->data_pitch_octaves_normalized.isExists())
        this->data_pitch_octaves_normalized.clear();
    if (this->data_pitch_cutted.isExists())
        this->data_pitch_cutted.clear();
    if (this->data_pitch_normalized_cutted.isExists())
        this->data_pitch_normalized_cutted.clear();
    if (this->data_pitch_octaves_normalized_cutted.isExists())
        this->data_pitch_octaves_normalized_cutted.clear();
    if (this->data_pitch_derivative.isExists())
        this->data_pitch_derivative.clear();
    if (this->data_pitch_normalized_derivative.isExists())
        this->data_pitch_normalized_derivative.clear();
    if (this->data_pitch_octaves_normalized_derivative.isExists())
        this->data_pitch_octaves_normalized_derivative.clear();
    if (this->data_pitch_cutted_derivative.isExists())
        this->data_pitch_cutted_derivative.clear();
    if (this->data_pitch_normalized_cutted_derivative.isExists())
        this->data_pitch_normalized_cutted_derivative.clear();
    if (this->data_pitch_octaves_normalized_cutted_derivative.isExists())
        this->data_pitch_octaves_normalized_cutted_derivative.clear();
}
