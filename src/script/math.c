#include "script.h"
#include <math.h>
#include "v7.h"
#include "defines.h"
#include "vector.h"
#include "matrix.h"

typedef struct v7 v7;
extern v7* v7g;
char anglemode = RADIANS;

float convertangle(float inputangle)
{
    if(anglemode == DEGREES)
    {
        return inputangle * (M_PI / 180.0f);
    }
    else
    {
        return inputangle;
    }
}

enum v7_err js_set_angle_mode(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        char newmode = v7_get_int(v7e, v7_arg(v7e, 0));

        if(newmode == DEGREES || newmode == RADIANS)
        {
            anglemode = newmode;
        }
        else
        {
            fprintf(stderr, "setmat4anglemode needs to have DEGREES or RADIANS as input\n");
        }
    }
    else
    {
        fprintf(stderr, "setmat4anglemode needs to have DEGREES or RADIANS as input\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_vec3_dot(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            vec3 v2;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            v2.x = v7_get_double(v7e, v7_get(v7e, arg2, "x", 1));
            v2.y = v7_get_double(v7e, v7_get(v7e, arg2, "y", 1));
            v2.z = v7_get_double(v7e, v7_get(v7e, arg2, "z", 1));
            *res = v7_mk_number(v7e, vec3dot(v1, v2));
        }
    }

    return V7_OK;
}

enum v7_err js_vec3_normalize(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);

        if(v7_is_object(arg1))
        {
            vec3 v1 = {0};
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            vec3 normalized = vec3normalize(v1);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, normalized.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, normalized.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, normalized.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3normalize\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

enum v7_err js_vec3_cross(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            vec3 v2;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            v2.x = v7_get_double(v7e, v7_get(v7e, arg2, "x", 1));
            v2.y = v7_get_double(v7e, v7_get(v7e, arg2, "y", 1));
            v2.z = v7_get_double(v7e, v7_get(v7e, arg2, "z", 1));
            vec3 crossproduct = vec3cross(v1, v2);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, crossproduct.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, crossproduct.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, crossproduct.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3cross\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

enum v7_err js_vec3_add(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            vec3 v2;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            v2.x = v7_get_double(v7e, v7_get(v7e, arg2, "x", 1));
            v2.y = v7_get_double(v7e, v7_get(v7e, arg2, "y", 1));
            v2.z = v7_get_double(v7e, v7_get(v7e, arg2, "z", 1));
            vec3 add = vec3add(v1, v2);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, add.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, add.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, add.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3add\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

enum v7_err js_vec3_sub(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            vec3 v2;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            v2.x = v7_get_double(v7e, v7_get(v7e, arg2, "x", 1));
            v2.y = v7_get_double(v7e, v7_get(v7e, arg2, "y", 1));
            v2.z = v7_get_double(v7e, v7_get(v7e, arg2, "z", 1));
            vec3 sub = vec3add(v1, v2);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, sub.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, sub.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, sub.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3sub\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

enum v7_err js_vec3_mul(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1))
        {
            vec3 v1;
            float scalar;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            scalar = v7_get_double(v7e, arg2);
            vec3 mul = vec3mul(v1, scalar);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, mul.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, mul.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, mul.z));
        }
        else
        {
            fprintf(stderr, "invalid type for vec3mul\n");
            return V7_SYNTAX_ERROR;
        }
    }

    return V7_OK;
}

v7_val_t mat4tov7_val(v7* v7e, const mat4 m)
{
    v7_val_t out = v7_mk_object(v7e);
    v7_set(v7e, out, "m11", 3, v7_mk_number(v7e, m.m[0]));
    v7_set(v7e, out, "m12", 3, v7_mk_number(v7e, m.m[1]));
    v7_set(v7e, out, "m13", 3, v7_mk_number(v7e, m.m[2]));
    v7_set(v7e, out, "m14", 3, v7_mk_number(v7e, m.m[3]));
    v7_set(v7e, out, "m21", 3, v7_mk_number(v7e, m.m[4]));
    v7_set(v7e, out, "m22", 3, v7_mk_number(v7e, m.m[5]));
    v7_set(v7e, out, "m23", 3, v7_mk_number(v7e, m.m[6]));
    v7_set(v7e, out, "m24", 3, v7_mk_number(v7e, m.m[7]));
    v7_set(v7e, out, "m31", 3, v7_mk_number(v7e, m.m[8]));
    v7_set(v7e, out, "m32", 3, v7_mk_number(v7e, m.m[9]));
    v7_set(v7e, out, "m33", 3, v7_mk_number(v7e, m.m[10]));
    v7_set(v7e, out, "m34", 3, v7_mk_number(v7e, m.m[11]));
    v7_set(v7e, out, "m41", 3, v7_mk_number(v7e, m.m[12]));
    v7_set(v7e, out, "m42", 3, v7_mk_number(v7e, m.m[13]));
    v7_set(v7e, out, "m43", 3, v7_mk_number(v7e, m.m[14]));
    v7_set(v7e, out, "m44", 3, v7_mk_number(v7e, m.m[15]));
    return out;
}

