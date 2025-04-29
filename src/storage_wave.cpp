#include "storage.h"

#include "define.h"
#include "config.h"

#include "modules/wav_file/wavFile.h"

using namespace IntonCore;

uint32_t Storage::getWaveFrameRate()
{
    // TODO: implement retrieve it from wave file
    return WAVE_FRAME_RATE;
}


std::vector<double> Storage::getWave()
{
    RETURN_VALUE_IF_EXIST(this->data_wave)

    DEBUG("Get wave")

    WaveFile * wave_file = this->getWaveFile();

    if (!wave_file) return std::vector<double>();

    uint32_t size = littleEndianBytesToUInt32(wave_file->dataChunk->chunkDataSize);
    uint16_t bits = littleEndianBytesToUInt16(wave_file->formatChunk->significantBitsPerSample);
    DEBUG("Get wave size %i", size)
    DEBUG("Get wave bits %i", bits)

    std::vector<double> wave_data = waveformDataToVector(wave_file->dataChunk->waveformData, size, bits);

    this->data_wave.setValue(wave_data);

    return wave_data;
}

std::vector<double> Storage::getWaveFrequenciesCorrectionValues()
{
    DEBUG("Storage::getWaveFrequenciesCorrectionValues: Get wave corrected")

    auto wave = this->getWave();
    double rounds = 1;

    if (wave.empty()) return std::vector<double>();

    auto frame_rate = this->getWaveFrameRate();
    auto wave_dynamic_zero_crossing_target = config->waveDynamicZeroCrossingTarget();
    double high_frequency_k, low_frequency_k;
    std::tie(high_frequency_k, low_frequency_k) = findZeroCrossingK(wave,  frame_rate, wave_dynamic_zero_crossing_target);
    if (high_frequency_k == -1 && low_frequency_k == -1)
    {
        rounds = 2;
        DEBUG("Storage::getWaveFrequenciesCorrectionValues: round 2")
        std::tie(high_frequency_k, low_frequency_k) = findZeroCrossingK(wave,  frame_rate, wave_dynamic_zero_crossing_target, true);
        DEBUG("Storage::getWaveFrequenciesCorrectionValues: high_frequency_k %f", high_frequency_k)
        DEBUG("Storage::getWaveFrequenciesCorrectionValues: low_frequency_k %f", low_frequency_k)
    }

    std::vector<double> results;
    results.push_back(high_frequency_k);
    results.push_back(low_frequency_k);
    results.push_back(rounds);

    return results;
}

double Storage::getWaveRMSCorrectionValues()
{
    DEBUG("getWaveRMSCorrectionValues")

    auto wave = this->getWave();

    if (wave.empty()) return 0;

    auto frame_rate = this->getWaveFrameRate();
    auto wave_dynamic_strength_target = config->waveDynamicStrengthTarget();
    DEBUG("getWaveRMSCorrectionValues: %f", wave_dynamic_strength_target)
    auto rms = this->getWaveRMS();
    DEBUG("getWaveRMSCorrectionValues: %f", rms)

    return wave_dynamic_strength_target / rms;
}

std::vector<double> Storage::getWaveCorrected()
{
    RETURN_VALUE_IF_EXIST(this->data_wave_corrected)

    DEBUG("Get wave corrected")

    auto wave = this->getWave();

    if (wave.empty()) return std::vector<double>();

    auto data_wave_corrected = wave;

    auto wave_correction_type = config->waveCorrectionType();
    DEBUG("Get wave corrected #%i", wave_correction_type)

    if (wave_correction_type == 1) {
        auto correction_high_frequency = config->waveCorrectionHighFrequency();
        auto correction_low_frequency = config->waveCorrectionLowFrequency();
        auto correction_strength = config->waveCorrectionStrength();
        DEBUG("Get wave corrected #1")
        DEBUG("Get wave corrected #1: %f", correction_high_frequency)
        DEBUG("Get wave corrected #1: %f", correction_low_frequency)
        DEBUG("Get wave corrected #1: %f", correction_strength)
        data_wave_corrected = fullWaveVectorCorrection(
            wave,
            correction_high_frequency,
            correction_low_frequency,
            correction_strength
        );
    } else if (wave_correction_type == 2) {
        auto frame_rate = this->getWaveFrameRate();
        auto wave_dynamic_zero_crossing_frame_sec = config->waveDynamicZeroCrossingFrameSec();
        auto wave_dynamic_zero_crossing_shift_sec = config->waveDynamicZeroCrossingShiftSec();
        auto wave_dynamic_zero_crossing_target = config->waveDynamicZeroCrossingTarget();
        auto wave_dynamic_strength_target = config->waveDynamicStrengthTarget();
        data_wave_corrected = dynamicWaveVectorCorrection(
            wave,
            frame_rate,
            wave_dynamic_zero_crossing_target,
            wave_dynamic_strength_target,
            frame_rate * wave_dynamic_zero_crossing_frame_sec,
            frame_rate * wave_dynamic_zero_crossing_shift_sec
        );
    } else if (wave_correction_type == 3) {
        auto frame_rate = this->getWaveFrameRate();
        auto wave_dynamic_zero_crossing_frame_sec = config->waveDynamicZeroCrossingFrameSec();
        auto wave_dynamic_zero_crossing_shift_sec = config->waveDynamicZeroCrossingShiftSec();
        auto wave_dynamic_zero_crossing_target = config->waveDynamicZeroCrossingTarget();
        auto wave_dynamic_strength_target = config->waveDynamicStrengthTarget();
        data_wave_corrected = waveVectorZeroCrossingCorrection(
            wave,
            frame_rate,
            wave_dynamic_zero_crossing_target
        );
        data_wave_corrected = dynamicWaveStrengthVectorCorrection(
            data_wave_corrected,
            wave_dynamic_strength_target,
            frame_rate * wave_dynamic_zero_crossing_frame_sec,
            frame_rate * wave_dynamic_zero_crossing_shift_sec
        );
    } else if (wave_correction_type == 4) {
        auto correction_high_frequency = config->waveCorrectionHighFrequency();
        auto correction_low_frequency = config->waveCorrectionLowFrequency();
        auto correction_strength = config->waveCorrectionStrength();
        DEBUG("Get wave corrected #1")
        DEBUG("Get wave corrected #1: %f", correction_high_frequency)
        DEBUG("Get wave corrected #1: %f", correction_low_frequency)
        DEBUG("Get wave corrected #1: %f", correction_strength)
        data_wave_corrected = fullWaveVectorCorrection(
            wave,
            correction_high_frequency,
            correction_low_frequency,
            correction_strength,
            true
        );
    }

    this->data_wave_corrected.setValue(data_wave_corrected);

    return data_wave_corrected;
}

