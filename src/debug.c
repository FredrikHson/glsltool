#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "opengl.h"
#include "options.h"
#include "renderfunc.h"

char debugmode = DEBUG_RENDERSINGLESTEP;
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

void drawRenderTargets()
{
    /*printf("drawing rendertargets numDebugTex:%i\n", numDebugTex);*/
    currentstep = 0;
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
    printf("cleaning up debug stuff\n");
}

void copyTargetToDebug(unsigned int id)
{
    int oldid = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldid);

    if(id == ~0)
    {
        /*printf("copying screen step:%i currentstep:%i\n", currentstep, currentstep);*/

        if(currentstep >= numDebugTex)
        {
            generateNewDebugTexture();
        }

        currentstep += 1;
    }
    else
    {
        /*printf("copying rendertarget %i step:%i currentstep:%i\n", id, currentstep, currentstep);*/
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

        /*printf("\twidth:%u height:%u layers:%i\n", targetx, targety, rendertargets[id].layers);*/

        for(int i = 0; i < rt->layers; i++)
        {
            if(currentstep >= numDebugTex)
            {
                generateNewDebugTexture();
            }

            debugtex* dt = &debugTex[currentstep];
            /*printf("\tdebug width:%u height:%u\n", dt->width, dt->height);*/

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
            /*printf("\tdebug width:%u height:%u\n", dt->width, dt->height);*/
            currentstep += 1;
        }

        printf("\n");
        /*glBindFramebuffer(GL_READ_FRAMEBUFFER, textureid);*/
        /*int oldid = 0;*/
        /*glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldid);*/
        /*glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, format, GL_BYTE, 0);*/
        /*glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);*/
    }

    glBindTexture(GL_TEXTURE_2D, oldid);
    /*printf("numDebugTex:%i\n", numDebugTex);*/
}

void setDebugMode(char mode)
{
    if(mode < DEBUG_LAST)
    {
        debugmode = mode;
    }
    else
    {
        printf("unknown debug mode\n");
    }
}