mat4 v7_val_tomat4(v7* v7e, v7_val_t m)
{
    mat4 out;
    out.m[0] = v7_get_double(v7e, v7_get(v7e, m, "m11", 3));
    out.m[1] = v7_get_double(v7e, v7_get(v7e, m, "m12", 3));
    out.m[2] = v7_get_double(v7e, v7_get(v7e, m, "m13", 3));
    out.m[3] = v7_get_double(v7e, v7_get(v7e, m, "m14", 3));
    out.m[4] = v7_get_double(v7e, v7_get(v7e, m, "m21", 3));
    out.m[5] = v7_get_double(v7e, v7_get(v7e, m, "m22", 3));
    out.m[6] = v7_get_double(v7e, v7_get(v7e, m, "m23", 3));
    out.m[7] = v7_get_double(v7e, v7_get(v7e, m, "m24", 3));
    out.m[8] = v7_get_double(v7e, v7_get(v7e, m, "m31", 3));
    out.m[9] = v7_get_double(v7e, v7_get(v7e, m, "m32", 3));
    out.m[10] = v7_get_double(v7e, v7_get(v7e, m, "m33", 3));
    out.m[11] = v7_get_double(v7e, v7_get(v7e, m, "m34", 3));
    out.m[12] = v7_get_double(v7e, v7_get(v7e, m, "m41", 3));
    out.m[13] = v7_get_double(v7e, v7_get(v7e, m, "m42", 3));
    out.m[14] = v7_get_double(v7e, v7_get(v7e, m, "m43", 3));
    out.m[15] = v7_get_double(v7e, v7_get(v7e, m, "m44", 3));
    return out;
}

