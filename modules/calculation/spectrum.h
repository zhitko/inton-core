#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <vector>
#include <cstdint>
#include <cfloat>


namespace IntonCore {

const uint32_t kDefaultFrameLength(256);
const uint32_t kDefaultFftLength(256);
const uint32_t kDefaultOutputFormat(0);
const double kDefaultEpsilon(0.0);
const double kDefaultRelativeFloorInDecibels(-DBL_MAX);

std::vector<double> waveToSpectrum(
        std::vector<double> waveform,
        uint32_t frame_length = kDefaultFrameLength,                        // frame_length Frame length, @f$L@f$.
        uint32_t fft_length = kDefaultFftLength,                            // fft_length FFT length, @f$N@f$.
        uint32_t output_format = kDefaultOutputFormat,                      // output_format Output format. [0: kLogAmplitudeSpectrumInDecibels, 1: kLogAmplitudeSpectrum, 2: kAmplitudeSpectrum, 3: kPowerSpectrum, 4: kNumInputOutputFormats]
        double epsilon = kDefaultEpsilon,                                   // epsilon Small value added to power spectrum.
        double relative_floor_in_decibels = kDefaultRelativeFloorInDecibels // relative_floor_in_decibels Relative floor in decibels.
);

std::vector<double> waveToSpectrum2D(
    std::vector<double> spectrum,
    uint32_t fft_length = kDefaultFftLength                                 // fft_length FFT length, @f$N@f$.
);

}

#endif // SPECTRUM_H
