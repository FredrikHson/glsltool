#include <IL/il.h>
#include <IL/ilu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "opengl.h"
#include "notify.h"
#include "resources.h"

image* textures = 0;
int numtextures = 0;

int loadImageOntoTexture(const char* filename, unsigned int texture)
{
    if(texture >= numtextures)
    {
        return -1;
    }

    unsigned int id = 0;
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    ilEnable(IL_ORIGIN_SET);
    ilGenImages(1, &id);
    ilBindImage(id);
    ilLoadImage(filename);
    ILenum Error;
    int out = 0;

    while((Error = ilGetError()) != IL_NO_ERROR)
    {
        fprintf(stderr, "%s: %s\n", filename, iluErrorString(Error));
        out = -1;
    }

    if(out == -1)
    {
        return out;
    }

    int width    = ilGetInteger(IL_IMAGE_WIDTH);
    int height   = ilGetInteger(IL_IMAGE_HEIGHT);
    int bpp      = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    int type     = ilGetInteger(IL_IMAGE_TYPE);
    int format   = ilGetInteger(IL_IMAGE_FORMAT);
    int channels = 0;
    printf("loading image %s h:%i w:%i c:%i type:", filename, height, width, bpp);
    int glType = GL_UNSIGNED_BYTE;

    switch(type)
    {
        case IL_BYTE:
        {
            printf("IL_BYTE");
            glType = GL_BYTE;
            break;
        }

        case IL_SHORT:
        {
            printf("IL_SHORT");
            glType = GL_SHORT;
            break;
        }

        case IL_UNSIGNED_BYTE:
        {
            printf("IL_UNSIGNED_BYTE");
            glType = GL_UNSIGNED_BYTE;
            break;
        }

        case IL_UNSIGNED_SHORT:
        {
            printf("IL_UNSIGNED_SHORT");
            glType = GL_UNSIGNED_SHORT;
            break;
        }

        case IL_INT:
        {
            printf("IL_INT");
            glType = GL_INT;
            break;
        }

        case IL_UNSIGNED_INT:
        {
            printf("IL_UNSIGNED_INT");
            glType = GL_UNSIGNED_INT;
            break;
        }

        case IL_FLOAT:
        {
            printf("IL_FLOAT");
            glType = GL_FLOAT;
            break;
        }

        case IL_DOUBLE:
        {
            printf("IL_DOUBLE");
            glType = GL_DOUBLE;
            break;
        }

        default:
            break;
    }

    printf(" Format:");
    int glInternalFormat = 0;

    switch(format)
    {
        case IL_RGB:
        {
            printf("IL_RGB");
            glInternalFormat = GL_RGB;
            channels = 3;
            break;
        }

        case IL_RGBA:
        {
            printf("IL_RGBA");
            glInternalFormat = GL_RGBA;
            channels = 4;
            break;
        }

        case IL_BGR:
        {
            printf("IL_BGR");
            format = IL_RGB;
            glInternalFormat = GL_RGB;
            channels = 3;
            break;
        }

        case IL_BGRA:
        {
            printf("IL_BGRA");
            format = IL_RGBA;
            glInternalFormat = GL_RGBA;
            channels = 4;
            break;
        }

        case IL_LUMINANCE:
        {
            printf("IL_LUMINANCE");
            glInternalFormat = GL_RED;
            channels = 1;
            break;
        }

        default:
            break;
    }

    printf("\n");
    fflush(stdout);
    image* tex = &textures[texture];
    void* data = malloc(width * height * bpp);
    ilCopyPixels(0, 0, 0, width, height, 1, format, type, data);
    glBindTexture(GL_TEXTURE_2D, tex->glImage);
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, glInternalFormat, glType, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(data);
    ilBindImage(0);
    ilDeleteImage(id);
    tex->channels = channels;
    tex->width    = width;
    tex->height   = height;
    return texture;
}

int loadImage(const char* filename)
{
    unsigned int out  = 0;

    for(int i = 0; i < numtextures; i++)
    {
        if(strcmp(filename, textures[i].name) == 0)
        {
            printf("already loaded image %s id:%i\n", filename, i);
            return i;
        }
    }

    if(numtextures == 0)
    {
        textures = malloc(sizeof(image));
    }
    else
    {
        void* newtextures = realloc(textures, sizeof(image) * (numtextures + 1));

        if(newtextures == 0)
        {
            fprintf(stderr, "out of memory while creating texture %s\n", filename);
            exit(1);
        }
        else
        {
            textures = newtextures;
        }
    }

    out  = numtextures;
    numtextures += 1;
    image* img = &textures[out];
    img->name = malloc(strlen(filename) + 1);
    glGenTextures(1, &img->glImage);
    out = loadImageOntoTexture(filename, out);
    sprintf(img->name, "%s", filename);
    watchFile(filename, &reloadImage);
    return out;
}

void reloadImage(const char* filename)
{
    for(int i = 0; i < numtextures; i++)
    {
        if(strcmp(filename, textures[i].name) == 0)
        {
            printf("wants to reload:%i %s\n", i, filename);
            loadImageOntoTexture(filename, i);
            break;
        }
    }
}

void cleanupImages()
{
    glBindTexture(GL_TEXTURE_2D, 0);

    for(int i = 0; i < numtextures; i++)
    {
        if(textures[i].name)
        {
            free(textures[i].name);
        }

        glDeleteTextures(1, &textures[i].glImage);
    }

    if(textures)
    {
        free(textures);
    }

    numtextures = 0;
    textures = 0;
}