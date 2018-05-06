#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "opengl.h"
#include "renderfunc.h"

char debugmode = DEBUG_RENDERSINGLESTEP;
unsigned int debug_step = 0; // for single stepping/stop at this step
unsigned int currentstep = 0; // internal current texture number

unsigned int* debugTex = 0;
unsigned int numDebugTex = 0;

void drawRenderTargets()
{
    printf("drawing rendertargets numDebugTex:%i\n", numDebugTex);
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
        void* newdebugtex = realloc(debugTex, sizeof(unsigned int) * (numDebugTex + 1));

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
    glGenTextures(1, &debugTex[out]);
    return debugTex[out];
}
void cleanupDebug()
{
    if(debugTex != 0)
    {
        glDeleteTextures(numDebugTex, debugTex);
        free(debugTex);
        debugTex = 0;
    }

    numDebugTex = 0;
    debug_step = 0;
    currentstep = 0;
    printf("cleaning up debug stuff\n");
}

void copyTargetToDebug(unsigned int textureid)
{
    unsigned int target = 0;

    if(currentstep < numDebugTex)
    {
        target = debugTex[currentstep];
    }
    else
    {
        target = generateNewDebugTexture();
    }

    if(textureid == ~0)
    {
        printf("copying screen step:%i target:%i\n", currentstep, target);
    }
    else
    {
        printf("copying rendertarget %i step:%i target:%i\n", textureid, currentstep, target);
    }

    currentstep += 1;
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
