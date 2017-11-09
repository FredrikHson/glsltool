#include <math.h>
#include "opengl.h"
#include "v7.h"
#include "renderfunc.h"
#include "resources.h"
#include "defines.h"
typedef struct v7 v7;
unsigned int default_magfilter = GL_LINEAR;
unsigned int default_minfilter = GL_LINEAR;
unsigned int default_clamp_s = GL_REPEAT;
unsigned int default_clamp_t = GL_REPEAT;

enum v7_err js_bind_attrib(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        v7_val_t val2 = v7_arg(v7e, 1);
        const char* name = v7_get_string(v7e, &val, &len);
        const int flag = v7_get_int(v7e, val2);

        if(!bindAttrib(name, flag))
        {
            fprintf(stderr, "failed to assign attribute %s flag:%i\n", name, flag);
            return V7_INTERNAL_ERROR;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to bindattribute\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_reset_attribs(v7* v7e, v7_val_t* res)
{
    resetAttribs();
    return V7_OK;
}

enum v7_err js_bind_shader(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const int shaderid = v7_get_int(v7e, v7_arg(v7e, 0));
        bindShader(shaderid);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to bindshader\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_set_uniformf(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc >= 2 && argc <= 5)
    {
        v7_val_t val = v7_arg(v7e, 0);
        size_t len = 0;
        const char* name = v7_get_string(v7e, &val, &len);
        float vars[4] = {0};

        for(int i = 1; i < argc; i++)
        {
            v7_val_t val2 = v7_arg(v7e, i);
            vars[i - 1] = v7_get_double(v7e, val2);
        }

        setUniformf(name, vars, argc - 1);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setuniformf\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_set_uniformi(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc >= 2 && argc <= 5)
    {
        v7_val_t val = v7_arg(v7e, 0);
        size_t len = 0;
        const char* name = v7_get_string(v7e, &val, &len);
        int vars[4] = {0};

        for(int i = 1; i < argc; i++)
        {
            v7_val_t val2 = v7_arg(v7e, i);
            vars[i - 1] = v7_get_int(v7e, val2);
        }

        setUniformi(name, vars, argc - 1);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setuniformi\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_set_uniformui(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc >= 2 && argc <= 5)
    {
        v7_val_t val = v7_arg(v7e, 0);
        size_t len = 0;
        const char* name = v7_get_string(v7e, &val, &len);
        unsigned int vars[4] = {0};

        for(int i = 1; i < argc; i++)
        {
            v7_val_t val2 = v7_arg(v7e, i);
            vars[i - 1] = v7_get_int(v7e, val2);
        }

        setUniformui(name, vars, argc - 1);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setUniformui\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_bind_texture(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc >= 2)
    {
        v7_val_t val = v7_arg(v7e, 0);
        size_t len = 0;
        const char* name = v7_get_string(v7e, &val, &len);
        v7_val_t val2 = v7_arg(v7e, 1);
        int textureid = v7_get_int(v7e, val2);
        unsigned int magfilter = default_magfilter;
        unsigned int minfilter = default_minfilter;
        unsigned int clamp_s = default_clamp_s;
        unsigned int clamp_t = default_clamp_t;

        if(argc >= 3)
        {
            magfilter = v7_get_int(v7e, v7_arg(v7e, 2));
        }

        if(argc >= 4)
        {
            minfilter = v7_get_int(v7e, v7_arg(v7e, 3));
        }

        if(argc >= 5)
        {
            clamp_s = v7_get_int(v7e, v7_arg(v7e, 4));
            clamp_t = clamp_s;
        }

        if(argc >= 6)
        {
            clamp_t = v7_get_int(v7e, v7_arg(v7e, 5));
        }

        bindTexture(name, textureid, magfilter, minfilter, clamp_s, clamp_t);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to bindtexture\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_bind_rendertarget(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc >= 2)
    {
        v7_val_t val = v7_arg(v7e, 0);
        size_t len = 0;
        const char* name = v7_get_string(v7e, &val, &len);
        int textureid = v7_get_int(v7e, v7_arg(v7e, 1));
        int layer = v7_get_int(v7e, v7_arg(v7e, 2));
        unsigned int magfilter = default_magfilter;
        unsigned int minfilter = default_minfilter;
        unsigned int clamp_s = default_clamp_s;
        unsigned int clamp_t = default_clamp_t;

        if(argc >= 4)
        {
            magfilter = v7_get_int(v7e, v7_arg(v7e, 3));
        }

        if(argc >= 5)
        {
            minfilter = v7_get_int(v7e, v7_arg(v7e, 4));
        }

        if(argc >= 6)
        {
            clamp_s = v7_get_int(v7e, v7_arg(v7e, 5));
            clamp_t = clamp_s;
        }

        if(argc >= 7)
        {
            clamp_t = v7_get_int(v7e, v7_arg(v7e, 6));
        }

        bindRendertarget(name,  textureid, layer, magfilter, minfilter, clamp_s, clamp_t);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to bindtexture\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_set_default_filter(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        default_magfilter = v7_get_int(v7e, v7_arg(v7e, 0));
        default_minfilter = v7_get_int(v7e, v7_arg(v7e, 1));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setdefaultfilter\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_set_default_clamp(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        default_clamp_s = v7_get_int(v7e, v7_arg(v7e, 0));
        default_clamp_t = v7_get_int(v7e, v7_arg(v7e, 1));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setdefaultclamp\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}
