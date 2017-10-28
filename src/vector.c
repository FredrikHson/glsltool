#include <math.h>
#include "vector.h"

float vec3dot(const vec3 v1, const vec3 v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec3 vec3normalize(const vec3 v1)
{
    vec3 out = {0};
    float len = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);

    if(len != 0)
    {
        out.x = v1.x / len;
        out.y = v1.y / len;
        out.z = v1.z / len;
    }

    return out;
}
