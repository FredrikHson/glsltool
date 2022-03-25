#include <miniaudio.h>
#include "audio.h"
#include "defines.h"

ma_engine audio_engine;
ma_sound_group audio_groups[MAX_AUDIOGROUPS];

int initAudio()
{
    ma_result result;
    result = ma_engine_init(0, &audio_engine);

    if(result != MA_SUCCESS)
    {
        return 0;
    }

    for(int i = 0; i < MAX_AUDIOGROUPS; i++)
    {
        ma_sound_group_init(&audio_engine, 0, 0, &audio_groups[i]);
    }

    return 1;
}

void shutdownAudio()
{
    ma_engine_uninit(&audio_engine);
}

/*void play_sound(const char* filename,*/
