#include <miniaudio.h>
#include "resources.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "audio.h"
#include "notify.h"

sound* sounds = 0;
int numsounds = 0;
extern ma_engine audio_engine;

char* strdupnull(const char* str);

void reloadSound(const char* filename)
{
    for(int i = 0; i  < numsounds; i ++)
    {
        if(sounds[i].name != 0)
        {
            if(strcmp(filename, sounds[i].name) == 0)
            {
                ma_sound_uninit(&sounds[i].sound);
                ma_sound_init_from_file(&audio_engine, filename, 0, 0, 0, &sounds[i].sound);
                break;
            }

        }

    }

}

int loadSound(const char* filename)
{
    unsigned int out  = 0;

    if(filename != 0)
    {
        for(int i = 0; i < numsounds; i++)
        {
            if(!(sounds[i].cleanup & CLEAN_DELETED) && sounds[i].name != 0)
            {
                if(strcmp(filename, sounds[i].name) == 0)
                {
                    sounds[i].cleanup = CLEAN_USED;
                    printf("already loaded sound %s id:%i\n", filename, i);
                    return i;
                }
            }
        }
    }

    FILE* f = fopen(filename, "rb");

    if(!f)
    {
        return -1;
    }
    else
    {
        fclose(f);
    }

    if(numsounds == 0)
    {
        sounds = malloc(sizeof(sound));
        out = 0;
        numsounds = 1;
    }
    else
    {
        char reuse = 0;

        for(int i = 0; i < numsounds; i++)
        {
            if((sounds[i].cleanup & CLEAN_DELETED))
            {
                reuse = 1;
                out = i;
            }
        }

        if(reuse == 0)
        {
            void* newsounds = realloc(sounds, sizeof(sound) * (numsounds + 1));

            if(newsounds == 0)
            {
                fprintf(stderr, "out of memory while creating sound %s\n", filename);
                exit(1);
            }
            else
            {
                sounds = newsounds;
                out = numsounds;
                numsounds += 1;
            }
        }
    }

    sounds[out].cleanup = CLEAN_USED;
    sounds[out].name = strdup(filename);
    ma_result r = ma_sound_init_from_file(&audio_engine, filename, 0, 0, 0, &sounds[out].sound);

    if(r != MA_SUCCESS)
    {
        return -1;
    }

    watchFile(filename, reloadSound);
    return out;
}
