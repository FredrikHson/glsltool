#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "debug.h"
#include "opengl.h"
#include "options.h"
#include "renderfunc.h"

char debugmode = DEBUG_OFF;
unsigned int debug_step = 0; // for single stepping/stop at this step
unsigned int currentstep = 0; // internal current texture number

typedef struct debugtex
{
    unsigned int width;
    unsigned int height;
    unsigned int format;
    unsigned int type;
    unsigned int texture;
} debugtex;

debugtex* debugTex = 0;
unsigned int numDebugTex = 0;
extern rendertarget* rendertargets;
extern unsigned int numrendertargets;
unsigned int debug_vbo = 0;
unsigned int debug_vao = 0;

unsigned int debug_vs = 0;
unsigned int debug_fs = 0;
unsigned int debug_program = 0;
int debug_texloc = 0;

extern char glsl_debug_vert[];
extern char glsl_debug_frag[];
extern unsigned int glsl_debug_vert_len;
extern unsigned int glsl_debug_frag_len;

void initDebug()
{
    glGenBuffers(1, &debug_vbo);
    glGenVertexArrays(1, &debug_vao);
    // create shader
    debug_vs = glCreateShader(GL_VERTEX_SHADER);
    debug_fs = glCreateShader(GL_FRAGMENT_SHADER);
    int len = glsl_debug_vert_len;
    char* data = glsl_debug_vert;
    glShaderSource(debug_vs, 1, (const char**)&data, &len);
    len = glsl_debug_frag_len;
    data = glsl_debug_frag;
    glShaderSource(debug_fs, 1, (const char**)&data, &len);
    glCompileShader(debug_vs);
    glCompileShader(debug_fs);
    debug_program = glCreateProgram();
    glAttachShader(debug_program, debug_vs);
    glAttachShader(debug_program, debug_fs);
    glBindAttribLocation(debug_program, 0, "in_Position");
    glBindAttribLocation(debug_program, 1, "in_Uvs");
    glLinkProgram(debug_program);
    debug_texloc = glGetUniformLocation(debug_program, "image");
}

void drawRenderTargets()
{
    if(debugmode == DEBUG_OFF)
    {
        return;
    }

    /*printf("drawing rendertargets numDebugTex:%i\n", numDebugTex);*/
    int lines = ceil(sqrt(numDebugTex));
    /*printf("x %i y %i\n", lines, lines);*/
    float width = 2.0f / (float)lines;
    glUseProgram(debug_program);
    glActiveTexture(GL_TEXTURE0);

    if(debugmode == DEBUG_RENDERALLSTEPS)
    {
        for(int i = 0; i < numDebugTex; i++)
        {
            float x = (float)(i % lines);
            float y = (float)(i / lines);
            float x1 = width * x - 1.0f;
            float y1 = width * y - 1.0f;
            float x2 = width * x + width - 1.0f;
            float y2 = width * y + width - 1.0f;
            float verts[30] =
            {
                x1, y1, 0.0f, 0.0f, 0.0f,
                x2, y1, 0.0f, 1.0f, 0.0f,
                x1, y2, 0.0f, 0.0f, 1.0f,
                x1, y2, 0.0f, 0.0f, 1.0f,
                x2, y1, 0.0f, 1.0f, 0.0f,
                x2, y2, 0.0f, 1.0f, 1.0f
            };
            glBindVertexArray(debug_vao);
            glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);
            glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), verts, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glUniform1i(debug_texloc, 0);
            glBindTexture(GL_TEXTURE_2D, debugTex[i].texture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    else if(debugmode == DEBUG_RENDERSINGLESTEP)
    {
        float verts[30] =
        {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f
        };
        glBindVertexArray(debug_vao);
        glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);
        glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glUniform1i(debug_texloc, 0);
        glBindTexture(GL_TEXTURE_2D, debugTex[debug_step % numDebugTex].texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glUseProgram(0);
    currentstep = 0;
    glfwSwapBuffers(window);
}

unsigned int generateNewDebugTexture()
{
    if(numDebugTex == 0)
    {
        debugTex = malloc(sizeof(unsigned int));
    }
    else
    {
        void* newdebugtex = realloc(debugTex, sizeof(debugtex) * (numDebugTex + 1));

        if(newdebugtex == 0)
        {
            fprintf(stderr, "out of memory creating debug texture\n");
            exit(1);
        }
        else
        {
            debugTex = newdebugtex;
        }
    }

    unsigned int out = numDebugTex;
    numDebugTex += 1;
    glGenTextures(1, &debugTex[out].texture);
    debugTex[out].width  = ~0;
    debugTex[out].height = ~0;
    debugTex[out].format = ~0;
    debugTex[out].type   = ~0;
    return out;
}

void cleanupDebug()
{
    if(debugTex != 0)
    {
        for(int i = 0; i < numDebugTex; i++)
        {
            glDeleteTextures(1, &debugTex[i].texture);
        }

        free(debugTex);
        debugTex = 0;
    }

    numDebugTex = 0;
    debug_step = 0;
    currentstep = 0;
    glDeleteBuffers(1, &debug_vbo);
    debug_vbo = 0;
    glDeleteVertexArrays(1, &debug_vao);
    debug_vao = 0;
}

void copyTargetToDebug(unsigned int id)
{
    if(debugmode == DEBUG_OFF)
    {
        return;
    }

    int oldid = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldid);

    if(id == ~0)
    {
        if(currentstep >= numDebugTex)
        {
            generateNewDebugTexture();
        }

        debugtex* dt = &debugTex[currentstep];
        glBindTexture(GL_TEXTURE_2D, dt->texture);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, options.width, options.height, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        currentstep += 1;
    }
    else
    {
        rendertarget* rt = &rendertargets[id];
        unsigned int targetx = 0;
        unsigned int targety = 0;

        if(rt->relative)
        {
            targetx = (unsigned int)(rt->width * (float)options.width);
            targety = (unsigned int)(rt->height * (float)options.height);
        }
        else
        {
            targetx = (unsigned int)rt->width;
            targety = (unsigned int)rt->height;
        }

        for(int i = 0; i < rt->layers; i++)
        {
            if(currentstep >= numDebugTex)
            {
                generateNewDebugTexture();
            }

            debugtex* dt = &debugTex[currentstep];

            if(dt->width != targetx ||
               dt->height != targety ||
               dt->type != rt->type ||
               dt->format != rt->format)
            {
                // recreate the texture
                dt->width = targetx;
                dt->height = targety;
                dt->type = rt->type;
                dt->format = rt->format;
                glBindTexture(GL_TEXTURE_2D, dt->texture);
                glTexImage2D(GL_TEXTURE_2D, 0, dt->type, targetx, targety, 0, dt->format, GL_BYTE, 0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            glCopyImageSubData(rt->textures[i], GL_TEXTURE_2D, 0, 0, 0, 0,
                               dt->texture, GL_TEXTURE_2D, 0, 0, 0, 0,
                               targetx, targety, 1);
            glBindTexture(GL_TEXTURE_2D, dt->texture);
            glGenerateTextureMipmap(dt->texture);
            currentstep += 1;
        }
    }

    glBindTexture(GL_TEXTURE_2D, oldid);
}

char setDebugMode(char mode)
{
    if(mode < DEBUG_LAST)
    {
        debugmode = mode;
        return 1;
    }
    else
    {
        printf("unknown debug mode\n");
        return 0;
    }
}

void setSingleDebugStep(int step)
{
    debug_step = (unsigned int)step;
}
