#include "script.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "v7.h"
#include "renderfunc.h"

unsigned char validscript = 0;
struct v7* v7g = 0;

static enum v7_err js_create_rendertarget(struct v7* v7, v7_val_t* res)
{
    if(v7_argc(v7) < 5)
    {
        printf("invalid number of arguments to create_rendertarget\n");
    }

    int width  = v7_get_int(v7, v7_arg(v7, 0));
    int height = v7_get_int(v7, v7_arg(v7, 1));
    int layers = v7_get_int(v7, v7_arg(v7, 2));
    int colors = v7_get_int(v7, v7_arg(v7, 3));
    int type   = v7_get_int(v7, v7_arg(v7, 4));
    int magfilter = GL_NEAREST;
    int minfilter = GL_NEAREST;

    if(v7_argc(v7) > 5)
    {
        magfilter = v7_get_int(v7, v7_arg(v7, 6));
    }

    if(v7_argc(v7) > 6)
    {
        minfilter = v7_get_int(v7, v7_arg(v7, 7));
    }



    unsigned int target = CreateRenderTarget(width, height, layers, colors, type, minfilter, magfilter);

    *res = v7_mk_number(v7, target);
    return V7_OK;
}


void create_js_functions()
{
    v7_set_method(v7g, v7_get_global(v7g), "create_rendertarget", &js_create_rendertarget);
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

    }

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

    }
}

int shutdownScript()
{
    v7_destroy(v7g);
    cleanupRender();

    return 1;
}
