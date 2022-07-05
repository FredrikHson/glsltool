#include "v7.h"
#include "resources.h"
#include "defines.h"
#include <stdlib.h>
#include "audio.h"

typedef struct v7 v7;

enum v7_err js_play_sound(v7* v7e, v7_val_t* res)
{
    int numvars = v7_argc(v7e);

    if(numvars > 4 ||  numvars == 0)
    {
        fprintf(stderr, "invalid number of arguments to openmesh\n");
        return V7_SYNTAX_ERROR;
    }

    int id = v7_get_int(v7e, v7_arg(v7e, 0));
    unsigned int group = 0;
    int loop = 0;
    float volume = 1.0f;

    if(numvars > 1)
    {
        group = v7_get_int(v7e, v7_arg(v7e, 1));
    }

    if(numvars > 2)
    {
        volume = v7_get_double(v7e, v7_arg(v7e, 1));
    }

    if(numvars > 3)
    {
        loop = v7_get_int(v7e, v7_arg(v7e, 1));
    }

    int runningsoundid = play_sound(id, group, volume, loop);

    *res = v7_mk_number(v7e, runningsoundid);

    return V7_OK;
}

enum v7_err js_load_sound(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);
    *res = v7_mk_number(v7e, 0);

    if(argc == 1)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* filename = v7_get_string(v7e, &val, &len);


        if(filename)
        {
            int sound = loadSound(filename);

            if(sound == -1)
            {
                fprintf(stderr, "failed to load sound %s\n", filename);
                return V7_OK;
            }

            printf("soundid %i\n", sound);
            *res = v7_mk_number(v7e, sound);
            return V7_OK;
        }
        else
        {
            fprintf(stderr, "first argument to loadsound should be a string\n");
            return V7_SYNTAX_ERROR;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadsound\n");
        return V7_SYNTAX_ERROR;
    }
}

enum v7_err js_set_global_volume(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);
    *res = v7_mk_number(v7e, 0);

    if(argc == 1)
    {
        float volume = v7_get_double(v7e, v7_arg(v7e, 0));
        set_global_volume(volume);
        return V7_OK;
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setglobalvolume\n");
        return V7_SYNTAX_ERROR;
    }

}
enum v7_err js_set_group_volume(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);
    *res = v7_mk_number(v7e, 0);

    if(argc == 2)
    {
        float volume = v7_get_double(v7e, v7_arg(v7e, 1));
        unsigned int group = v7_get_int(v7e, v7_arg(v7e, 0));
        set_group_volume(group, volume);
        return V7_OK;
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setgroupvolume\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_stop_all_sounds(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);
    *res = v7_mk_number(v7e, 0);

    if(argc == 0)
    {
        stop_all_sounds();
        return V7_OK;
    }
    else if(argc == 1)
    {
        unsigned int group = v7_get_int(v7e, v7_arg(v7e, 0));
        stop_group_sounds(group);
        return V7_OK;

    }
    else
    {
        fprintf(stderr, "invalid number of arguments to stopallsounds\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}
