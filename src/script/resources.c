#include "opengl.h"
#include "v7.h"
#include "renderfunc.h"
#include "resources.h"
typedef struct v7 v7;
enum v7_err js_load_image(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);
    *res = v7_mk_number(v7e, 0);

    if(argc == 1)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* filename = v7_get_string(v7e, &val, &len);

        if(filename)
        {
            int image = loadImage(filename);

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
            fprintf(stderr, "first argument to loadmesh should be a string\n");
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

    int width  = v7_get_int(v7e, v7_arg(v7e, 0));
    int height = v7_get_int(v7e, v7_arg(v7e, 1));
    int layers = v7_get_int(v7e, v7_arg(v7e, 2));
    int colors = v7_get_int(v7e, v7_arg(v7e, 3));
    int type   = v7_get_int(v7e, v7_arg(v7e, 4));
    int magfilter = GL_NEAREST;
    int minfilter = GL_NEAREST;

    if(v7_argc(v7e) > 5)
    {
        magfilter = v7_get_int(v7e, v7_arg(v7e, 6));
    }

    if(v7_argc(v7e) > 6)
    {
        minfilter = v7_get_int(v7e, v7_arg(v7e, 7));
    }

    unsigned int target = CreateRenderTarget(width, height, layers, colors, type, minfilter, magfilter);
    *res = v7_mk_number(v7e, target);
    return V7_OK;
}
