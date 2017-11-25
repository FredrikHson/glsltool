#include "opengl.h"
#include "v7.h"
#include "renderfunc.h"
#include "defines.h"
typedef struct v7 v7;
extern v7* v7g;
enum v7_err js_clear(v7* v7e, v7_val_t* res)
{
    float red   = 0;
    float green = 0;
    float blue  = 0;
    float alpha = 0;
    int argc = v7_argc(v7e);

    if(argc == 3)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = v7_get_double(v7e, v7_arg(v7e, 1));
        blue  = v7_get_double(v7e, v7_arg(v7e, 2));
    }
    else if(argc == 4)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = v7_get_double(v7e, v7_arg(v7e, 1));
        blue  = v7_get_double(v7e, v7_arg(v7e, 2));
        alpha = v7_get_double(v7e, v7_arg(v7e, 3));
    }
    else if(argc == 1)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = red;
        blue  = red;
    }
    else if(argc == 0) {}
    else
    {
        fprintf(stderr, "invalid number of arguments to clear\n");
        return V7_SYNTAX_ERROR;
    }

    clear(red, green, blue, alpha, 1, 1);
    return V7_OK;
}

enum v7_err js_clearcolor(v7* v7e, v7_val_t* res)
{
    float red   = 0;
    float green = 0;
    float blue  = 0;
    float alpha = 0;
    int argc = v7_argc(v7e);

    if(argc == 3)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = v7_get_double(v7e, v7_arg(v7e, 1));
        blue  = v7_get_double(v7e, v7_arg(v7e, 2));
    }
    else if(argc == 4)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = v7_get_double(v7e, v7_arg(v7e, 1));
        blue  = v7_get_double(v7e, v7_arg(v7e, 2));
        alpha = v7_get_double(v7e, v7_arg(v7e, 3));
    }
    else if(argc == 1)
    {
        red   = v7_get_double(v7e, v7_arg(v7e, 0));
        green = red;
        blue  = red;
    }
    else if(argc == 0) {}
    else
    {
        fprintf(stderr, "invalid number of arguments to clear\n");
        return V7_SYNTAX_ERROR;
    }

    clear(red, green, blue, alpha, 1, 0);
    return V7_OK;
}

enum v7_err js_cleardepth(v7* v7e, v7_val_t* res)
{
    clear(0, 0, 0, 0, 0, 1);
    return V7_OK;
}

enum v7_err js_beginPass(v7* v7e, v7_val_t* res)
{
    int width;
    int height;

    if(v7_argc(v7e) == 0)
    {
        beginPass(-1, &width, &height);
    }
    else
    {
        beginPass(v7_get_int(v7e, v7_arg(v7e, 0)), &width, &height);
    }

    v7_set(v7g, v7_get_global(v7g), "RENDER_WIDTH", 12, v7_mk_number(v7g, width));
    v7_set(v7g, v7_get_global(v7g), "RENDER_HEIGHT", 13, v7_mk_number(v7g, height));
    return V7_OK;
}

enum v7_err js_endPass(v7* v7e, v7_val_t* res)
{
    endPass();
    return V7_OK;
}

enum v7_err js_draw_mesh(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const int meshid = v7_get_int(v7e, v7_arg(v7e, 0));
        drawMesh(meshid, -1);
    }
    else if(argc == 2)
    {
        const int meshid = v7_get_int(v7e, v7_arg(v7e, 0));
        const int submesh = v7_get_int(v7e, v7_arg(v7e, 1));
        drawMesh(meshid, submesh);
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to loadmesh\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_set_depth(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const int enable = v7_get_int(v7e, v7_arg(v7e, 0));

        if(enable)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to setdepth\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_set_cullface(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const int cullmode = v7_get_int(v7e, v7_arg(v7e, 0));

        switch(cullmode)
        {
            case CULL_NONE:
                glDisable(GL_CULL_FACE);
                break;

            case CULL_FRONT:
                glCullFace(GL_FRONT);
                glEnable(GL_CULL_FACE);
                break;

            case CULL_BACK:
                glCullFace(GL_BACK);
                glEnable(GL_CULL_FACE);
                break;

            case CULL_BOTH:
                glCullFace(GL_FRONT_AND_BACK);
                glEnable(GL_CULL_FACE);
                break;

            default:
                break;
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to cullface\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}

enum v7_err js_wireframe(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        const int enable = v7_get_int(v7e, v7_arg(v7e, 0));

        if(enable)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        }
    }
    else
    {
        fprintf(stderr, "invalid number of arguments to wireframe\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}