enum v7_err js_mat4_loadidentity(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 0)
    {
        *res = mat4tov7_val(v7e, mat4loadidentity());
    }
    else
    {
        fprintf(stderr, "mat4loadidentity does not take any arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_mat4_setscale(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const float scale = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = mat4tov7_val(v7e, mat4setscale(scale, scale, scale));
    }
    else if(argc == 3)
    {
        const float x = v7_get_double(v7e, v7_arg(v7e, 0));
        const float y = v7_get_double(v7e, v7_arg(v7e, 1));
        const float z = v7_get_double(v7e, v7_arg(v7e, 2));
        *res = mat4tov7_val(v7e, mat4setscale(x, y, z));
    }
    else
    {
        fprintf(stderr, "mat4setscale requires 1 or 3 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_mat4_settranslation(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 3)
    {
        const float x = v7_get_double(v7e, v7_arg(v7e, 0));
        const float y = v7_get_double(v7e, v7_arg(v7e, 1));
        const float z = v7_get_double(v7e, v7_arg(v7e, 2));
        *res = mat4tov7_val(v7e, mat4settranslation(x, y, z));
    }
    else
    {
        fprintf(stderr, "mat4settranslation requires 3 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_mat4_setrotation(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 4)
    {
        const float angle = convertangle(v7_get_double(v7e, v7_arg(v7e, 0)));
        const float x = v7_get_double(v7e, v7_arg(v7e, 1));
        const float y = v7_get_double(v7e, v7_arg(v7e, 2));
        const float z = v7_get_double(v7e, v7_arg(v7e, 3));
        *res = mat4tov7_val(v7e, mat4setrotr(angle, x, y, z));
    }
    else
    {
        fprintf(stderr, "mat4setrotation requires 4 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_mat4_setperspective(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 4)
    {
        const float fov = convertangle(v7_get_double(v7e, v7_arg(v7e, 0)));
        const float aspect = v7_get_double(v7e, v7_arg(v7e, 1));
        const float near = v7_get_double(v7e, v7_arg(v7e, 2));
        const float far = v7_get_double(v7e, v7_arg(v7e, 3));
        *res = mat4tov7_val(v7e, mat4setperspective(fov, aspect, near, far));
    }
    else
    {
        fprintf(stderr, "mat4setperspective requires 4 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_mat4_mul(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            const mat4 m1 = v7_val_tomat4(v7e, arg1);
            const mat4 m2 = v7_val_tomat4(v7e, arg2);
            *res = mat4tov7_val(v7e, mat4mul(&m1, &m2));
        }
        else
        {
            *res = mat4tov7_val(v7e, mat4loadidentity());
            fprintf(stderr, "mat4mul needs 2 matrices as arguments\n");
        }
    }
    else
    {
        fprintf(stderr, "mat4mul needs 2 arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_vec3_mat4_mul(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);

        if(v7_is_object(arg1) && v7_is_object(arg2))
        {
            vec3 v1;
            v1.x = v7_get_double(v7e, v7_get(v7e, arg1, "x", 1));
            v1.y = v7_get_double(v7e, v7_get(v7e, arg1, "y", 1));
            v1.z = v7_get_double(v7e, v7_get(v7e, arg1, "z", 1));
            const mat4 m = v7_val_tomat4(v7e, arg2);
            vec3 mul = vec3mat4mul(&v1, &m);
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, mul.x));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, mul.y));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, mul.z));
        }
        else
        {
            *res = v7_mk_object(v7e);
            v7_set(v7e, *res, "x", 1, v7_mk_number(v7e, 0));
            v7_set(v7e, *res, "y", 1, v7_mk_number(v7e, 0));
            v7_set(v7e, *res, "z", 1, v7_mk_number(v7e, 0));
            fprintf(stderr, "vec3mat4mul needs 1 vec3 and 1 matrix as arguments\n");
        }
    }
    else
    {
        fprintf(stderr, "vec3mat4mul needs 1 vec3 and 1 matrix as arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_set_uniform_matrix(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 2)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);
        v7_val_t arg2 = v7_arg(v7e, 1);
        size_t len = 0;
        const char* name = v7_get_string(v7e, &arg1, &len);

        if(v7_is_object(arg2))
        {
            const mat4 m1 = v7_val_tomat4(v7e, arg2);
            setUniformMat4(name, &m1);
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setuniformmat4\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_mat4_transpose(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);

        if(v7_is_object(arg1))
        {
            const mat4 m = v7_val_tomat4(v7e, arg1);
            *res = mat4tov7_val(v7e, mat4transpose(&m));
        }
        else
        {
            *res = mat4tov7_val(v7e, mat4loadidentity());
            fprintf(stderr, "mat4transpose needs 1 matrix as arguments\n");
        }
    }
    else
    {
        fprintf(stderr, "mat4transpose needs 1 matrix as arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}
enum v7_err js_mat4_invert(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        v7_val_t arg1 = v7_arg(v7e, 0);

        if(v7_is_object(arg1))
        {
            const mat4 m = v7_val_tomat4(v7e, arg1);
            *res = mat4tov7_val(v7e, mat4invert(&m));
        }
        else
        {
            *res = mat4tov7_val(v7e, mat4loadidentity());
            fprintf(stderr, "mat4invert needs 1 matrix as arguments\n");
        }
    }
    else
    {
        fprintf(stderr, "mat4invert needs 1 matrix as arguments\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_sin(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, sin(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to sin\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_cos(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, cos(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to cos\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_tan(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, tan(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to tan\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_asin(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, asin(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to asin\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_acos(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, acos(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to acos\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_atan(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, atan(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to atan\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_floor(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, floor(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to floor\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_ceil(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, ceil(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to ceil\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_abs(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, fabs(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to abs\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_sqrt(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        double v = v7_get_double(v7e, v7_arg(v7e, 0));
        *res = v7_mk_number(v7e, sqrt(v));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to sqrt\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}
