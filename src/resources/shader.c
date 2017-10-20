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
        printf("reusing shader object:%i\n", out);
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

shader* allocateNewShader(const char* vertex,
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
            return s;
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
    s->program = glCreateProgram();
    s->control = 0;
    s->eval = 0;
    s->frag = 0;
    s->geom = 0;
    s->vert = 0;
    numshaders++;
    return s;
}

int loadShader(const char* vertex,
               const char* pixel,
               const char* geometry,
               const char* tesscontrol,
               const char* tesseval)
{
    shader* s = allocateNewShader(vertex, pixel, geometry, tesscontrol, tesseval);
    s->working = 1;
    char error = 0;

    if(vertex)
    {
        s->vert = loadShaderfile(vertex, GL_VERTEX_SHADER, s->vert, &error);

        if(s->vert == 0)
        {
            fprintf(stderr, "file not found:%s\n", vertex);
            return -1;
        }

        glAttachShader(s->program, s->vert);

        if(error != 0)
        {
            s->working = 0;
        }
    }

    if(pixel)
    {
        s->frag = loadShaderfile(pixel, GL_FRAGMENT_SHADER, s->frag, &error);

        if(s->frag == 0)
        {
            fprintf(stderr, "file not found:%s\n", pixel);
            return -1;
        }

        glAttachShader(s->program, s->frag);

        if(error != 0)
        {
            s->working = 0;
        }
    }

    if(geometry)
    {
        s->geom = loadShaderfile(geometry, GL_GEOMETRY_SHADER, s->geom, &error);

        if(s->geom == 0)
        {
            fprintf(stderr, "file not found:%s\n", geometry);
            return -1;
        }

        glAttachShader(s->program, s->geom);

        if(error != 0)
        {
            s->working = 0;
        }
    }

    if(tesscontrol)
    {
        s->control = loadShaderfile(tesscontrol, GL_TESS_CONTROL_SHADER, s->control, &error);

        if(s->control == 0)
        {
            fprintf(stderr, "file not found:%s\n", tesscontrol);
            return -1;
        }

        glAttachShader(s->program, s->control);

        if(error != 0)
        {
            s->working = 0;
        }
    }

    if(tesseval)
    {
        s->eval = loadShaderfile(tesseval, GL_TESS_EVALUATION_SHADER, s->eval, &error);

        if(s->eval == 0)
        {
            fprintf(stderr, "file not found:%s\n", tesseval);
            return -1;
        }

        glAttachShader(s->program, s->eval);

        if(error != 0)
        {
            s->working = 0;
        }
    }

    if(s->working == 0)
    {
        fprintf(stderr, "shader error going to use default until fixed\n");
    }

    if(vertex)
    {
        watchFile(vertex, &reloadShader);
    }

    if(pixel)
    {
        watchFile(pixel, &reloadShader);
    }

    if(geometry)
    {
        watchFile(geometry, &reloadShader);
    }

    if(tesscontrol)
    {
        watchFile(tesscontrol, &reloadShader);
    }

    if(tesseval)
    {
        watchFile(tesseval, &reloadShader);
    }

    glLinkProgram(s->program);
    int linked;
    glGetProgramiv(s->program, GL_LINK_STATUS, &linked);

    if(!linked)
    {
        int shaderloglen = 0;
        char shaderlog[4096] = {0};
        glGetProgramInfoLog(s->program, 4096, &shaderloglen, shaderlog);
        fprintf(stderr, "Error linking shader: %s %s %s %s %s\n%s\n",
                s->vertname,
                s->fragname,
                s->geomname,
                s->controlname,
                s->evalname,
                shaderlog);
    }

    return 0;
}

void reloadShader(const char* filename)
{
    for(int i = 0; i < numshaders; i++)
    {
        shader* s = &shaders[i];

        if(samestring(filename, s->vertname))
        {
            loadShader(s->vertname, s->fragname, s->geomname, s->controlname, s->evalname);
            continue;
        }

        if(samestring(filename, s->fragname))
        {
            loadShader(s->vertname, s->fragname, s->geomname, s->controlname, s->evalname);
            continue;
        }

        if(samestring(filename, s->geomname))
        {
            loadShader(s->vertname, s->fragname, s->geomname, s->controlname, s->evalname);
            continue;
        }

        if(samestring(filename, s->controlname))
        {
            loadShader(s->vertname, s->fragname, s->geomname, s->controlname, s->evalname);
            continue;
        }

        if(samestring(filename, s->evalname))
        {
            loadShader(s->vertname, s->fragname, s->geomname, s->controlname, s->evalname);
            continue;
        }
    }
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
                glDeleteShader(shaders[i].vert);
            }

            if(shaders[i].fragname)
            {
                free(shaders[i].fragname);
                glDeleteShader(shaders[i].frag);
            }

            if(shaders[i].evalname)
            {
                free(shaders[i].evalname);
                glDeleteShader(shaders[i].eval);
            }

            if(shaders[i].controlname)
            {
                free(shaders[i].controlname);
                glDeleteShader(shaders[i].control);
            }

            if(shaders[i].geomname)
            {
                free(shaders[i].geomname);
                glDeleteShader(shaders[i].geom);
            }

            if(shaders[i].program)
            {
                glDeleteProgram(shaders[i].program);
            }
        }

        free(shaders);
        numshaders = 0;
    }
}

void bindShader(int shader)
{
    if(shader >= numshaders)
    {
        return;
    }
}
