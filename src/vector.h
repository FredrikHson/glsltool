#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct vec3
{
    double x;
    double y;
    double z;
} vec3;

typedef struct vec4
{
    double x;
    double y;
    double z;
    double w;
} vec4;

double vec3dot(const vec3 v1, const vec3 v2);
vec3 vec3normalize(const vec3 v1);
vec3 vec3cross(const vec3 v1, const vec3 v2);
vec3 vec3add(const vec3 v1, const vec3 v2);
vec3 vec3sub(const vec3 v1, const vec3 v2);
vec3 vec3mul(const vec3 v1, const double s);

#endif //__VECTOR_H__
