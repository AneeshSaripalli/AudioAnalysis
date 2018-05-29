//
// Created by Aneesh on 5/25/2018.
//

#include "audio_process.h"

#define format          WAVE_FORMAT_PCM
#define channels        1
#define samples         22050
#define bitspersample   8
#define avgbitspersec   22050
#define blockalign      1
#define header          0

void test() {

    HWAVEIN wave_handle;

    WAVEFORMATEX wave_format;
    wave_format.wFormatTag = format;
    wave_format.nChannels = 1;
    wave_format.nSamplesPerSec = 22050;
    wave_format.wBitsPerSample = 8;
    wave_format.nAvgBytesPerSec = 22050;
    wave_format.nBlockAlign = 1;
    wave_format.cbSize = header;

    int Res = waveInOpen(&wave_handle, WAVE_MAPPER,
                         &wave_format, 0, 0, WAVE_FORMAT_QUERY);

    if (Res == WAVERR_BADFORMAT) {
        printf("audio query was bad.\n");
        return;
    }
};