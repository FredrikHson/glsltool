#include "defines.h"
#include "opengl.h"
#include "options.h"
#include "renderfunc.h"
#include "script.h"
#include "scriptfunc.h"
#include "resources.h"
#include "debug.h"
#include "v7.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

typedef struct v7 v7;
char* entrypointfile = 0;
unsigned char validscript = 0;
v7* v7g = 0;

char** globals = 0;
size_t numglobals = 0;
extern double deltaTime;
extern double currenttime;
extern int should_quit;

void setsighandler(int enable);
extern sigjmp_buf before;

enum v7_err override_exit(v7* v7e, v7_val_t* res)
{
    should_quit = 1;
    return V7_OK;
}

enum v7_err js_register_global(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* name = v7_get_string(v7e, &val, &len);

        for(int i = 0; i < numglobals; i++)
        {
            if(strcmp(name, globals[i]) == 0)
            {
                return V7_OK;
            }
        }

        if(numglobals == 0)
        {
            globals = malloc(sizeof(char*));
        }
        else
        {
            void* newglobals = realloc(globals, sizeof(char*) * (numglobals + 1));

            if(newglobals == 0)
            {
                fprintf(stderr, "out of memory while registering a global%s\n", name);
                exit(1);
            }
            else
            {
                globals = newglobals;
            }
        }

        globals[numglobals] = strdup(name);
        numglobals++;
        printf("registered: %s\n", name);
    }

    return V7_OK;
}

void create_js_functions()
{
    v7_val_t global = v7_get_global(v7g);
    v7_set_method(v7g, global, "registerglobal", &js_register_global);
    v7_set_method(v7g, global, "createrendertarget", &js_create_rendertarget);
    v7_set_method(v7g, global, "clear", &js_clear);
    v7_set_method(v7g, global, "wireframe", &js_wireframe);
    v7_set_method(v7g, global, "cleardepth", &js_cleardepth);
    v7_set_method(v7g, global, "clearcolor", &js_clearcolor);
    v7_set_method(v7g, global, "beginpass", &js_beginPass);
    v7_set_method(v7g, global, "endpass", &js_endPass);
    v7_set_method(v7g, global, "loadimage", &js_load_image);
    v7_set_method(v7g, global, "loadmesh", &js_load_mesh);
    v7_set_method(v7g, global, "destroymesh", &js_destroy_mesh);
    v7_set_method(v7g, global, "ismesh", &js_ismesh);
    v7_set_method(v7g, global, "loadshader", &js_load_shader);
    v7_set_method(v7g, global, "generateplane", &js_generate_plane);
    v7_set_method(v7g, global, "generatemesh", &js_generate_generic_mesh);
    v7_set_method(v7g, global, "openmesh", &js_open_mesh);
    v7_set_method(v7g, global, "setindices", &js_set_indices);
    v7_set_method(v7g, global, "setvertdata", &js_set_vert_data);
    v7_set_method(v7g, global, "drawmesh", &js_draw_mesh);
    v7_set_method(v7g, global, "bindattribute", &js_bind_attrib);
    v7_set_method(v7g, global, "bindshader", &js_bind_shader);
    v7_set_method(v7g, global, "bindtexture", &js_bind_texture);
    v7_set_method(v7g, global, "bindrendertarget", &js_bind_rendertarget);
    v7_set_method(v7g, global, "bindfragmentdata", &js_bind_fragment_data);
    v7_set_method(v7g, global, "setdefaultfilter", &js_set_default_filter);
    v7_set_method(v7g, global, "setdefaultclamp", &js_set_default_clamp);
    v7_set_method(v7g, global, "setuniformf", &js_set_uniformf);
    v7_set_method(v7g, global, "setuniformi", &js_set_uniformi);
    v7_set_method(v7g, global, "setuniformui", &js_set_uniformui);
    v7_set_method(v7g, global, "setuniformmat4", &js_set_uniform_matrix);
    v7_set_method(v7g, global, "setuniformmat4d", &js_set_uniform_matrix_d);
    v7_set_method(v7g, global, "depthtest", &js_set_depth);
    v7_set_method(v7g, global, "culling", &js_set_cullface);
    v7_set_method(v7g, global, "blendfunc", &js_blend_func);
    v7_set_method(v7g, global, "blendfuncseparate", &js_blend_func_separate);
    v7_set_method(v7g, global, "blendequation", &js_blend_equation);
    v7_set_method(v7g, global, "blend", &js_blend);
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
    v7_set_method(v7g, global, "mat4transpose", &js_mat4_transpose);
    v7_set_method(v7g, global, "mat4invert", &js_mat4_invert);
    v7_set_method(v7g, global, "vec3mat4mul", &js_vec3_mat4_mul);
    v7_set_method(v7g, global, "vec4mat4mul", &js_vec4_mat4_mul);
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
    v7_set_method(v7g, global, "debugmode", js_set_debug_mode);
    v7_set_method(v7g, global, "debugstep", js_set_debug_step);
    v7_set_method(v7g, global, "debugrange", js_set_debug_range);
    v7_set_method(v7g, global, "debugclip", js_set_debug_clip);
    v7_set_method(v7g, global, "imagewidth", js_image_width);
    v7_set_method(v7g, global, "imageheight", js_image_height);
    v7_set_method(v7g, global, "getoptionalstring", js_get_optional_string);
    v7_set_method(v7g, global, "getmeshbbox", js_get_mesh_bbox);
    v7_set_method(v7g, global, "setwindowtitle", js_set_window_title);
    v7_set_method(v7g, global, "setclipboard", js_set_clipboard);
    v7_set_method(v7g, global, "getclipboard", js_get_clipboard);
    v7_set_method(v7g, global, "watchfile", js_watchfile);
    v7_set_method(v7g, global, "getsquarejitterx", js_get_square_jitter_x);
    v7_set_method(v7g, global, "getsquarejittery", js_get_square_jitter_y);
    v7_set_method(v7g, global, "getcirclejitterx", js_get_circle_jitter_x);
    v7_set_method(v7g, global, "getcirclejittery", js_get_circle_jitter_y);
    v7_set_method(v7g, global, "exit", override_exit);
}

