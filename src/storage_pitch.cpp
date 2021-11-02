#include "storage.h"

#include "define.h"
#include "config.h"

#include "modules/calculation/pitch.h"

using namespace IntonCore;

std::vector<double> Storage::getPitch()
{
    RETURN_VALUE_IF_EXIST(this->data_pitch)

    DEBUG("Get pitch")

    auto wave = this->getWave();

    if (wave.empty()) return std::vector<double>();

    auto pith = waveToPitch(
        wave,
        config->getPitchAlgorithmType(),
        config->getPitchOutputType(),
        config->getPitchFrameShift(),
        config->getPitchThresholdForRapt(),
        config->getPitchThresholdForSwipe(),
        config->getPitchSamplingFrequency(),
        config->getPitchMinFrequency(),
        config->getPitchMaxFrequency()
    );

    auto pitchNorm = normalizeVector(pith);

    this->data_pitch.setValue(pitchNorm);

    return pitchNorm;
}

void Storage::cleanPitch()
{
    this->data_pitch.clear();
}
