#ifndef __MATRIX_H__
#define __MATRIX_H__

typedef struct mat4
{
    double m[16];
} mat4;

mat4 mat4loadidentity();
mat4 mat4setscale(double x, float y, float z);
mat4 mat4settranslation(double x, float y, float z);
mat4 mat4mul(const mat4* m1, const mat4* m2);
mat4 mat4setperspective(double fov, float aspect, float near, float far);
mat4 mat4setrotr(double angle, float x, float y, float z);
void setUniformMat4f(const char* name, const mat4* m);
void setUniformMat4d(const char* name, const mat4* m);
struct vec3 vec3mat4mul(const struct vec3* v, const mat4* mat);
struct vec4 vec4mat4mul(const struct vec4* v, const mat4* mat);
mat4 mat4transpose(const mat4* m);
mat4 mat4invert(const mat4* m);

#endif //__MATRIX_H__
