#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "opengl.h"
#include "notify.h"
#include "resources.h"
#include "renderfunc.h"

shader* shaders;
unsigned int numshaders = 0;

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

    if(data == 0)
    {
        fprintf(stderr, "out of memory loading %s\n", filename);
        exit(1);
    }

    data[len] = 0;
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

    fclose(f);
    return out;
}

unsigned char samestring(const char* p1, const char* p2)
{
    if(p1 == p2)
    {
        return 1;
    }

    if(p1 != 0 && p2 != 0)
    {
        return strcmp(p1, p2) == 0;
    }

    return 0;
}

char* strdupnull(const char* str)
{
    if(str)
    {
        return strdup(str);
    }

    return 0;
}

int allocateNewShader(const char* vertex,
                      const char* pixel,
                      const char* geometry,
                      const char* tesscontrol,
                      const char* tesseval)
{
    for(int i = 0; i < numshaders; i++)
    {
        shader* s = &shaders[i];

        if(
            samestring(s->vertname, vertex) &&
            samestring(s->fragname, pixel) &&
            samestring(s->geomname, geometry) &&
            samestring(s->controlname, tesscontrol) &&
            samestring(s->evalname, tesseval)
        )
        {
            return i;
        }
    }

    if(numshaders == 0)
    {
        shaders = malloc(sizeof(shader));
    }
    else
    {
        void* newshaders = realloc(shaders, sizeof(shader) * (numshaders + 1));

        if(newshaders == 0)
        {
            fprintf(stderr, "out of memory while creating new shaders\n");
            exit(1);
        }
        else
        {
            shaders = newshaders;
        }
    }

    shader* s = &shaders[numshaders];
    s->vertname = strdupnull(vertex);
    s->fragname = strdupnull(pixel);
    s->geomname = strdupnull(geometry);
    s->controlname = strdupnull(tesscontrol);
    s->evalname = strdupnull(tesseval);
    numshaders++;
    return numshaders - 1;
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

void cleanupShaders(int shader)
{
    if(shaders)
    {
        for(int i = 0; i < numshaders; i++)
        {
            if(shaders[i].vertname)
            {
                free(shaders[i].vertname);
            }

            if(shaders[i].fragname)
            {
                free(shaders[i].fragname);
            }

            if(shaders[i].evalname)
            {
                free(shaders[i].evalname);
            }

            if(shaders[i].controlname)
            {
                free(shaders[i].controlname);
            }

            if(shaders[i].geomname)
            {
                free(shaders[i].geomname);
            }
        }

        free(shaders);
        numshaders = 0;
    }
}

void bindShader(int shader)
{
}
