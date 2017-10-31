#include "script.h"
#include <stdio.h>
#include "opengl.h"
#include "v7.h"
#include "renderfunc.h"
#include "resources.h"
#include "defines.h"
#include "vector.h"
#include "matrix.h"

typedef struct v7 v7;

unsigned char validscript = 0;
v7* v7g = 0;

static enum v7_err js_create_rendertarget(v7* v7e, v7_val_t* res)
{
    if(v7_argc(v7e) < 5)
    {
        fprintf(stderr, "invalid number of arguments to create_rendertarget\n");
        return V7_SYNTAX_ERROR;
    }

    int width  = v7_get_int(v7e, v7_arg(v7e, 0));
    int height = v7_get_int(v7e, v7_arg(v7e, 1));
    int layers = v7_get_int(v7e, v7_arg(v7e, 2));
    int colors = v7_get_int(v7e, v7_arg(v7e, 3));
    int type   = v7_get_int(v7e, v7_arg(v7e, 4));
    int magfilter = GL_NEAREST;
    int minfilter = GL_NEAREST;

    if(v7_argc(v7e) > 5)
    {
        magfilter = v7_get_int(v7e, v7_arg(v7e, 6));
    }

    if(v7_argc(v7e) > 6)
    {
        minfilter = v7_get_int(v7e, v7_arg(v7e, 7));
    }

    unsigned int target = CreateRenderTarget(width, height, layers, colors, type, minfilter, magfilter);
    *res = v7_mk_number(v7e, target);
    return V7_OK;
}
static enum v7_err js_clear(v7* v7e, v7_val_t* res)
{
    float red   = 0;
    float green = 0;
    float blue  = 0;
    float alpha = 0;
    int argc = v7_argc(v7e);

    if(argc == 3)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = v7_get_double(v7e, v7_arg(v7e, 1));
        blue  = v7_get_double(v7e, v7_arg(v7e, 2));
    }
    else if(argc == 4)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = v7_get_double(v7e, v7_arg(v7e, 1));
        blue  = v7_get_double(v7e, v7_arg(v7e, 2));
        alpha = v7_get_double(v7e, v7_arg(v7e, 3));
    }
    else if(argc == 1)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = red;
        blue  = red;
    }
    else if(argc == 0) {}
    else
    {
        fprintf(stderr, "invalid number of arguments to clear\n");
        return V7_SYNTAX_ERROR;
    }

    clear(red, green, blue, alpha);
    return V7_OK;
}

static enum v7_err js_beginPass(v7* v7e, v7_val_t* res)
{
    int width;
    int height;

    if(v7_argc(v7e) == 0)
    {
        beginPass(-1, &width, &height);
    }
    else
    {
        beginPass(v7_get_int(v7e, v7_arg(v7e, 0)), &width, &height);
    }

    v7_set(v7g, v7_get_global(v7g), "RENDER_WIDTH", 12, v7_mk_number(v7g, width));
    v7_set(v7g, v7_get_global(v7g), "RENDER_HEIGHT", 13, v7_mk_number(v7g, height));
    return V7_OK;
}
static enum v7_err js_endPass(v7* v7e, v7_val_t* res)
{
    endPass();
    return V7_OK;
}

static enum v7_err js_load_image(v7* v7e, v7_val_t* res)
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
            int image = loadImage(filename);

            if(image == -1)
            {
                fprintf(stderr, "failed to load image %s\n", filename);
                return V7_OK;
            }

            printf("imageid %i\n", image);
            *res = v7_mk_number(v7e, image);
            return V7_OK;
        }
        else
        {
            fprintf(stderr, "first argument to loadmesh should be a string\n");
            return V7_SYNTAX_ERROR;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadimage\n");
        return V7_SYNTAX_ERROR;
    }
}

static enum v7_err js_load_mesh(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* filename = v7_get_string(v7e, &val, &len);

        if(filename)
        {
            int mesh = loadMesh(filename);

            if(mesh == -1)
            {
                fprintf(stderr, "failed to load mesh %s\n", filename);
            }
            else
            {
                printf("meshid %i\n", mesh);
            }

            *res = v7_mk_number(v7e, mesh);
            return V7_OK;
        }
        else
        {
            fprintf(stderr, "first argument to loadmesh should be a string\n");
            return V7_SYNTAX_ERROR;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadmesh\n");
        return V7_SYNTAX_ERROR;
    }
}

