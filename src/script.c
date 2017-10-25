#include "script.h"
#include <stdio.h>
#include "opengl.h"
#include "v7.h"
#include "renderfunc.h"
#include "resources.h"
#include "defines.h"

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
    if(v7_argc(v7e) == 0)
    {
        beginPass(-1);
    }
    else
    {
        beginPass(v7_get_int(v7e, v7_arg(v7e, 0)));
    }

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
                return V7_INTERNAL_ERROR;
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
                return V7_INTERNAL_ERROR;
            }

            printf("meshid %i\n", mesh);
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
            return V7_INTERNAL_ERROR;
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
        fprintf(stderr, "invalid number of arguments to bindshader\n");
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
        fprintf(stderr, "invalid number of arguments to bindshader\n");
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
        fprintf(stderr, "invalid number of arguments to bindshader\n");
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
    v7_set_method(v7g, v7_get_global(v7g), "setuniformf", &js_set_uniformf);
    v7_set_method(v7g, v7_get_global(v7g), "setuniformi", &js_set_uniformi);
    v7_set_method(v7g, v7_get_global(v7g), "setuniformui", &js_set_uniformui);
    v7_set_method(v7g, v7_get_global(v7g), "depthtest", &js_set_depth);
    v7_set_method(v7g, v7_get_global(v7g), "culling", &js_set_cullface);
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
    }

    validscript = 1;
    run_loop();
    return 1;
}

void run_loop()
{
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
