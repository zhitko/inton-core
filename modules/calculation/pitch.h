#ifndef PITCH_H
#define PITCH_H

#include <vector>
#include <cstdint>


namespace IntonCore {

const int kDefaultFrameShift(80);
const double kDefaultSamplingRate(8.0);
const double kDefaultLowerF0(60.0);
const double kDefaultUpperF0(240.0);
const double kDefaultVoicingThresholdForRapt(0.0);
const double kDefaultVoicingThresholdForSwipe(0.3);
const double kDefaultVoicingThresholdForReaper(0.9);
const double kDefaultVoicingThresholdForWorld(0.1);

std::vector<double> waveToPitch(
        std::vector<double> waveform,
        uint32_t atype = 3,                                     // Algorithm used for extraction of pitch. [0: RAPT, 1: SWIPE, 2: Reaper, 3: World, 4: NumAlgorithms]
        uint32_t otype = 1,                                     // Output format. [0:pitch, 1:F0, 2:log(F0), 3: NumOutputFormats]
        uint32_t frame_shift = kDefaultFrameShift,              // Frame shift
        double thresh_rapt = kDefaultVoicingThresholdForRapt,   // Voiced/unvoiced threhold (used only for RAPT algorithm)
        double thresh_swipe = kDefaultVoicingThresholdForSwipe, // Voiced/unvoiced threhold (used only for SWIPE algorithm)
        double sample_freq = kDefaultSamplingRate,              // Samplingfrequency (kHz)
        double min_freq = kDefaultLowerF0,                      // Minimum fundamental frequency to search for (Hz)
        double max_freq = kDefaultUpperF0                       // Maximum fundamental frequency to search for (Hz)
);

}

#endif // PITCH_H