static enum v7_err js_draw_mesh(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const int meshid = v7_get_int(v7e, v7_arg(v7e, 0));
        drawMesh(meshid, -1);
    }
    else if(argc == 2)
    {
        const int meshid = v7_get_int(v7e, v7_arg(v7e, 0));
        const int submesh = v7_get_int(v7e, v7_arg(v7e, 1));
        drawMesh(meshid, submesh);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadmesh\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_load_shader(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 5)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* vertfilename = v7_get_string(v7e, &val, &len);
        val = v7_arg(v7e, 1);
        const char* pixelfilename = v7_get_string(v7e, &val, &len);
        val = v7_arg(v7e, 2);
        const char* geomfilename = v7_get_string(v7e, &val, &len);
        val = v7_arg(v7e, 3);
        const char* controlfilename = v7_get_string(v7e, &val, &len);
        val = v7_arg(v7e, 4);
        const char* evalfilename = v7_get_string(v7e, &val, &len);
        int shader = loadShader(vertfilename,
                                pixelfilename,
                                geomfilename,
                                controlfilename,
                                evalfilename);

        if(shader == -1)
        {
            fprintf(stderr, "Failed to load shader\n"
                    "    vertex:%s\n"
                    "    fragment:%s\n"
                    "    geometry:%s\n"
                    "    control:%s\n"
                    "    evaluation:%s\n",
                    vertfilename,
                    pixelfilename,
                    geomfilename,
                    controlfilename,
                    evalfilename);
        }

        *res = v7_mk_number(v7e, shader);
        return V7_OK;
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadmesh\n");
        return V7_SYNTAX_ERROR;
    }
}
static enum v7_err js_bind_attrib(v7* v7e, v7_val_t* res)
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
static enum v7_err js_reset_attribs(v7* v7e, v7_val_t* res)
{
    resetAttribs();
    return V7_OK;
}

static enum v7_err js_bind_shader(v7* v7e, v7_val_t* res)
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

static enum v7_err js_set_uniformf(v7* v7e, v7_val_t* res)
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

static enum v7_err js_set_uniformi(v7* v7e, v7_val_t* res)
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

static enum v7_err js_set_uniformui(v7* v7e, v7_val_t* res)
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

