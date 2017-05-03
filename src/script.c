#include "script.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "v7.h"

unsigned char validscript = 0;
struct v7* v7g = 0;

static enum v7_err js_create_rendertarget(struct v7* v7, v7_val_t* res)
{
    int width  = v7_get_int(v7, v7_arg(v7, 0));
    int height = v7_get_int(v7, v7_arg(v7, 1));
    int colors = v7_get_int(v7, v7_arg(v7, 2));
    int type   = v7_get_int(v7, v7_arg(v7, 3));

    printf("creating a buffer:\n");
    printf("    width:  %i\n", width);
    printf("    height: %i\n", height);
    printf("    colors: %i\n", colors);
    printf("    type:   %i\n", type);
    *res = v7_mk_number(v7, 0);
    return V7_OK;
}


void create_js_functions()
{
    v7_set_method(v7g, v7_get_global(v7g), "create_rendertarget", &js_create_rendertarget);
}

void create_js_defines()
{
    v7_set(v7g, v7_get_global(v7g), "RGB", 3, v7_mk_number(v7g, 3));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_BYTE",16 , v7_mk_number(v7g, GL_UNSIGNED_BYTE));
    v7_set(v7g, v7_get_global(v7g), "GL_BYTE",7 , v7_mk_number(v7g, GL_BYTE));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT",17 , v7_mk_number(v7g, GL_UNSIGNED_SHORT));
    v7_set(v7g, v7_get_global(v7g), "GL_SHORT",8 , v7_mk_number(v7g, GL_SHORT));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT",15 , v7_mk_number(v7g, GL_UNSIGNED_INT));
    v7_set(v7g, v7_get_global(v7g), "GL_INT",6 , v7_mk_number(v7g, GL_INT));
    v7_set(v7g, v7_get_global(v7g), "GL_FLOAT",8 , v7_mk_number(v7g, GL_FLOAT));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_BYTE_3_3_2",22 , v7_mk_number(v7g, GL_UNSIGNED_BYTE_3_3_2));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_BYTE_2_3_3_REV",26 , v7_mk_number(v7g, GL_UNSIGNED_BYTE_2_3_3_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_5_6_5",23 , v7_mk_number(v7g, GL_UNSIGNED_SHORT_5_6_5));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_5_6_5_REV",27 , v7_mk_number(v7g, GL_UNSIGNED_SHORT_5_6_5_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_4_4_4_4",25 , v7_mk_number(v7g, GL_UNSIGNED_SHORT_4_4_4_4));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_4_4_4_4_REV",29 , v7_mk_number(v7g, GL_UNSIGNED_SHORT_4_4_4_4_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_5_5_5_1",25 , v7_mk_number(v7g, GL_UNSIGNED_SHORT_5_5_5_1));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_SHORT_1_5_5_5_REV",29 , v7_mk_number(v7g, GL_UNSIGNED_SHORT_1_5_5_5_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT_8_8_8_8",23 , v7_mk_number(v7g, GL_UNSIGNED_INT_8_8_8_8));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT_8_8_8_8_REV",27 , v7_mk_number(v7g, GL_UNSIGNED_INT_8_8_8_8_REV));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT_10_10_10_2",26 , v7_mk_number(v7g, GL_UNSIGNED_INT_10_10_10_2));
    v7_set(v7g, v7_get_global(v7g), "GL_UNSIGNED_INT_2_10_10_10_REV",30 , v7_mk_number(v7g, GL_UNSIGNED_INT_2_10_10_10_REV));
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

    return 1;
}
