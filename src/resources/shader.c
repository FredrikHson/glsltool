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

unsigned int loadShaderfile(const char* filename, int shadertype, unsigned int shaderobject)
{
    FILE* f = fopen(filename, "rb");

    if(!f)
    {
        return 0;
    }

    int len = getFilesize(f);
    char* data = malloc(len);

    if(!data)
    {
        fprintf(stderr, "out of memory loading %s\n", filename);
    }

    fread(&data, len, 1, f);
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

    if(vertex)
    {
        s.vert = loadShaderfile(vertex, GL_VERTEX_SHADER, s.vert);
    }

    return 0;
}
