#ifndef __SCRIPTFUNC_H__
#define __SCRIPTFUNC_H__

#include <stdint.h>
#include "v7.h"

typedef struct v7 v7;
typedef uint64_t v7_val_t;
/* resources */
enum v7_err js_create_rendertarget(v7* v7e, v7_val_t* res);
enum v7_err js_load_image(v7* v7e, v7_val_t* res);
enum v7_err js_load_mesh(v7* v7e, v7_val_t* res);
enum v7_err js_load_shader(v7* v7e, v7_val_t* res);
enum v7_err js_get_mesh_bbox(v7* v7e, v7_val_t* res);
enum v7_err js_generate_plane(v7* v7e, v7_val_t* res);
enum v7_err js_image_width(v7* v7e, v7_val_t* res);
enum v7_err js_image_height(v7* v7e, v7_val_t* res);
/* render */
enum v7_err js_beginPass(v7* v7e, v7_val_t* res);
enum v7_err js_endPass(v7* v7e, v7_val_t* res);
enum v7_err js_clear(v7* v7e, v7_val_t* res);
enum v7_err js_clearcolor(v7* v7e, v7_val_t* res);
enum v7_err js_cleardepth(v7* v7e, v7_val_t* res);
enum v7_err js_draw_mesh(v7* v7e, v7_val_t* res);
enum v7_err js_reset_attribs(v7* v7e, v7_val_t* res);
enum v7_err js_set_cullface(v7* v7e, v7_val_t* res);
enum v7_err js_set_depth(v7* v7e, v7_val_t* res);
enum v7_err js_wireframe(v7* v7e, v7_val_t* res);
enum v7_err js_set_debug_mode(v7* v7e, v7_val_t* res);
enum v7_err js_set_debug_step(v7* v7e, v7_val_t* res);
enum v7_err js_set_debug_range(v7* v7e, v7_val_t* res);
enum v7_err js_set_debug_clip(v7* v7e, v7_val_t* res);
/* shader */
enum v7_err js_bind_attrib(v7* v7e, v7_val_t* res);
enum v7_err js_bind_rendertarget(v7* v7e, v7_val_t* res);
enum v7_err js_bind_shader(v7* v7e, v7_val_t* res);
enum v7_err js_bind_texture(v7* v7e, v7_val_t* res);
enum v7_err js_bind_fragment_data(v7* v7e, v7_val_t* res);
enum v7_err js_set_uniform_matrix(v7* v7e, v7_val_t* res);
enum v7_err js_set_uniform_matrix_d(v7* v7e, v7_val_t* res);
enum v7_err js_set_uniformf(v7* v7e, v7_val_t* res);
enum v7_err js_set_uniformd(v7* v7e, v7_val_t* res);
enum v7_err js_set_uniformi(v7* v7e, v7_val_t* res);
enum v7_err js_set_uniformui(v7* v7e, v7_val_t* res);
enum v7_err js_set_default_filter(v7* v7e, v7_val_t* res);
enum v7_err js_set_default_clamp(v7* v7e, v7_val_t* res);
/* math */
enum v7_err js_vec3_dot(v7* v7e, v7_val_t* res);
enum v7_err js_vec3_normalize(v7* v7e, v7_val_t* res);
enum v7_err js_vec3_cross(v7* v7e, v7_val_t* res);
enum v7_err js_vec3_add(v7* v7e, v7_val_t* res);
enum v7_err js_vec3_sub(v7* v7e, v7_val_t* res);
enum v7_err js_vec3_mul(v7* v7e, v7_val_t* res);
enum v7_err js_mat4_loadidentity(v7* v7e, v7_val_t* res);
enum v7_err js_mat4_setscale(v7* v7e, v7_val_t* res);
enum v7_err js_mat4_settranslation(v7* v7e, v7_val_t* res);
enum v7_err js_mat4_setrotation(v7* v7e, v7_val_t* res);
enum v7_err js_mat4_setperspective(v7* v7e, v7_val_t* res);
enum v7_err js_mat4_mul(v7* v7e, v7_val_t* res);
enum v7_err js_vec3_mat4_mul(v7* v7e, v7_val_t* res);
enum v7_err js_vec4_mat4_mul(v7* v7e, v7_val_t* res);
enum v7_err js_mat4_transpose(v7* v7e, v7_val_t* res);
enum v7_err js_mat4_invert(v7* v7e, v7_val_t* res);
enum v7_err js_set_uniform_matrix(v7* v7e, v7_val_t* res);
enum v7_err js_sin(v7* v7e, v7_val_t* res);
enum v7_err js_cos(v7* v7e, v7_val_t* res);
enum v7_err js_tan(v7* v7e, v7_val_t* res);
enum v7_err js_asin(v7* v7e, v7_val_t* res);
enum v7_err js_acos(v7* v7e, v7_val_t* res);
enum v7_err js_atan(v7* v7e, v7_val_t* res);
enum v7_err js_floor(v7* v7e, v7_val_t* res);
enum v7_err js_ceil(v7* v7e, v7_val_t* res);
enum v7_err js_abs(v7* v7e, v7_val_t* res);
enum v7_err js_sqrt(v7* v7e, v7_val_t* res);
enum v7_err js_set_angle_mode(v7* v7e, v7_val_t* res);
/* update variables functions */
void updateinput();
enum v7_err js_register_global(v7* v7e, v7_val_t* res);
/* for getting variables from the commandline */
enum v7_err js_get_optional_string(v7* v7e, v7_val_t* res);
/* window manipulation */
enum v7_err js_set_window_title(v7* v7e, v7_val_t* res);

#endif //__SCRIPTFUNC_H__
