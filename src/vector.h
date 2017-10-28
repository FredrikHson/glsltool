#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct vec3
{
    float x;
    float y;
    float z;
} vec3;

float vec3dot(const vec3 v1, const vec3 v2);
vec3 vec3normalize(const vec3 v1);

#endif //__VECTOR_H__
