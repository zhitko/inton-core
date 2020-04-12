#include "intensity.h"

#include <cmath>

#include "define.h"
#include "modules/wav_file/wavFile.h"


std::vector<double> IntonCore::waveToIntensive(std::vector<double> wave, uint32_t frame, uint32_t shift)
{
    DEBUG("waveToIntensive %i, %i", frame, shift)
    if (wave.empty()) return std::vector<double>();

    std::vector<double> result;

    int wave_size = wave.size();

    double middle = 0.0;
    int half_frame = static_cast<int>(std::round(frame / 2));

    int start_position = -half_frame;
    int stop_position = half_frame;
    int finished_at = wave_size - half_frame;

    while(stop_position < finished_at)
    {
        middle = 0.0;

        for(int j=start_position; j<stop_position; j++)
        {
            if (j < 0) continue;
            else if (j >= wave_size) continue;
            middle += fabs(wave.at(j));
        }

        result.push_back(middle / (frame));

        start_position += shift;
        stop_position += shift;
    }

    return result;
}
