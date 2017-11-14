#include "script.h"
#include <stdio.h>
#include <math.h>
#include "opengl.h"
#include "v7.h"
#include "renderfunc.h"
#include "defines.h"
#include "scriptfunc.h"

typedef struct v7 v7;
unsigned char validscript = 0;
v7* v7g = 0;

void create_js_functions()
{
    v7_val_t global = v7_get_global(v7g);
    v7_set_method(v7g, global, "createrendertarget", &js_create_rendertarget);
    v7_set_method(v7g, global, "clear", &js_clear);
    v7_set_method(v7g, global, "cleardepth", &js_cleardepth);
    v7_set_method(v7g, global, "clearcolor", &js_clearcolor);
    v7_set_method(v7g, global, "beginpass", &js_beginPass);
    v7_set_method(v7g, global, "endpass", &js_endPass);
    v7_set_method(v7g, global, "loadimage", &js_load_image);
    v7_set_method(v7g, global, "loadmesh", &js_load_mesh);
    v7_set_method(v7g, global, "loadshader", &js_load_shader);
    v7_set_method(v7g, global, "drawmesh", &js_draw_mesh);
    v7_set_method(v7g, global, "bindattribute", &js_bind_attrib);
    v7_set_method(v7g, global, "bindshader", &js_bind_shader);
    v7_set_method(v7g, global, "bindtexture", &js_bind_texture);
    v7_set_method(v7g, global, "bindrendertarget", &js_bind_rendertarget);
    v7_set_method(v7g, global, "setdefaultfilter", &js_set_default_filter);
    v7_set_method(v7g, global, "setuniformf", &js_set_uniformf);
    v7_set_method(v7g, global, "setuniformi", &js_set_uniformi);
    v7_set_method(v7g, global, "setuniformui", &js_set_uniformui);
    v7_set_method(v7g, global, "setuniformmat4", &js_set_uniform_matrix);
    v7_set_method(v7g, global, "depthtest", &js_set_depth);
    v7_set_method(v7g, global, "culling", &js_set_cullface);
    v7_set_method(v7g, global, "vec3dot", &js_vec3_dot);
    v7_set_method(v7g, global, "vec3normalize", &js_vec3_normalize);
    v7_set_method(v7g, global, "vec3cross", &js_vec3_cross);
    v7_set_method(v7g, global, "vec3add", &js_vec3_add);
    v7_set_method(v7g, global, "vec3sub", &js_vec3_sub);
    v7_set_method(v7g, global, "vec3mul", &js_vec3_mul);
    v7_set_method(v7g, global, "mat4loadidentity", &js_mat4_loadidentity);
    v7_set_method(v7g, global, "mat4setscale", &js_mat4_setscale);
    v7_set_method(v7g, global, "mat4settranslation", &js_mat4_settranslation);
    v7_set_method(v7g, global, "mat4setrotation", &js_mat4_setrotation);
    v7_set_method(v7g, global, "mat4setperspective", &js_mat4_setperspective);
    v7_set_method(v7g, global, "mat4mul", &js_mat4_mul);
    v7_set_method(v7g, global, "vec3mat4mul", &js_vec3_mat4_mul);
    v7_set_method(v7g, global, "sin", &js_sin);
    v7_set_method(v7g, global, "cos", js_cos);
    v7_set_method(v7g, global, "tan", js_tan);
    v7_set_method(v7g, global, "asin", js_asin);
    v7_set_method(v7g, global, "acos", js_acos);
    v7_set_method(v7g, global, "atan", js_atan);
    v7_set_method(v7g, global, "floor", js_floor);
    v7_set_method(v7g, global, "ceil", js_ceil);
    v7_set_method(v7g, global, "abs", js_abs);
    v7_set_method(v7g, global, "sqrt", js_sqrt);
    v7_set_method(v7g, global, "setmat4anglemode", js_set_angle_mode);
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
    v7_set(v7g, v7_get_global(v7g), "GL_CLAMP_TO_EDGE", 16, v7_mk_number(v7g, GL_CLAMP_TO_EDGE));
    v7_set(v7g, v7_get_global(v7g), "GL_MIRRORED_REPEAT", 18, v7_mk_number(v7g, GL_MIRRORED_REPEAT));
    v7_set(v7g, v7_get_global(v7g), "GL_REPEAT", 9, v7_mk_number(v7g, GL_REPEAT));
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
    /* render flags */
    v7_set(v7g, v7_get_global(v7g), "CULL_FRONT", 10, v7_mk_number(v7g, CULL_FRONT));
    v7_set(v7g, v7_get_global(v7g), "CULL_BACK", 9, v7_mk_number(v7g, CULL_BACK));
    v7_set(v7g, v7_get_global(v7g), "CULL_NONE", 9, v7_mk_number(v7g, CULL_NONE));
    v7_set(v7g, v7_get_global(v7g), "CULL_BOTH", 9, v7_mk_number(v7g, CULL_BOTH));
    /* input flags*/
    v7_set(v7g, v7_get_global(v7g), "PRESSED", 7, v7_mk_number(v7g, PRESSED));
    v7_set(v7g, v7_get_global(v7g), "PRESSED_NOW", 11, v7_mk_number(v7g, PRESSED_NOW));
    /* math flags */
    v7_set(v7g, v7_get_global(v7g), "RADIANS", 7, v7_mk_number(v7g, RADIANS));
    v7_set(v7g, v7_get_global(v7g), "DEGREES", 7, v7_mk_number(v7g, DEGREES));
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
    updateinput();
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
