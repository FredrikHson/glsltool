#include "opengl.h"
#include "v7.h"
#include "renderfunc.h"
#include "resources.h"
#include "generators.h"

typedef struct v7 v7;
enum v7_err js_load_image(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);
    *res = v7_mk_number(v7e, 0);

    if(argc >= 1)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* filename = v7_get_string(v7e, &val, &len);
        char origin = 0;

        if(argc == 2)
        {
            origin = v7_get_int(v7e, v7_arg(v7e, 1));
        }

        if(filename)
        {
            int image = loadImage(filename, origin);

            if(image == -1)
            {
                fprintf(stderr, "failed to load image %s\n", filename);
                return V7_OK;
            }

            printf("imageid %i\n", image);
            *res = v7_mk_number(v7e, image);
            return V7_OK;
        }
        else
        {
            fprintf(stderr, "first argument to loadimage should be a string\n");
            return V7_SYNTAX_ERROR;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadimage\n");
        return V7_SYNTAX_ERROR;
    }
}

enum v7_err js_load_mesh(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* filename = v7_get_string(v7e, &val, &len);

        if(filename)
        {
            int mesh = loadMesh(filename);

            if(mesh == -1)
            {
                fprintf(stderr, "failed to load mesh %s\n", filename);
            }
            else
            {
                printf("meshid %i\n", mesh);
            }

            *res = v7_mk_number(v7e, mesh);
            return V7_OK;
        }
        else
        {
            fprintf(stderr, "first argument to loadmesh should be a string\n");
            return V7_SYNTAX_ERROR;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadmesh\n");
        return V7_SYNTAX_ERROR;
    }
}

enum v7_err js_get_mesh_bbox(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        int meshid = v7_get_int(v7e, v7_arg(v7e, 0));

        if(meshid >= nummeshes)
        {
            fprintf(stderr, "invalid mesh id\n");
            return V7_SYNTAX_ERROR;
        }

        *res = v7_mk_object(v7e);
        v7_set(v7e, *res, "min_x", 5, v7_mk_number(v7e, meshes[meshid].bboxmin[0]));
        v7_set(v7e, *res, "min_y", 5, v7_mk_number(v7e, meshes[meshid].bboxmin[1]));
        v7_set(v7e, *res, "min_z", 5, v7_mk_number(v7e, meshes[meshid].bboxmin[2]));
        v7_set(v7e, *res, "max_x", 5, v7_mk_number(v7e, meshes[meshid].bboxmax[0]));
        v7_set(v7e, *res, "max_y", 5, v7_mk_number(v7e, meshes[meshid].bboxmax[1]));
        v7_set(v7e, *res, "max_z", 5, v7_mk_number(v7e, meshes[meshid].bboxmax[2]));
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to get_mesh_bbox\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_load_shader(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 5)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* vertfilename = v7_get_string(v7e, &val, &len);
        val = v7_arg(v7e, 1);
        const char* pixelfilename = v7_get_string(v7e, &val, &len);
        val = v7_arg(v7e, 2);
        const char* geomfilename = v7_get_string(v7e, &val, &len);
        val = v7_arg(v7e, 3);
        const char* controlfilename = v7_get_string(v7e, &val, &len);
        val = v7_arg(v7e, 4);
        const char* evalfilename = v7_get_string(v7e, &val, &len);
        int shader = loadShader(vertfilename,
                                pixelfilename,
                                geomfilename,
                                controlfilename,
                                evalfilename);

        if(shader == -1)
        {
            fprintf(stderr, "Failed to load shader\n"
                    "    vertex:%s\n"
                    "    fragment:%s\n"
                    "    geometry:%s\n"
                    "    control:%s\n"
                    "    evaluation:%s\n",
                    vertfilename,
                    pixelfilename,
                    geomfilename,
                    controlfilename,
                    evalfilename);
        }

        *res = v7_mk_number(v7e, shader);
        return V7_OK;
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadmesh\n");
        return V7_SYNTAX_ERROR;
    }
}

enum v7_err js_create_rendertarget(v7* v7e, v7_val_t* res)
{
    if(v7_argc(v7e) < 5)
    {
        fprintf(stderr, "invalid number of arguments to create_rendertarget\n");
        return V7_SYNTAX_ERROR;
    }

    float width  = v7_get_double(v7e, v7_arg(v7e, 0));
    float height = v7_get_double(v7e, v7_arg(v7e, 1));
    int layers   = v7_get_int(v7e, v7_arg(v7e, 2));
    int colors   = v7_get_int(v7e, v7_arg(v7e, 3));
    int type     = v7_get_int(v7e, v7_arg(v7e, 4));
    char relative = 0;

    if(v7_argc(v7e) == 6)
    {
        relative = (char)v7_get_int(v7e, v7_arg(v7e, 5));
    }

    unsigned int target = CreateRenderTarget(width, height, layers, colors, type, relative);
    *res = v7_mk_number(v7e, target);
    return V7_OK;
}
enum v7_err js_generate_plane(v7* v7e, v7_val_t* res)
{
    int subw = 0;
    int subh = 0;
    float w = 2;
    float h = 2;

    if(v7_argc(v7e) > 0)
    {
        subw = v7_get_int(v7e, v7_arg(v7e, 0));
        subh = subw;
    }

    if(v7_argc(v7e) > 1)
    {
        subh = v7_get_int(v7e, v7_arg(v7e, 1));
    }

    if(v7_argc(v7e) > 2)
    {
        w = v7_get_double(v7e, v7_arg(v7e, 2));
        h = w;
    }

    if(v7_argc(v7e) > 3)
    {
        h = v7_get_double(v7e, v7_arg(v7e, 3));
    }

    printf("generating plane %i %i %f %f\n", subw, subh, w, h);
    unsigned int target = generatePlane(subw, subh, w, h);
    *res = v7_mk_number(v7e, target);
    return V7_OK;
}

enum v7_err js_image_width(v7* v7e, v7_val_t* res)
{
    if(v7_argc(v7e) != 1)
    {
        fprintf(stderr, "invalid number of arguments to imagewidth\n");
        return V7_SYNTAX_ERROR;
    }

    int id = v7_get_int(v7e, v7_arg(v7e, 0));
    *res = v7_mk_number(v7e, getImageWidth(id));
    return V7_OK;
}

enum v7_err js_image_height(v7* v7e, v7_val_t* res)
{
    if(v7_argc(v7e) != 1)
    {
        fprintf(stderr, "invalid number of arguments to imageheight\n");
        return V7_SYNTAX_ERROR;
    }

    int id = v7_get_int(v7e, v7_arg(v7e, 0));
    *res = v7_mk_number(v7e, getImageHeight(id));
    return V7_OK;
}
