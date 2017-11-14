#ifndef __MATRIX_H__
#define __MATRIX_H__

typedef struct mat4
{
    float m[16];
} mat4;

mat4 mat4loadidentity();
mat4 mat4setscale(float x, float y, float z);
mat4 mat4settranslation(float x, float y, float z);
mat4 mat4mul(const mat4* m1, const mat4* m2);
mat4 mat4setperspective(float fov, float aspect, float near, float far);
mat4 mat4setrotr(float angle, float x, float y, float z);
void setUniformMat4(const char* name, const mat4* m);
struct vec3 vec3mat4mul(const struct vec3* v, const mat4* mat);
mat4 mat4transpose(const mat4* m);
mat4 mat4invert(const mat4* m);

#endif //__MATRIX_H__
