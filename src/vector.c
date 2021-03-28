#include <math.h>
#include "vector.h"

double vec3dot(const vec3 v1, const vec3 v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec3 vec3normalize(const vec3 v1)
{
    vec3 out = {0};
    double len = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);

    if(len != 0)
    {
        out.x = v1.x / len;
        out.y = v1.y / len;
        out.z = v1.z / len;
    }

    return out;
}

vec3 vec3cross(const vec3 v1, const vec3 v2)
{
    vec3 out;
    out.x = v1.y * v2.z - v1.z * v2.y;
    out.y = v1.z * v2.x - v1.x * v2.z;;
    out.z = v1.x * v2.y - v1.y * v2.x;;
    return out;
}

vec3 vec3add(const vec3 v1, const vec3 v2)
{
    vec3 out;
    out.x = v1.x + v1.x;
    out.y = v1.y + v1.y;
    out.z = v1.z + v1.z;
    return out;
}

vec3 vec3sub(const vec3 v1, const vec3 v2)
{
    vec3 out;
    out.x = v1.x - v1.x;
    out.y = v1.y - v1.y;
    out.z = v1.z - v1.z;
    return out;
}

vec3 vec3mul(const vec3 v1, const double s)
{
    vec3 out;
    out.x = v1.x * s;
    out.y = v1.y * s;
    out.z = v1.z * s;
    return out;
}