static enum v7_err js_set_depth(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const int enable = v7_get_int(v7e, v7_arg(v7e, 0));

        if(enable)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to bindshader\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_set_cullface(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const int cullmode = v7_get_int(v7e, v7_arg(v7e, 0));

        switch(cullmode)
        {
            case CULL_NONE:
                glDisable(GL_CULL_FACE);
                break;

            case CULL_FRONT:
                glCullFace(GL_FRONT);
                glEnable(GL_CULL_FACE);
                break;

            case CULL_BACK:
                glCullFace(GL_BACK);
                glEnable(GL_CULL_FACE);
                break;

            case CULL_BOTH:
                glCullFace(GL_FRONT_AND_BACK);
                glEnable(GL_CULL_FACE);
                break;

            default:
                break;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to bindshader\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_vec3_dot(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            vec3 v2;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            v2.x = v7_get_double(v7e, v7_get(v7e, arg2, "x", 1));
            v2.y = v7_get_double(v7e, v7_get(v7e, arg2, "y", 1));
            v2.z = v7_get_double(v7e, v7_get(v7e, arg2, "z", 1));
            *res = v7_mk_number(v7e, vec3dot(v1, v2));
        }
    }

    return V7_OK;
}

static enum v7_err js_vec3_normalize(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);

        if(v7_is_object(arg1))
        {
            vec3 v1 = {0};
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            vec3 normalized = vec3normalize(v1);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, normalized.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, normalized.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, normalized.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3normalize\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

static enum v7_err js_vec3_cross(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            vec3 v2;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            v2.x = v7_get_double(v7e, v7_get(v7e, arg2, "x", 1));
            v2.y = v7_get_double(v7e, v7_get(v7e, arg2, "y", 1));
            v2.z = v7_get_double(v7e, v7_get(v7e, arg2, "z", 1));
            vec3 crossproduct = vec3cross(v1, v2);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, crossproduct.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, crossproduct.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, crossproduct.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3cross\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

static enum v7_err js_vec3_add(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            vec3 v2;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            v2.x = v7_get_double(v7e, v7_get(v7e, arg2, "x", 1));
            v2.y = v7_get_double(v7e, v7_get(v7e, arg2, "y", 1));
            v2.z = v7_get_double(v7e, v7_get(v7e, arg2, "z", 1));
            vec3 add = vec3add(v1, v2);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, add.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, add.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, add.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3add\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

static enum v7_err js_vec3_sub(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            vec3 v2;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            v2.x = v7_get_double(v7e, v7_get(v7e, arg2, "x", 1));
            v2.y = v7_get_double(v7e, v7_get(v7e, arg2, "y", 1));
            v2.z = v7_get_double(v7e, v7_get(v7e, arg2, "z", 1));
            vec3 sub = vec3add(v1, v2);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, sub.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, sub.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, sub.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3sub\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

static enum v7_err js_vec3_mul(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1))
        {
            vec3 v1;
            float scalar;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            scalar = v7_get_double(v7e, arg2);
            vec3 mul = vec3mul(v1, scalar);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, mul.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, mul.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, mul.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3mul\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

v7_val_t mat4tov7_val(v7* v7e, const mat4 m)
{
    v7_val_t out = v7_mk_object(v7e);
    v7_set(v7e, out, "m11", 3, v7_mk_number(v7e, m.m[0]));
    v7_set(v7e, out, "m12", 3, v7_mk_number(v7e, m.m[1]));
    v7_set(v7e, out, "m13", 3, v7_mk_number(v7e, m.m[2]));
    v7_set(v7e, out, "m14", 3, v7_mk_number(v7e, m.m[3]));
    v7_set(v7e, out, "m21", 3, v7_mk_number(v7e, m.m[4]));
    v7_set(v7e, out, "m22", 3, v7_mk_number(v7e, m.m[5]));
    v7_set(v7e, out, "m23", 3, v7_mk_number(v7e, m.m[6]));
    v7_set(v7e, out, "m24", 3, v7_mk_number(v7e, m.m[7]));
    v7_set(v7e, out, "m31", 3, v7_mk_number(v7e, m.m[8]));
    v7_set(v7e, out, "m32", 3, v7_mk_number(v7e, m.m[9]));
    v7_set(v7e, out, "m33", 3, v7_mk_number(v7e, m.m[10]));
    v7_set(v7e, out, "m34", 3, v7_mk_number(v7e, m.m[11]));
    v7_set(v7e, out, "m41", 3, v7_mk_number(v7e, m.m[12]));
    v7_set(v7e, out, "m42", 3, v7_mk_number(v7e, m.m[13]));
    v7_set(v7e, out, "m43", 3, v7_mk_number(v7e, m.m[14]));
    v7_set(v7e, out, "m44", 3, v7_mk_number(v7e, m.m[15]));
    return out;
}

mat4 v7_val_tomat4(v7* v7e, v7_val_t m)
{
    mat4 out;
    out.m[0] = v7_get_double(v7e, v7_get(v7e, m, "m11", 3));
    out.m[1] = v7_get_double(v7e, v7_get(v7e, m, "m12", 3));
    out.m[2] = v7_get_double(v7e, v7_get(v7e, m, "m13", 3));
    out.m[3] = v7_get_double(v7e, v7_get(v7e, m, "m14", 3));
    out.m[4] = v7_get_double(v7e, v7_get(v7e, m, "m21", 3));
    out.m[5] = v7_get_double(v7e, v7_get(v7e, m, "m22", 3));
    out.m[6] = v7_get_double(v7e, v7_get(v7e, m, "m23", 3));
    out.m[7] = v7_get_double(v7e, v7_get(v7e, m, "m24", 3));
    out.m[8] = v7_get_double(v7e, v7_get(v7e, m, "m31", 3));
    out.m[9] = v7_get_double(v7e, v7_get(v7e, m, "m32", 3));
    out.m[10] = v7_get_double(v7e, v7_get(v7e, m, "m33", 3));
    out.m[11] = v7_get_double(v7e, v7_get(v7e, m, "m34", 3));
    out.m[12] = v7_get_double(v7e, v7_get(v7e, m, "m41", 3));
    out.m[13] = v7_get_double(v7e, v7_get(v7e, m, "m42", 3));
    out.m[14] = v7_get_double(v7e, v7_get(v7e, m, "m43", 3));
    out.m[15] = v7_get_double(v7e, v7_get(v7e, m, "m44", 3));
    return out;
}

static enum v7_err js_mat4_loadidentity(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 0)
    {
        *res = mat4tov7_val(v7e, mat4loadidentity());
    }
    else
    {
        fprintf(stderr, "mat4loadidentity does not take any arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_mat4_setscale(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const float scale = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = mat4tov7_val(v7e, mat4setscale(scale, scale, scale));
    }
    else if(argc == 3)
    {
        const float x = v7_get_double(v7e, v7_arg(v7e, 0));
        const float y = v7_get_double(v7e, v7_arg(v7e, 1));
        const float z = v7_get_double(v7e, v7_arg(v7e, 2));
        *res = mat4tov7_val(v7e, mat4setscale(x, y, z));
    }
    else
    {
        fprintf(stderr, "mat4setscale requires 1 or 3 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_mat4_settranslation(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 3)
    {
        const float x = v7_get_double(v7e, v7_arg(v7e, 0));
        const float y = v7_get_double(v7e, v7_arg(v7e, 1));
        const float z = v7_get_double(v7e, v7_arg(v7e, 2));
        *res = mat4tov7_val(v7e, mat4settranslation(x, y, z));
    }
    else
    {
        fprintf(stderr, "mat4settranslation requires 3 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_mat4_setrotation(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 4)
    {
        const float angle = v7_get_double(v7e, v7_arg(v7e, 0));
        const float x = v7_get_double(v7e, v7_arg(v7e, 1));
        const float y = v7_get_double(v7e, v7_arg(v7e, 2));
        const float z = v7_get_double(v7e, v7_arg(v7e, 3));
        *res = mat4tov7_val(v7e, mat4setrotr(angle, x, y, z));
    }
    else
    {
        fprintf(stderr, "mat4setrotation requires 4 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_mat4_setperspective(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 4)
    {
        const float fov = v7_get_double(v7e, v7_arg(v7e, 0));
        const float aspect = v7_get_double(v7e, v7_arg(v7e, 1));
        const float near = v7_get_double(v7e, v7_arg(v7e, 2));
        const float far = v7_get_double(v7e, v7_arg(v7e, 3));
        *res = mat4tov7_val(v7e, mat4setperspective(fov, aspect, near, far));
    }
    else
    {
        fprintf(stderr, "mat4setperspective requires 4 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_mat4_mul(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            const mat4 m1 = v7_val_tomat4(v7e, arg1);
            const mat4 m2 = v7_val_tomat4(v7e, arg2);
            *res = mat4tov7_val(v7e, mat4mul(&m1, &m2));
        }
        else
        {
            *res = mat4tov7_val(v7e, mat4loadidentity());
            fprintf(stderr, "mat4mul needs 2 matrices as arguments\n");
        }
    }
    else
    {
        fprintf(stderr, "mat4mul needs 2 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_set_uniform_matrix(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);
        size_t len = 0;
        const char* name = v7_get_string(v7e, &arg1, &len);

        if(v7_is_object(arg2))
        {
            const mat4 m1 = v7_val_tomat4(v7e, arg2);
            setUniformMat4(name, &m1);
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setuniformmat4\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

static enum v7_err js_bind_texture(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc >= 2)
    {
        v7_val_t val = v7_arg(v7e, 0);
        size_t len = 0;
        const char* name = v7_get_string(v7e, &val, &len);
        v7_val_t val2 = v7_arg(v7e, 1);
        int textureid = v7_get_int(v7e, val2);
        unsigned int magfilter = GL_LINEAR;
        unsigned int minfilter = GL_LINEAR;

        if(argc >= 3)
        {
            magfilter = v7_get_int(v7e, v7_arg(v7e, 2));
        }

        if(argc >= 4)
        {
            minfilter = v7_get_int(v7e, v7_arg(v7e, 3));
        }

        bindTexture(name, textureid, magfilter, minfilter);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to bindtexture\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

void create_js_functions()
{
    v7_set_method(v7g, v7_get_global(v7g), "createrendertarget", &js_create_rendertarget);
    v7_set_method(v7g, v7_get_global(v7g), "clear", &js_clear);
    v7_set_method(v7g, v7_get_global(v7g), "beginpass", &js_beginPass);
    v7_set_method(v7g, v7_get_global(v7g), "endpass", &js_endPass);
    v7_set_method(v7g, v7_get_global(v7g), "loadimage", &js_load_image);
    v7_set_method(v7g, v7_get_global(v7g), "loadmesh", &js_load_mesh);
    v7_set_method(v7g, v7_get_global(v7g), "loadshader", &js_load_shader);
    v7_set_method(v7g, v7_get_global(v7g), "drawmesh", &js_draw_mesh);
    v7_set_method(v7g, v7_get_global(v7g), "bindattribute", &js_bind_attrib);
    v7_set_method(v7g, v7_get_global(v7g), "resetattributes", &js_reset_attribs);
    v7_set_method(v7g, v7_get_global(v7g), "bindshader", &js_bind_shader);
    v7_set_method(v7g, v7_get_global(v7g), "bindtexture", &js_bind_texture);
    v7_set_method(v7g, v7_get_global(v7g), "setuniformf", &js_set_uniformf);
    v7_set_method(v7g, v7_get_global(v7g), "setuniformi", &js_set_uniformi);
    v7_set_method(v7g, v7_get_global(v7g), "setuniformui", &js_set_uniformui);
    v7_set_method(v7g, v7_get_global(v7g), "setuniformmat4", &js_set_uniform_matrix);
    v7_set_method(v7g, v7_get_global(v7g), "depthtest", &js_set_depth);
    v7_set_method(v7g, v7_get_global(v7g), "culling", &js_set_cullface);
    v7_set_method(v7g, v7_get_global(v7g), "vec3dot", &js_vec3_dot);
    v7_set_method(v7g, v7_get_global(v7g), "vec3normalize", &js_vec3_normalize);
    v7_set_method(v7g, v7_get_global(v7g), "vec3cross", &js_vec3_cross);
    v7_set_method(v7g, v7_get_global(v7g), "vec3add", &js_vec3_add);
    v7_set_method(v7g, v7_get_global(v7g), "vec3sub", &js_vec3_sub);
    v7_set_method(v7g, v7_get_global(v7g), "vec3mul", &js_vec3_mul);
    v7_set_method(v7g, v7_get_global(v7g), "mat4loadidentity", &js_mat4_loadidentity);
    v7_set_method(v7g, v7_get_global(v7g), "mat4setscale", &js_mat4_setscale);
    v7_set_method(v7g, v7_get_global(v7g), "mat4settranslation", &js_mat4_settranslation);
    v7_set_method(v7g, v7_get_global(v7g), "mat4setrotation", &js_mat4_setrotation);
    v7_set_method(v7g, v7_get_global(v7g), "mat4setperspective", &js_mat4_setperspective);
    v7_set_method(v7g, v7_get_global(v7g), "mat4mul", &js_mat4_mul);
}
void create_js_defines()
{
    /* gl data types */
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_BYTE", 16, v7_mk_number(v7g, GL_UNSIGNED_BYTE));
    v7_set(v7g, v7_get_global(v7g), "GL_BYTE", 7, v7_mk_number(v7g, GL_BYTE));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT", 17, v7_mk_number(v7g, GL_UNSIGNED_SHORT));
    v7_set(v7g, v7_get_global(v7g), "GL_SHORT", 8, v7_mk_number(v7g, GL_SHORT));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT", 15, v7_mk_number(v7g, GL_UNSIGNED_INT));
    v7_set(v7g, v7_get_global(v7g), "GL_INT", 6, v7_mk_number(v7g, GL_INT));
    v7_set(v7g, v7_get_global(v7g), "GL_FLOAT", 8, v7_mk_number(v7g, GL_FLOAT));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_BYTE_3_3_2", 22, v7_mk_number(v7g, GL_UNSIGNED_BYTE_3_3_2));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_BYTE_2_3_3_REV", 26, v7_mk_number(v7g, GL_UNSIGNED_BYTE_2_3_3_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_5_6_5", 23, v7_mk_number(v7g, GL_UNSIGNED_SHORT_5_6_5));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_5_6_5_REV", 27, v7_mk_number(v7g, GL_UNSIGNED_SHORT_5_6_5_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_4_4_4_4", 25, v7_mk_number(v7g, GL_UNSIGNED_SHORT_4_4_4_4));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_4_4_4_4_REV", 29, v7_mk_number(v7g, GL_UNSIGNED_SHORT_4_4_4_4_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_5_5_5_1", 25, v7_mk_number(v7g, GL_UNSIGNED_SHORT_5_5_5_1));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_1_5_5_5_REV", 29, v7_mk_number(v7g, GL_UNSIGNED_SHORT_1_5_5_5_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT_8_8_8_8", 23, v7_mk_number(v7g, GL_UNSIGNED_INT_8_8_8_8));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT_8_8_8_8_REV", 27, v7_mk_number(v7g, GL_UNSIGNED_INT_8_8_8_8_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT_10_10_10_2", 26, v7_mk_number(v7g, GL_UNSIGNED_INT_10_10_10_2));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT_2_10_10_10_REV", 30, v7_mk_number(v7g, GL_UNSIGNED_INT_2_10_10_10_REV));
    /* gl formats */
    v7_set(v7g, v7_get_global(v7g), "GL_RED", 6, v7_mk_number(v7g, GL_RED));
    v7_set(v7g, v7_get_global(v7g), "GL_RG", 5, v7_mk_number(v7g, GL_RG));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB", 6, v7_mk_number(v7g, GL_RGB));
    v7_set(v7g, v7_get_global(v7g), "GL_BGR", 6, v7_mk_number(v7g, GL_BGR));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA", 7, v7_mk_number(v7g, GL_RGBA));
    v7_set(v7g, v7_get_global(v7g), "GL_BGRA", 7, v7_mk_number(v7g, GL_BGRA));
    v7_set(v7g, v7_get_global(v7g), "GL_RED_INTEGER", 14, v7_mk_number(v7g, GL_RED_INTEGER));
    v7_set(v7g, v7_get_global(v7g), "GL_RG_INTEGER", 13, v7_mk_number(v7g, GL_RG_INTEGER));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB_INTEGER", 14, v7_mk_number(v7g, GL_RGB_INTEGER));
    v7_set(v7g, v7_get_global(v7g), "GL_BGR_INTEGER", 14, v7_mk_number(v7g, GL_BGR_INTEGER));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA_INTEGER", 15, v7_mk_number(v7g, GL_RGBA_INTEGER));
    v7_set(v7g, v7_get_global(v7g), "GL_BGRA_INTEGER", 15, v7_mk_number(v7g, GL_BGRA_INTEGER));
    v7_set(v7g, v7_get_global(v7g), "GL_STENCIL_INDEX", 16, v7_mk_number(v7g, GL_STENCIL_INDEX));
    v7_set(v7g, v7_get_global(v7g), "GL_DEPTH_COMPONENT", 18, v7_mk_number(v7g, GL_DEPTH_COMPONENT));
    v7_set(v7g, v7_get_global(v7g), "GL_DEPTH_STENCIL", 16, v7_mk_number(v7g, GL_DEPTH_STENCIL));
    /* texture filters */
    v7_set(v7g, v7_get_global(v7g), "GL_NEAREST", 10, v7_mk_number(v7g, GL_NEAREST));
    v7_set(v7g, v7_get_global(v7g), "GL_LINEAR", 9, v7_mk_number(v7g, GL_LINEAR));
    v7_set(v7g, v7_get_global(v7g), "GL_NEAREST_MIPMAP_NEAREST", 25, v7_mk_number(v7g, GL_NEAREST_MIPMAP_NEAREST));
    v7_set(v7g, v7_get_global(v7g), "GL_LINEAR_MIPMAP_NEAREST", 24, v7_mk_number(v7g, GL_LINEAR_MIPMAP_NEAREST));
    v7_set(v7g, v7_get_global(v7g), "GL_NEAREST_MIPMAP_LINEAR", 24, v7_mk_number(v7g, GL_NEAREST_MIPMAP_LINEAR));
    v7_set(v7g, v7_get_global(v7g), "GL_LINEAR_MIPMAP_LINEAR", 23, v7_mk_number(v7g, GL_LINEAR_MIPMAP_LINEAR));
    /* mesh flags */
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_POSITION", 18, v7_mk_number(v7g, MESH_FLAG_POSITION));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_NORMAL", 16, v7_mk_number(v7g, MESH_FLAG_NORMAL));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TANGENT", 17, v7_mk_number(v7g, MESH_FLAG_TANGENT));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_BINORMAL", 18, v7_mk_number(v7g, MESH_FLAG_BINORMAL));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TEXCOORD0", 19, v7_mk_number(v7g, MESH_FLAG_TEXCOORD0));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TEXCOORD1", 19, v7_mk_number(v7g, MESH_FLAG_TEXCOORD1));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TEXCOORD2", 19, v7_mk_number(v7g, MESH_FLAG_TEXCOORD2));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TEXCOORD3", 19, v7_mk_number(v7g, MESH_FLAG_TEXCOORD3));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TEXCOORD4", 19, v7_mk_number(v7g, MESH_FLAG_TEXCOORD4));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TEXCOORD5", 19, v7_mk_number(v7g, MESH_FLAG_TEXCOORD5));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TEXCOORD6", 19, v7_mk_number(v7g, MESH_FLAG_TEXCOORD6));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_TEXCOORD7", 19, v7_mk_number(v7g, MESH_FLAG_TEXCOORD7));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_COLOR0", 16, v7_mk_number(v7g, MESH_FLAG_COLOR0));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_COLOR1", 16, v7_mk_number(v7g, MESH_FLAG_COLOR1));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_COLOR2", 16, v7_mk_number(v7g, MESH_FLAG_COLOR2));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_COLOR3", 16, v7_mk_number(v7g, MESH_FLAG_COLOR3));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_COLOR4", 16, v7_mk_number(v7g, MESH_FLAG_COLOR4));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_COLOR5", 16, v7_mk_number(v7g, MESH_FLAG_COLOR5));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_COLOR6", 16, v7_mk_number(v7g, MESH_FLAG_COLOR6));
    v7_set(v7g, v7_get_global(v7g), "MESH_FLAG_COLOR7", 16, v7_mk_number(v7g, MESH_FLAG_COLOR7));
    v7_set(v7g, v7_get_global(v7g), "CULL_FRONT", 10, v7_mk_number(v7g, CULL_FRONT));
    v7_set(v7g, v7_get_global(v7g), "CULL_BACK", 9, v7_mk_number(v7g, CULL_BACK));
    v7_set(v7g, v7_get_global(v7g), "CULL_NONE", 9, v7_mk_number(v7g, CULL_NONE));
    v7_set(v7g, v7_get_global(v7g), "CULL_BOTH", 9, v7_mk_number(v7g, CULL_BOTH));
}
int initScript(const char* filename)
{
    v7g = v7_create();
    create_js_functions();
    create_js_defines();
    v7_val_t result;
    enum v7_err rcode = v7_exec_file(v7g, filename, &result);

    if(rcode != V7_OK)
    {
        v7_print_error(stderr, v7g, "Error", result);
        validscript = 0;
        return 0;
    }

    validscript = 1;
    run_loop();
    return 1;
}
extern double deltaTime;
extern double currenttime;
void run_loop()
{
    v7_set(v7g, v7_get_global(v7g), "TIME", 4, v7_mk_number(v7g, currenttime));
    v7_set(v7g, v7_get_global(v7g), "DELTA_TIME", 10, v7_mk_number(v7g, deltaTime));
    v7_val_t function;
    v7_val_t result;
    function = v7_get(v7g, v7_get_global(v7g), "loop", 4);
    enum v7_err rcode = v7_apply(v7g, function, V7_UNDEFINED, V7_UNDEFINED, &result);

    if(rcode != V7_OK)
    {
        v7_print_error(stderr, v7g, "Error", result);
        validscript = 0;
    }
}
int shutdownScript()
{
    v7_destroy(v7g);
    cleanupRender();
    return 1;
}
void reloadScript(const char* filename)
{
    printf("reloading script: %s\n", filename);
    v7_destroy(v7g);
    cleanupRender();
    initScript(filename);
}
