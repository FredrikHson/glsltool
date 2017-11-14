#include "matrix.h"
#include "vector.h"
#include <math.h>


mat4 mat4loadidentity()
{
    mat4 out =
    {
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        }
    };
    return out;
}

mat4 mat4setscale(float x, float y, float z)
{
    mat4 out =
    {
        {
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        }
    };
    return out;
}

mat4 mat4settranslation(float x, float y, float z)
{
    mat4 out =
    {
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
        }
    };
    return out;
}

mat4 mat4mul(const mat4* m1, const mat4* m2)
{
    mat4 out;
    out.m[0] = m1->m[0] * m2->m[0]   + m1->m[1] * m2->m[4]  + m1->m[2] * m2->m[8]   + m1->m[3] * m2->m[12];
    out.m[1] = m1->m[0] * m2->m[1]   + m1->m[1] * m2->m[5]  + m1->m[2] * m2->m[9]   + m1->m[3] * m2->m[13];
    out.m[2] = m1->m[0] * m2->m[2]   + m1->m[1] * m2->m[6]  + m1->m[2] * m2->m[10]  + m1->m[3] * m2->m[14];
    out.m[3] = m1->m[0] * m2->m[3]   + m1->m[1] * m2->m[7]  + m1->m[2] * m2->m[11]  + m1->m[3] * m2->m[15];
    out.m[4] = m1->m[4] * m2->m[0]   + m1->m[5] * m2->m[4]  + m1->m[6] * m2->m[8]   + m1->m[7] * m2->m[12];
    out.m[5] = m1->m[4] * m2->m[1]   + m1->m[5] * m2->m[5]  + m1->m[6] * m2->m[9]   + m1->m[7] * m2->m[13];
    out.m[6] = m1->m[4] * m2->m[2]   + m1->m[5] * m2->m[6]  + m1->m[6] * m2->m[10]  + m1->m[7] * m2->m[14];
    out.m[7] = m1->m[4] * m2->m[3]   + m1->m[5] * m2->m[7]  + m1->m[6] * m2->m[11]  + m1->m[7] * m2->m[15];
    out.m[8] = m1->m[8] * m2->m[0]   + m1->m[9] * m2->m[4]  + m1->m[10] * m2->m[8]  + m1->m[11] * m2->m[12];
    out.m[9] = m1->m[8] * m2->m[1]   + m1->m[9] * m2->m[5]  + m1->m[10] * m2->m[9]  + m1->m[11] * m2->m[13];
    out.m[10] = m1->m[8] * m2->m[2]  + m1->m[9] * m2->m[6]  + m1->m[10] * m2->m[10] + m1->m[11] * m2->m[14];
    out.m[11] = m1->m[8] * m2->m[3]  + m1->m[9] * m2->m[7]  + m1->m[10] * m2->m[11] + m1->m[11] * m2->m[15];
    out.m[12] = m1->m[12] * m2->m[0] + m1->m[13] * m2->m[4] + m1->m[14] * m2->m[8]  + m1->m[15] * m2->m[12];
    out.m[13] = m1->m[12] * m2->m[1] + m1->m[13] * m2->m[5] + m1->m[14] * m2->m[9]  + m1->m[15] * m2->m[13];
    out.m[14] = m1->m[12] * m2->m[2] + m1->m[13] * m2->m[6] + m1->m[14] * m2->m[10] + m1->m[15] * m2->m[14];
    out.m[15] = m1->m[12] * m2->m[3] + m1->m[13] * m2->m[7] + m1->m[14] * m2->m[11] + m1->m[15] * m2->m[15];
    return out;
}

mat4 mat4setperspective(float fov, float aspect, float near, float far)
{
    mat4 out;
    float f = 1.0f / tan(fov / 2.0f);
    out.m[0]  = f / aspect;
    out.m[1]  = 0;
    out.m[2]  = 0;
    out.m[3]  = 0;
    out.m[4]  = 0;
    out.m[5]  = f;
    out.m[6]  = 0;
    out.m[7]  = 0;
    out.m[8]  = 0;
    out.m[9]  = 0;
    out.m[10] = (far + near) / (near - far);
    out.m[11] = (2 * far * near) / (near - far);
    out.m[12] = 0;
    out.m[13] = 0;
    out.m[14] = -1;
    out.m[15] = 0;
    return out;
}

mat4 mat4setrotr(float angle, float x, float y, float z)
{
    float l = sqrt(x * x + y * y + z * z);
    float x2 = x / l;
    float y2 = y / l;
    float z2 = z / l;
    float s = sin(angle);
    float c = cos(angle);
    float ic = 1.0f - c;
    mat4 out;
    out.m[0]  = (ic * x2 * x2) + c;
    out.m[1]  = (ic * x2 * y2) - z2 * s;
    out.m[2]  = (ic * z2 * x2) + y2 * s;
    out.m[3]  = 0.0f;
    out.m[4]  = (ic * x2 * y2) + z2 * s;
    out.m[5]  = (ic * y2 * y2) + c;
    out.m[6]  = (ic * y2 * z2) - x2 * s;
    out.m[7]  = 0.0f;
    out.m[8]  = (ic * z2 * x2) - y2 * s;
    out.m[9]  = (ic * y2 * z2) + x2 * s;
    out.m[10] = (ic * z2 * z2) + c;
    out.m[11] = 0.0f;
    out.m[12] = 0.0f;
    out.m[13] = 0.0f;
    out.m[14] = 0.0f;
    out.m[15] = 1.0f;
    return out;
}


vec3 vec3mat4mul(const vec3* v, const mat4* mat)
{
    vec3 out;
    out.x = v->x * mat->m[0] + v->y * mat->m[4] + v->z * mat->m[8] + mat->m[12];
    out.y = v->x * mat->m[1] + v->y * mat->m[5] + v->z * mat->m[9] + mat->m[13];
    out.z = v->x * mat->m[2] + v->y * mat->m[6] + v->z * mat->m[10] + mat->m[14];
    float w = v->x * mat->m[3] + v->y * mat->m[7] + v->z * mat->m[11] * mat->m[15];
    out.x /= w;
    out.y /= w;
    out.z /= w;
    return out;
}