double Storage::getWaveRMS()
{
    RETURN_VALUE_IF_EXIST(this->data_wave_rms)

    DEBUG("Get wave RMS")

    auto data = this->getWaveCorrected();

    if (data.empty()) return 0.0;

    auto rms = calculateVectorRMS(data);

    this->data_wave_rms.setValue(rms);

    return rms;
}

std::vector<double> Storage::getWaveDynamicRMS()
{
    RETURN_VALUE_IF_EXIST(this->data_wave_dynamic_rms)

    DEBUG("Get wave dynamic RMS")

    auto data = this->getWaveCorrected();

    if (data.empty()) return std::vector<double>();

    auto frame_sec = config->waveDynamicZeroCrossingFrameSec();
    auto fr = this->getWaveFrameRate();
    uint32_t frame = fr * frame_sec;
    auto shift_sec = config->waveDynamicZeroCrossingShiftSec();
    uint32_t shift = fr * shift_sec;

    auto zc = calculateDynamicVectorRMS(data, frame, shift);

    this->data_wave_dynamic_rms.setValue(zc);

    return zc;
}

double Storage::getWaveZeroCrossing()
{
    RETURN_VALUE_IF_EXIST(this->data_wave_zero_crossing)

    DEBUG("Get wave ZeroCrossing")

    auto data = this->getWaveCorrected();

    if (data.empty()) return 0.0;

    auto fr = this->getWaveFrameRate();
    auto zc = calculateVectorZeroCrossing(data, fr);

    this->data_wave_zero_crossing.setValue(zc);

    return zc;
}

std::vector<double> Storage::getWaveDynamicZeroCrossing()
{
    RETURN_VALUE_IF_EXIST(this->data_wave_dynamic_zero_crossing)

    DEBUG("Get wave dynamic ZeroCrossing")

    auto data = this->getWaveCorrected();

    if (data.empty()) return std::vector<double>();

    auto frame_sec = config->waveDynamicZeroCrossingFrameSec();
    auto fr = this->getWaveFrameRate();
    uint32_t frame = fr * frame_sec;
    auto shift_sec = config->waveDynamicZeroCrossingShiftSec();
    uint32_t shift = fr * shift_sec;
    DEBUG("Get wave ZeroCrossing: frame_sec=%f", frame_sec)
    DEBUG("Get wave ZeroCrossing: fr=%i", fr)
    DEBUG("Get wave ZeroCrossing: frame=%i", frame)
    DEBUG("Get wave ZeroCrossing: data.size=%lo", data.size())

    auto zc = calculateDynamicVectorZeroCrossing(data, fr, frame, shift);

    this->data_wave_dynamic_zero_crossing.setValue(zc);

    return zc;
}

std::vector<double> Storage::Storage::getWaveNormalized()
{
    RETURN_VALUE_IF_EXIST(this->data_wave_normalized)

    DEBUG("Get wave normalized")

    auto wave = this->getWaveCorrected();

    if (wave.empty()) return std::vector<double>();

    auto wave_mormalized = normalizeVector(wave, WAVE_NORMALIZED_MIN, WAVE_NORMALIZED_MAX);

    this->data_wave_normalized.setValue(wave_mormalized);

    return wave_mormalized;
}

void Storage::cleanWave()
{
    if (this->data_wave.isExists())
        this->data_wave.clear();
    if (this->data_wave_corrected.isExists())
        this->data_wave_corrected.clear();
    if (this->data_wave_normalized.isExists())
        this->data_wave_normalized.clear();
    if (this->data_wave_rms.isExists())
        this->data_wave_rms.clear();
    if (this->data_wave_zero_crossing.isExists())
        this->data_wave_zero_crossing.clear();
    if (this->data_wave_dynamic_zero_crossing.isExists())
        this->data_wave_dynamic_zero_crossing.clear();
    if (this->data_wave_dynamic_rms.isExists())
        this->data_wave_dynamic_rms.clear();

    this->cleanIntensity();
    this->cleanPitch();
    this->cleanSegments();
    this->cleanSpectrum();
}