/* for easy writing of v7_set global
'<,'>s/\(.*\)/\="v7_set(v7g, v7_get_global(v7g),\"" . submatch(0) . "\"," . strlen(submatch(0)) . ", v7_mk_number(v7g,". submatch(0) ."));"/g
*/
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
    v7_set(v7g, v7_get_global(v7g), "GL_R8", 5, v7_mk_number(v7g, GL_R8));
    v7_set(v7g, v7_get_global(v7g), "GL_R8_SNORM", 11, v7_mk_number(v7g, GL_R8_SNORM));
    v7_set(v7g, v7_get_global(v7g), "GL_R16", 6, v7_mk_number(v7g, GL_R16));
    v7_set(v7g, v7_get_global(v7g), "GL_R16_SNORM", 12, v7_mk_number(v7g, GL_R16_SNORM));
    v7_set(v7g, v7_get_global(v7g), "GL_RG8", 6, v7_mk_number(v7g, GL_RG8));
    v7_set(v7g, v7_get_global(v7g), "GL_RG8_SNORM", 12, v7_mk_number(v7g, GL_RG8_SNORM));
    v7_set(v7g, v7_get_global(v7g), "GL_RG16", 7, v7_mk_number(v7g, GL_RG16));
    v7_set(v7g, v7_get_global(v7g), "GL_RG16_SNORM", 13, v7_mk_number(v7g, GL_RG16_SNORM));
    v7_set(v7g, v7_get_global(v7g), "GL_R3_G3_B2", 11, v7_mk_number(v7g, GL_R3_G3_B2));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB4", 7, v7_mk_number(v7g, GL_RGB4));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB5", 7, v7_mk_number(v7g, GL_RGB5));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB8", 7, v7_mk_number(v7g, GL_RGB8));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB8_SNORM", 13, v7_mk_number(v7g, GL_RGB8_SNORM));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB10", 8, v7_mk_number(v7g, GL_RGB10));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB12", 8, v7_mk_number(v7g, GL_RGB12));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB16_SNORM", 14, v7_mk_number(v7g, GL_RGB16_SNORM));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA2", 8, v7_mk_number(v7g, GL_RGBA2));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA4", 8, v7_mk_number(v7g, GL_RGBA4));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB5_A1", 10, v7_mk_number(v7g, GL_RGB5_A1));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA8", 8, v7_mk_number(v7g, GL_RGBA8));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA8_SNORM", 14, v7_mk_number(v7g, GL_RGBA8_SNORM));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB10_A2", 11, v7_mk_number(v7g, GL_RGB10_A2));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB10_A2UI", 13, v7_mk_number(v7g, GL_RGB10_A2UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA12", 9, v7_mk_number(v7g, GL_RGBA12));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA16", 9, v7_mk_number(v7g, GL_RGBA16));
    v7_set(v7g, v7_get_global(v7g), "GL_SRGB8", 8, v7_mk_number(v7g, GL_SRGB8));
    v7_set(v7g, v7_get_global(v7g), "GL_SRGB8_ALPHA8", 15, v7_mk_number(v7g, GL_SRGB8_ALPHA8));
    v7_set(v7g, v7_get_global(v7g), "GL_R16F", 7, v7_mk_number(v7g, GL_R16F));
    v7_set(v7g, v7_get_global(v7g), "GL_RG16F", 8, v7_mk_number(v7g, GL_RG16F));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB16F", 9, v7_mk_number(v7g, GL_RGB16F));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA16F", 10, v7_mk_number(v7g, GL_RGBA16F));
    v7_set(v7g, v7_get_global(v7g), "GL_R32F", 7, v7_mk_number(v7g, GL_R32F));
    v7_set(v7g, v7_get_global(v7g), "GL_RG32F", 8, v7_mk_number(v7g, GL_RG32F));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB32F", 9, v7_mk_number(v7g, GL_RGB32F));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA32F", 10, v7_mk_number(v7g, GL_RGBA32F));
    v7_set(v7g, v7_get_global(v7g), "GL_R11F_G11F_B10F", 17, v7_mk_number(v7g, GL_R11F_G11F_B10F));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB9_E5", 10, v7_mk_number(v7g, GL_RGB9_E5));
    v7_set(v7g, v7_get_global(v7g), "GL_R8I", 6, v7_mk_number(v7g, GL_R8I));
    v7_set(v7g, v7_get_global(v7g), "GL_R8UI", 7, v7_mk_number(v7g, GL_R8UI));
    v7_set(v7g, v7_get_global(v7g), "GL_R16I", 7, v7_mk_number(v7g, GL_R16I));
    v7_set(v7g, v7_get_global(v7g), "GL_R16UI", 8, v7_mk_number(v7g, GL_R16UI));
    v7_set(v7g, v7_get_global(v7g), "GL_R32I", 7, v7_mk_number(v7g, GL_R32I));
    v7_set(v7g, v7_get_global(v7g), "GL_R32UI", 8, v7_mk_number(v7g, GL_R32UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RG8I", 7, v7_mk_number(v7g, GL_RG8I));
    v7_set(v7g, v7_get_global(v7g), "GL_RG8UI", 8, v7_mk_number(v7g, GL_RG8UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RG16I", 8, v7_mk_number(v7g, GL_RG16I));
    v7_set(v7g, v7_get_global(v7g), "GL_RG16UI", 9, v7_mk_number(v7g, GL_RG16UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RG32I", 8, v7_mk_number(v7g, GL_RG32I));
    v7_set(v7g, v7_get_global(v7g), "GL_RG32UI", 9, v7_mk_number(v7g, GL_RG32UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB8I", 8, v7_mk_number(v7g, GL_RGB8I));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB8UI", 9, v7_mk_number(v7g, GL_RGB8UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB16I", 9, v7_mk_number(v7g, GL_RGB16I));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB16UI", 10, v7_mk_number(v7g, GL_RGB16UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB32I", 9, v7_mk_number(v7g, GL_RGB32I));
    v7_set(v7g, v7_get_global(v7g), "GL_RGB32UI", 10, v7_mk_number(v7g, GL_RGB32UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA8I", 9, v7_mk_number(v7g, GL_RGBA8I));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA8UI", 10, v7_mk_number(v7g, GL_RGBA8UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA16I", 10, v7_mk_number(v7g, GL_RGBA16I));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA16UI", 11, v7_mk_number(v7g, GL_RGBA16UI));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA32I", 10, v7_mk_number(v7g, GL_RGBA32I));
    v7_set(v7g, v7_get_global(v7g), "GL_RGBA32UI", 11, v7_mk_number(v7g, GL_RGBA32UI));
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
    /* blending */
    v7_set(v7g, v7_get_global(v7g), "GL_FUNC_ADD", 11, v7_mk_number(v7g, GL_FUNC_ADD));
    v7_set(v7g, v7_get_global(v7g), "GL_FUNC_SUBTRACT", 16, v7_mk_number(v7g, GL_FUNC_SUBTRACT));
    v7_set(v7g, v7_get_global(v7g), "GL_FUNC_REVERSE_SUBTRACT", 24, v7_mk_number(v7g, GL_FUNC_REVERSE_SUBTRACT));
    v7_set(v7g, v7_get_global(v7g), "GL_MIN", 6, v7_mk_number(v7g, GL_MIN));
    v7_set(v7g, v7_get_global(v7g), "GL_MAX", 6, v7_mk_number(v7g, GL_MAX));
    v7_set(v7g, v7_get_global(v7g), "GL_ZERO", 7, v7_mk_number(v7g, GL_ZERO));
    v7_set(v7g, v7_get_global(v7g), "GL_ONE", 6, v7_mk_number(v7g, GL_ONE));
    v7_set(v7g, v7_get_global(v7g), "GL_SRC_COLOR", 12, v7_mk_number(v7g, GL_SRC_COLOR));
    v7_set(v7g, v7_get_global(v7g), "GL_ONE_MINUS_SRC_COLOR", 22, v7_mk_number(v7g, GL_ONE_MINUS_SRC_COLOR));
    v7_set(v7g, v7_get_global(v7g), "GL_DST_COLOR", 12, v7_mk_number(v7g, GL_DST_COLOR));
    v7_set(v7g, v7_get_global(v7g), "GL_ONE_MINUS_DST_COLOR", 22, v7_mk_number(v7g, GL_ONE_MINUS_DST_COLOR));
    v7_set(v7g, v7_get_global(v7g), "GL_SRC_ALPHA", 12, v7_mk_number(v7g, GL_SRC_ALPHA));
    v7_set(v7g, v7_get_global(v7g), "GL_ONE_MINUS_SRC_ALPHA", 22, v7_mk_number(v7g, GL_ONE_MINUS_SRC_ALPHA));
    v7_set(v7g, v7_get_global(v7g), "GL_DST_ALPHA", 12, v7_mk_number(v7g, GL_DST_ALPHA));
    v7_set(v7g, v7_get_global(v7g), "GL_ONE_MINUS_DST_ALPHA", 22, v7_mk_number(v7g, GL_ONE_MINUS_DST_ALPHA));
    v7_set(v7g, v7_get_global(v7g), "GL_CONSTANT_COLOR", 17, v7_mk_number(v7g, GL_CONSTANT_COLOR));
    v7_set(v7g, v7_get_global(v7g), "GL_ONE_MINUS_CONSTANT_COLOR", 27, v7_mk_number(v7g, GL_ONE_MINUS_CONSTANT_COLOR));
    v7_set(v7g, v7_get_global(v7g), "GL_CONSTANT_ALPHA", 17, v7_mk_number(v7g, GL_CONSTANT_ALPHA));
    v7_set(v7g, v7_get_global(v7g), "GL_ONE_MINUS_CONSTANT_ALPHA", 27, v7_mk_number(v7g, GL_ONE_MINUS_CONSTANT_ALPHA));
    /* draw modes */
    v7_set(v7g, v7_get_global(v7g), "GL_POINTS", 9, v7_mk_number(v7g, GL_POINTS));
    v7_set(v7g, v7_get_global(v7g), "GL_LINE_STRIP", 13, v7_mk_number(v7g, GL_LINE_STRIP));
    v7_set(v7g, v7_get_global(v7g), "GL_LINE_LOOP", 12, v7_mk_number(v7g, GL_LINE_LOOP));
    v7_set(v7g, v7_get_global(v7g), "GL_LINES", 8, v7_mk_number(v7g, GL_LINES));
    v7_set(v7g, v7_get_global(v7g), "GL_LINE_STRIP_ADJACENCY", 23, v7_mk_number(v7g, GL_LINE_STRIP_ADJACENCY));
    v7_set(v7g, v7_get_global(v7g), "GL_LINES_ADJACENCY", 18, v7_mk_number(v7g, GL_LINES_ADJACENCY));
    v7_set(v7g, v7_get_global(v7g), "GL_TRIANGLE_STRIP", 17, v7_mk_number(v7g, GL_TRIANGLE_STRIP));
    v7_set(v7g, v7_get_global(v7g), "GL_TRIANGLE_FAN", 15, v7_mk_number(v7g, GL_TRIANGLE_FAN));
    v7_set(v7g, v7_get_global(v7g), "GL_TRIANGLES", 12, v7_mk_number(v7g, GL_TRIANGLES));
    v7_set(v7g, v7_get_global(v7g), "GL_TRIANGLE_STRIP_ADJACENCY", 27, v7_mk_number(v7g, GL_TRIANGLE_STRIP_ADJACENCY));
    v7_set(v7g, v7_get_global(v7g), "GL_TRIANGLES_ADJACENCY", 22, v7_mk_number(v7g, GL_TRIANGLES_ADJACENCY));
    v7_set(v7g, v7_get_global(v7g), "GL_PATCHES", 10, v7_mk_number(v7g, GL_PATCHES));
    /* texture flags */
    v7_set(v7g, v7_get_global(v7g), "IMG_TOP", 7, v7_mk_number(v7g, IMG_TOP));
    v7_set(v7g, v7_get_global(v7g), "IMG_BOTTOM", 10, v7_mk_number(v7g, IMG_BOTTOM));
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
    v7_set(v7g, v7_get_global(v7g), "TIME", 4, v7_mk_number(v7g, currenttime));
    v7_set(v7g, v7_get_global(v7g), "DELTA_TIME", 10, v7_mk_number(v7g, deltaTime));
    /* debug flags */
    v7_set(v7g, v7_get_global(v7g), "DEBUG_OFF", 9, v7_mk_number(v7g, DEBUG_OFF));
    v7_set(v7g, v7_get_global(v7g), "DEBUG_RENDERALLSTEPS", 20, v7_mk_number(v7g, DEBUG_RENDERALLSTEPS));
    v7_set(v7g, v7_get_global(v7g), "DEBUG_RENDERSINGLESTEP", 22, v7_mk_number(v7g, DEBUG_RENDERSINGLESTEP));
}

void override_require() // add watchfile on the require command
{
    if(v7_exec(v7g,
               "function require(m) { "
               "  if (m in _modcache) { return _modcache[m]; }"
               "  var module = {exports:{}};"
               "  File.eval(m);"
               "  watchfile(m);"
               "  return (_modcache[m] = module.exports)"
               " }",
               NULL) != V7_OK)
    {
    }
}

int initScript(const char* filename)
{
    if(entrypointfile == 0)
    {
        entrypointfile = malloc(strlen(filename) + 1);
        strcpy(entrypointfile, filename);
    }

    initResourceCleanup();
    v7g = v7_create();
    create_js_functions();
    create_js_defines();
    override_require();
    v7_val_t result;
    enum v7_err rcode = v7_exec_file(v7g, filename, &result);

    if(rcode != V7_OK)
    {
        v7_print_error(stderr, v7g, "Error", result);
        validscript = 0;
        return 0;
    }

    validscript = 1;
    static char firstrun = 1;

    if(firstrun)
    {
        firstrun = 0;
        run_loop();
    }

    endResourceCleanup();
    return 1;
}

void run_resize()
{
    v7_set(v7g, v7_get_global(v7g), "WINDOW_WIDTH", 12, v7_mk_number(v7g, options.width));
    v7_set(v7g, v7_get_global(v7g), "WINDOW_HEIGHT", 13, v7_mk_number(v7g, options.height));
    v7_val_t function;
    v7_val_t result;
    function = v7_get(v7g, v7_get_global(v7g), "resize", 6);

    if(v7_is_undefined(function) == 0)
    {
        enum v7_err rcode = v7_apply(v7g, function, V7_UNDEFINED, V7_UNDEFINED, &result);

        if(rcode != V7_OK)
        {
            v7_print_error(stderr, v7g, "Error", result);
            validscript = 0;
        }
    }
}

void run_filechange(char* filename)
{
    v7_val_t function;
    v7_val_t result;
    function = v7_get(v7g, v7_get_global(v7g), "filechange", 10);

    if(v7_is_undefined(function) == 0)
    {
        v7_val_t args = v7_mk_array(v7g);
        v7_array_push(v7g, args, v7_mk_string(v7g, filename, ~0, 1));
        enum v7_err rcode = v7_apply(v7g, function, V7_UNDEFINED, args, &result);

        if(rcode != V7_OK)
        {
            v7_print_error(stderr, v7g, "Error", result);
            validscript = 0;
        }
    }
}

void run_droppedfiles(const char** files, int count)
{
    v7_val_t function;
    v7_val_t result;
    function = v7_get(v7g, v7_get_global(v7g), "filedrop", 8);

    if(v7_is_undefined(function) == 0)
    {
        v7_val_t array = v7_mk_array(v7g);
        v7_val_t args = v7_mk_array(v7g);

        for(int i = 0; i < count; i++)
        {
            v7_array_push(v7g, array, v7_mk_string(v7g, files[i], ~0, 1));
        }

        v7_array_push(v7g, args, array);
        enum v7_err rcode = v7_apply(v7g, function, V7_UNDEFINED, args, &result);

        if(rcode != V7_OK)
        {
            v7_print_error(stderr, v7g, "Error", result);
            validscript = 0;
        }
    }
}

void run_loop()
{
    v7_set(v7g, v7_get_global(v7g), "TIME", 4, v7_mk_number(v7g, currenttime));
    v7_set(v7g, v7_get_global(v7g), "DELTA_TIME", 10, v7_mk_number(v7g, deltaTime));
    v7_set(v7g, v7_get_global(v7g), "WINDOW_WIDTH", 12, v7_mk_number(v7g, options.width));
    v7_set(v7g, v7_get_global(v7g), "WINDOW_HEIGHT", 13, v7_mk_number(v7g, options.height));
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
    else
    {
        drawRenderTargets();
    }
}

int shutdownScript()
{
    v7_destroy(v7g);
    cleanupRender();

    if(globals)
    {
        for(int i = 0; i < numglobals; i++)
        {
            free(globals[i]);
        }

        free(globals);
    }

    if(entrypointfile)
    {
        free(entrypointfile);
    }

    return 1;
}

void reloadScript(const char* filename)
{
    setsighandler(1);
    int oldnumglobals = numglobals;
    printf("reloading script: %s due to change in %s\n", entrypointfile, filename);
    printf("num globals stored: %zu\n", numglobals);
    char** tmpstorage = malloc(sizeof(char*)*oldnumglobals);

    for(int i = 0; i < oldnumglobals; i++)
    {
        tmpstorage[i] = 0;
        v7_val_t g = v7_get(v7g, v7_get_global(v7g), globals[i], ~0);

        if(v7_is_undefined(g) == 0)
        {
            tmpstorage[i] = v7_to_json(v7g, g, 0, 0);
        }
    }

    v7_destroy(v7g);
    cleanupRender();
    cleanupDebug(); // mostly to delete opengl textures
    initDebug();

    if(setjmp(before) == 0)
    {
        initScript(entrypointfile);

        for(int i = 0; i < oldnumglobals; i++)
        {
            if(tmpstorage[i] != 0)
            {
                v7_val_t oldvalues;
                enum v7_err err = v7_parse_json(v7g, tmpstorage[i], &oldvalues);

                if(err == V7_OK)
                {
                    v7_set(v7g, v7_get_global(v7g), globals[i], ~0, oldvalues);
                }
            }
        }

    }
    else
    {
        validscript = 0;
    }

    if(tmpstorage)
    {
        for(int i = 0; i < oldnumglobals; i++)
        {
            free(tmpstorage[i]);
        }

        free(tmpstorage);
    }

    setsighandler(0);
}
