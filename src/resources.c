#include "resources.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <stdlib.h>
#include <stdio.h>
#include "opengl.h"

int loadImage(const char* filename)
{
    unsigned int id = 0;
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    ilEnable(IL_ORIGIN_SET);
    ilGenImages(1, &id);
    ilBindImage(id);
    ilLoadImage(filename);
    int width  = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    int bpp    = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    int type   = ilGetInteger(IL_IMAGE_TYPE);
    int format = ilGetInteger(IL_IMAGE_FORMAT);
    printf("%s h:%i w:%i c:%i type:", filename, height, width, bpp);
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
            break;
        }

        case IL_RGBA:
        {
            printf("IL_RGBA");
            glInternalFormat = GL_RGBA;
            break;
        }

        case IL_BGR:
        {
            printf("IL_BGR");
            format = IL_RGB;
            glInternalFormat = GL_RGB;
            break;
        }

        case IL_BGRA:
        {
            printf("IL_BGRA");
            format = IL_RGBA;
            glInternalFormat = GL_RGBA;
            break;
        }

        case IL_LUMINANCE:
        {
            printf("IL_LUMINANCE");
            glInternalFormat = GL_RED;
            break;
        }

        default:
            break;
    }

    printf("\n");
    fflush(stdout);
    void* data = malloc(width * height * bpp);
    ilCopyPixels(0, 0, 0, width, height, 1, format, type, data);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, glInternalFormat, glType, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(data);
    ilBindImage(0);
    ilDeleteImage(id);
    return 0;
}
