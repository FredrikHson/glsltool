#include <miniaudio.h>
#include "resources.h"
#include "audio.h"
#include "defines.h"
#include <stdio.h>

ma_engine audio_engine;
ma_sound_group audio_groups[MAX_AUDIOGROUPS];
extern sound* sounds;
extern int numsounds;

#define MAX_CONCURRENT_SOUNDS 65536
ma_sound audioringbuffer[MAX_CONCURRENT_SOUNDS] = {0};
int audioringbuffer_group[MAX_CONCURRENT_SOUNDS] = {0};

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

int play_sound(int id, int group, float volume, int looping)
{
    if(id >= numsounds || id == -1)
    {
        return MAX_AUDIOGROUPS;
    }

    if(sounds[id].cleanup != CLEAN_USED)
    {
        return MAX_AUDIOGROUPS;
    }

    int index = 0;

    for(index = 0; index < MAX_CONCURRENT_SOUNDS; index++)
    {
        if(ma_sound_is_playing(&audioringbuffer[index]) == 0)
        {
            ma_sound_uninit(&audioringbuffer[index]);
            break;
        }
    }

    ma_sound_init_copy(&audio_engine, &sounds[id].sound, 0, &audio_groups[group], &audioringbuffer[index]);

    if(looping)
    {
        ma_sound_set_looping(&audioringbuffer[index], 1);
    }

    ma_sound_set_volume(&audioringbuffer[index], volume);
    ma_sound_start(&audioringbuffer[index]);
    audioringbuffer_group[index] = group;
    return index;
}

void set_global_volume(float volume)
{
    printf("setting volume to:%f\n", volume);
    ma_engine_set_volume(&audio_engine, volume);
}

void set_group_volume(int group, float volume)
{
    if(group < MAX_AUDIOGROUPS)
    {
        ma_sound_group_set_volume(&audio_groups[group], volume);
    }
}

void stop_all_sounds()
{
    for(int index = 0; index < MAX_CONCURRENT_SOUNDS; index++)
    {
        if(ma_sound_is_playing(&audioringbuffer[index]))
        {
            ma_sound_stop(&audioringbuffer[index]);
        }
    }
}

void stop_group_sounds(int group)
{
    if(group < MAX_AUDIOGROUPS)
    {
        for(int index = 0; index < MAX_CONCURRENT_SOUNDS; index++)
        {
            if(audioringbuffer_group[index] == group)
            {
                if(ma_sound_is_playing(&audioringbuffer[index]))
                {
                    ma_sound_stop(&audioringbuffer[index]);
                }

            }

        }
    }
}
