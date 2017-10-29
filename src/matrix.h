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

#endif //__MATRIX_H__
