#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "opengl.h"
#include "notify.h"
#include "resources.h"
#include "renderfunc.h"
#include "matrix.h"

shader* shaders;
unsigned int numshaders = 0;

unsigned int currentprogram = 0;
unsigned int usingtessellation = 0;

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
        printf("reusing shader object:%u\n", out);
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
                          const char* tesseval,
                          size_t* id)
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
            *id = i;
            s->cleanup = CLEAN_USED;
            return s;
        }
    }

    int reuse = 0;
    shader* s = 0;


    for(int i = 0; i < numshaders; i++)
    {
        if(shaders[i].cleanup & CLEAN_DELETED)
        {
            reuse = 1;
            s = &shaders[i];
            *id = i;
            break;
        }
    }

    if(!reuse)
    {

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

        s = &shaders[numshaders];
        *id = numshaders;
        numshaders++;
    }

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
    s->cleanup = CLEAN_USED;
    return s;
}

int loadShader(const char* vertex,
               const char* pixel,
               const char* geometry,
               const char* tesscontrol,
               const char* tesseval)
{
    size_t out = 0;
    shader* s = allocateNewShader(vertex, pixel, geometry, tesscontrol, tesseval, &out);
    s->working = 1;
    char error = 0;

    if(vertex)
    {
        if(s->vert)
        {
            glDetachShader(s->program, s->vert);
        }

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
        if(s->frag)
        {
            glDetachShader(s->program, s->frag);
        }

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
        if(s->geom)
        {
            glDetachShader(s->program, s->geom);
        }

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
        if(s->control)
        {
            glDetachShader(s->program, s->control);
        }

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
        if(s->eval)
        {
            glDetachShader(s->program, s->eval);
        }

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
        s->working = 0;
    }

    return out;
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

void cleanupShader(shader* s)
{

    if(s->vertname)
    {
        free(s->vertname);
        s->vertname = 0;
        glDetachShader(s->program, s->vert);
        glDeleteShader(s->vert);
    }

    if(s->fragname)
    {
        free(s->fragname);
        s->fragname = 0;
        glDetachShader(s->program, s->frag);
        glDeleteShader(s->frag);
    }

    if(s->evalname)
    {
        free(s->evalname);
        s->evalname = 0;
        glDetachShader(s->program, s->eval);
        glDeleteShader(s->eval);
    }

    if(s->controlname)
    {
        free(s->controlname);
        s->control = 0;
        glDetachShader(s->program, s->control);
        glDeleteShader(s->control);
    }

    if(s->geomname)
    {
        free(s->geomname);
        s->geomname = 0;
        glDetachShader(s->program, s->geom);
        glDeleteShader(s->geom);
    }

    if(s->program)
    {
        glDeleteProgram(s->program);
    }

    s->cleanup = CLEAN_DELETED;
}

void cleanupShaders(void)
{
    if(shaders)
    {
        for(int i = 0; i < numshaders; i++)
        {
            cleanupShader(&shaders[i]);
        }

        free(shaders);
        numshaders = 0;
    }
}

void bindShader(int id)
{
    resetTexturebindings();

    if(id < 0)
    {
        glUseProgram(0);
        currentprogram = 0;
        usingtessellation = 0;
    }
    else
    {
        if(id >= numshaders)
        {
            return;
        }

        if(shaders[id].program == currentprogram)
        {
            return;
        }

        shader* s = &shaders[id];

        if(s->working)
        {
            glUseProgram(s->program);
            currentprogram = s->program;
        }
        else
        {
            glUseProgram(0);
            currentprogram = 0;
        }

        if(s->eval && s->control)
        {
            usingtessellation = 1;
        }
        else
        {
            usingtessellation = 0;
        }
    }
}

void setUniformf(const char* name, float* f, int argc)
{
    if(currentprogram == 0)
    {
        return;
    }

    int loc = glGetUniformLocation(currentprogram, name);

    switch(argc)
    {
        case 1:
            glUniform1f(loc, f[0]);
            break;

        case 2:
            glUniform2f(loc, f[0], f[1]);
            break;

        case 3:
            glUniform3f(loc, f[0], f[1], f[2]);
            break;

        case 4:
            glUniform4f(loc, f[0], f[1], f[2], f[3]);
            break;

        default:
            break;
    }
}

void setUniformd(const char* name, double* f, int argc)
{
    if(currentprogram == 0)
    {
        return;
    }

    int loc = glGetUniformLocation(currentprogram, name);

    switch(argc)
    {
        case 1:
            glUniform1d(loc, f[0]);
            break;

        case 2:
            glUniform2d(loc, f[0], f[1]);
            break;

        case 3:
            glUniform3d(loc, f[0], f[1], f[2]);
            break;

        case 4:
            glUniform4d(loc, f[0], f[1], f[2], f[3]);
            break;

        default:
            break;
    }
}

void setUniformi(const char* name, int* f, int argc)
{
    if(currentprogram == 0)
    {
        return;
    }

    int loc = glGetUniformLocation(currentprogram, name);

    switch(argc)
    {
        case 1:
            glUniform1i(loc, f[0]);
            break;

        case 2:
            glUniform2i(loc, f[0], f[1]);
            break;

        case 3:
            glUniform3i(loc, f[0], f[1], f[2]);
            break;

        case 4:
            glUniform4i(loc, f[0], f[1], f[2], f[3]);
            break;

        default:
            break;
    }
}

void setUniformui(const char* name, unsigned int* f, int argc)
{
    if(currentprogram == 0)
    {
        return;
    }

    int loc = glGetUniformLocation(currentprogram, name);

    switch(argc)
    {
        case 1:
            glUniform1ui(loc, f[0]);
            break;

        case 2:
            glUniform2ui(loc, f[0], f[1]);
            break;

        case 3:
            glUniform3ui(loc, f[0], f[1], f[2]);
            break;

        case 4:
            glUniform4ui(loc, f[0], f[1], f[2], f[3]);
            break;

        default:
            break;
    }
}

void setUniformMat4f(const char* name, const mat4* m)
{
    if(currentprogram == 0)
    {
        return;
    }

    int loc = glGetUniformLocation(currentprogram, name);
    float mat[16] =
    {
        m->m[0], m->m[1], m->m[2], m->m[3],
        m->m[4], m->m[5], m->m[6], m->m[7],
        m->m[8], m->m[9], m->m[10], m->m[11],
        m->m[12], m->m[13], m->m[14], m->m[15]
    };
    glUniformMatrix4fv(loc, 1, 0, mat);
}

void setUniformMat4d(const char* name, const mat4* m)
{
    if(currentprogram == 0)
    {
        return;
    }

    int loc = glGetUniformLocation(currentprogram, name);
    glUniformMatrix4dv(loc, 1, 0, m->m);
}
