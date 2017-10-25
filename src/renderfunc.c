#include "opengl.h"
#include "renderfunc.h"
#include <stdlib.h>
#include <stdio.h>
#include "options.h"

rendertarget* rendertargets = 0;
unsigned int numrendertargets = 0;

unsigned int CreateRenderTarget(unsigned int width,
                                unsigned int height,
                                unsigned int layers,
                                unsigned int format,
                                unsigned int type,
                                unsigned int magfilter,
                                unsigned int minfilter)
{
    if(numrendertargets == 0)
    {
        rendertargets = malloc(sizeof(rendertarget));
    }
    else
    {
        void* newrendertargets = realloc(rendertargets, sizeof(rendertarget) * (numrendertargets + 1));

        if(newrendertargets == 0)
        {
            fprintf(stderr, "out of memory creating rendertarget\n");
            exit(1);
        }
        else
        {
            rendertargets = newrendertargets;
        }
    }

    unsigned int out = numrendertargets;
    numrendertargets += 1;
    rendertarget* target = &rendertargets[out];
    printf("creating a buffer:\n");
    printf("    width:  %u\n", width);
    printf("    height: %u\n", height);

    if(layers <= 32)
    {
        printf("    layers: %u\n", layers);
    }
    else
    {
        printf("    layers: %u error capping layers to 32\n", layers);
        layers = 32;
    }

    printf("    format: %u\n", format);
    printf("    type:   %u\n", type);
    target->textures = malloc(sizeof(unsigned int) * layers);
    target->height   = height;
    target->width    = width;
    target->format   = format;
    target->type     = type;
    target->layers   = layers;
    GLint oldFramebuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFramebuffer);
    glGenFramebuffers(1, &target->buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, target->buffer);
    glGenTextures(layers, target->textures);
    int components = 0;

    switch(format)
    {
        case GL_RED:
        case GL_RED_INTEGER:
        case GL_STENCIL_INDEX:
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_STENCIL:
        {
            components = 1;
            break;
        }

        case GL_RG:
        case GL_RG_INTEGER:
        {
            components = 2;
            break;
        }

        case GL_RGB:
        case GL_BGR:
        case GL_RGB_INTEGER:
        case GL_BGR_INTEGER:
        {
            components = 3;
            break;
        }

        case GL_RGBA:
        case GL_BGRA:
        case GL_BGRA_INTEGER:
        case GL_RGBA_INTEGER:
        {
            components = 4;
            break;
        }

        default:
            printf("unknown format %u\n", format);
            break;
    }

    GLint oldTextureBinding = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTextureBinding);

    for(int i = 0; i < layers; i++)
    {
        glBindTexture(GL_TEXTURE_2D, target->textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, format, type, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target->textures[i], 0);
    }

    glGenRenderbuffers(1, &target->depth);
    glBindRenderbuffer(GL_RENDERBUFFER, target->depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target->depth);
    glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer);
    glBindTexture(GL_TEXTURE_2D, oldTextureBinding);
    printf("numrendertargets:%u\n", numrendertargets);
    return out;
}

void clear(float red, float green, float blue, float alpha)
{
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void cleanupRender()
{
    for(int i = 0; i < numrendertargets; i++)
    {
        rendertarget* target = &rendertargets[i];
        glDeleteTextures(target->layers, target->textures);

        if(target->textures)
        {
            free(target->textures);
        }

        glDeleteFramebuffers(1, &target->buffer);
    }

    if(rendertargets)
    {
        free(rendertargets);
    }

    rendertargets = 0;
    numrendertargets = 0;
}
int currentPassTarget = 0;
void beginPass(int target, int* width, int* height)
{
    currentPassTarget = target;

    if(currentPassTarget == -1)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, options.width, options.height);
        *width = options.width;
        *height = options.height;
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, rendertargets[target].buffer);
        glViewport(0, 0, rendertargets[target].width, rendertargets[target].height);
        *height = rendertargets[target].height;
        *width = rendertargets[target].width;
    }
}
void endPass()
{
    if(currentPassTarget == -1)
    {
        glfwSwapBuffers(window);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
