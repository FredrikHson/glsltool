#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include "renderfunc.h"
#include <stdlib.h>
#include <stdio.h>

struct rendertarget* rendertargets = 0;
unsigned int numrendertargets = 0;
unsigned int CreateRenderTarget(unsigned int width,
                                unsigned int height,
                                unsigned int layers,
                                unsigned int format,
                                unsigned int type)
{

    if(numrendertargets == 0)
    {
        rendertargets = malloc(sizeof(struct rendertarget));
    }
    else
    {
        rendertargets = realloc(rendertargets, sizeof(struct rendertarget) * (numrendertargets + 1));

    }

    unsigned int out = numrendertargets;
    numrendertargets += 1;
    struct rendertarget* target = &rendertargets[out];

    printf("creating a buffer:\n");
    printf("    width:  %i\n", width);
    printf("    height: %i\n", height);
    printf("    layers: %i\n", layers);
    printf("    format: %i\n", format);
    printf("    type:   %i\n", type);

    target->textures = malloc(sizeof(unsigned int) * layers);
    target->height   = height;
    target->width    = width;
    target->format   = format;
    target->type     = type;
    target->layers   = layers;

    glGenFramebuffers(1, &target->buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, target->buffer);

    printf("numrendertargets:%i\n", numrendertargets);

    return out;
}
