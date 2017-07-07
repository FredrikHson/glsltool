#include <stdlib.h>
#include <stdio.h>
#include "opengl.h"
#include "notify.h"
#include "resources.h"


size_t getFilesize(FILE* f)
{
    if(!f)
    {
        return 0;
    }

    size_t pos = ftell(f);
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, pos, SEEK_SET);
    return len;
}

unsigned int loadShaderfile(const char* filename, int shadertype, unsigned int shaderobject, char* error)
{
    printf("loading shader file:%s\n", filename);
    FILE* f = fopen(filename, "rb");

    if(!f)
    {
        return 0;
    }

    int len = getFilesize(f);
    char* data = malloc(len + 1);
    data[len] = 0;

    if(!data)
    {
        fprintf(stderr, "out of memory loading %s\n", filename);
    }

    fread(data, len, 1, f);
    unsigned int out = 0;

    if(shaderobject == 0)
    {
        out = glCreateShader(shadertype);
    }
    else
    {
        out = shaderobject;
    }

    glShaderSource(out, 1, (const char**)&data, &len);
    glCompileShader(out);
    int compiled;
    glGetShaderiv(out, GL_COMPILE_STATUS, &compiled);

    if(!compiled)
    {
        int shaderloglen = 0;
        char shaderlog[4096] = {0};
        glGetShaderInfoLog(out, 4096, &shaderloglen, shaderlog);
        fprintf(stderr, "Error %s:\n%s\n", filename, shaderlog);
        *error = 1;
    }

    if(data)
    {
        free(data);
    }

    return out;
}

int loadShader(const char* vertex,
               const char* pixel,
               const char* geometry,
               const char* tesscontrol,
               const char* tesseval)
{
    shader s = {0};
    s.working = 1;
    char error = 0;

    if(vertex)
    {
        s.vert = loadShaderfile(vertex, GL_VERTEX_SHADER, s.vert, &error);

        if(s.vert == 0)
        {
            fprintf(stderr, "file not found:%s\n", vertex);
            return -1;
        }

        if(error != 0)
        {
            s.working = 0;
        }
    }

    if(pixel)
    {
        s.frag = loadShaderfile(pixel, GL_FRAGMENT_SHADER, s.frag, &error);

        if(s.frag == 0)
        {
            fprintf(stderr, "file not found:%s\n", pixel);
            return -1;
        }

        if(error != 0)
        {
            s.working = 0;
        }
    }

    if(geometry)
    {
        s.geom = loadShaderfile(geometry, GL_GEOMETRY_SHADER, s.geom, &error);

        if(s.geom == 0)
        {
            fprintf(stderr, "file not found:%s\n", geometry);
            return -1;
        }

        if(error != 0)
        {
            s.working = 0;
        }
    }

    if(tesscontrol)
    {
        s.control = loadShaderfile(tesscontrol, GL_TESS_CONTROL_SHADER, s.control, &error);

        if(s.control == 0)
        {
            fprintf(stderr, "file not found:%s\n", tesscontrol);
            return -1;
        }

        if(error != 0)
        {
            s.working = 0;
        }
    }

    if(tesseval)
    {
        s.eval = loadShaderfile(tesseval, GL_TESS_EVALUATION_SHADER, s.eval, &error);

        if(s.eval == 0)
        {
            fprintf(stderr, "file not found:%s\n", tesseval);
            return -1;
        }

        if(error != 0)
        {
            s.working = 0;
        }
    }

    if(s.working == 0)
    {
        fprintf(stderr, "shader error going to use default until fixed\n");
    }

    return 0;
}
