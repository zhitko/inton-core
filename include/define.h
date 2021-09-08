#ifndef DEFINE_H
#define DEFINE_H

#include "utils.h"
#include <cstdio>
#include <cstdio>

#define WHEREAMI    string_format("[%s] %s: %s", __func__, __FILE__, __LINE__)

#define LOG_LEVEL 4
#define _fe(...) fprintf(stdout, __VA_ARGS__)
#define _fo(...) fprintf(stdout, __VA_ARGS__)
#define _if(level) if (LOG_LEVEL > level)
#define ERROR(...) _if(0) { _fe("\nERROR: "); _fe(__VA_ARGS__); }
#define WARN(...)  _if(1) { _fo("\nWARN:  "); _fo(__VA_ARGS__); }
#define INFO(...)  _if(2) { _fo("\nINFO:  "); _fo(__VA_ARGS__); }
#define DEBUG(...) _if(3) { _fo("\nDEBUG: "); _fo(__VA_ARGS__); }

#define WAVE_FRAME_RATE 8000

#define WAVE_FILE_SEGMENT_PRE_NUCLEUS 'P'
#define WAVE_FILE_SEGMENT_NUCLEUS 'N'
#define WAVE_FILE_SEGMENT_POST_NUCLEUS 'T'

#define WAVE_NORMALIZED_MIN -1.0
#define WAVE_NORMALIZED_MAX 1.0

#define DATA_NORMALIZED_MIN 0.0
#define DATA_NORMALIZED_MAX 1.0
#define DATA_NORMALIZED_LIMIT 0.009

#define CONFIG_DEFUALT_INTENSIVE_FRAME 240
#define CONFIG_DEFUALT_INTENSIVE_SHIFT 120
#define CONFIG_DEFUALT_INTENSIVE_SMOOTH_FRAME 120
#define CONFIG_DEFUALT_INTENSIVE_DOUBLE_SMOOTH_FRAME 120
#define SEGMANTS_BY_INTENSITY_THRESHOLD_ABSOLUTE 0.5
#define SEGMANTS_BY_INTENSITY_THRESHOLD_RELATIVE 0.1
#define SEGMANTS_BY_INTENSITY_MIN_LENGTH 15
#define SEGMANTS_BY_INTENSITY_DOUBLE_SMOOTHED_MIN_LENGTH 15

#endif // DEFINE_H
