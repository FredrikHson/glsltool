#include "script.h"
#include <stdio.h>
#include "opengl.h"
#include "v7.h"
#include "renderfunc.h"
#include "resources.h"

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
            fprintf(stderr, "first argument to loadImage should be a string\n");
            return V7_SYNTAX_ERROR;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadImage\n");
        return V7_SYNTAX_ERROR;
    }
}

void create_js_functions()
{
    v7_set_method(v7g, v7_get_global(v7g), "createrendertarget", &js_create_rendertarget);
    v7_set_method(v7g, v7_get_global(v7g), "clear", &js_clear);
    v7_set_method(v7g, v7_get_global(v7g), "beginpass", &js_beginPass);
    v7_set_method(v7g, v7_get_global(v7g), "endpass", &js_endPass);
    v7_set_method(v7g, v7_get_global(v7g), "loadimage", &js_load_image);
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
