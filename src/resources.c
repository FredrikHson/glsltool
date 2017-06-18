#include <IL/il.h>
#include <IL/ilu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "opengl.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#include <assimp/mesh.h>
#include "notify.h"
#include "resources.h"

image* textures = 0;
int numtextures = 0;
mesh* meshes = 0;
int nummeshes = 0;

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
        textures = realloc(textures, sizeof(image) * (numtextures + 1));
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

#define MESH_FLAG_POSITION  (1<<0)
#define MESH_FLAG_NORMAL    (1<<1)
#define MESH_FLAG_TANGENT   (1<<2)
#define MESH_FLAG_BINORMAL  (1<<3)
#define MESH_FLAG_TEXCOORD0 (1<<4)
#define MESH_FLAG_TEXCOORD1 (1<<5)
#define MESH_FLAG_TEXCOORD2 (1<<6)
#define MESH_FLAG_TEXCOORD3 (1<<7)
#define MESH_FLAG_TEXCOORD4 (1<<8)
#define MESH_FLAG_TEXCOORD5 (1<<9)
#define MESH_FLAG_TEXCOORD6 (1<<10)
#define MESH_FLAG_TEXCOORD7 (1<<11)
#define MESH_FLAG_COLOR0    (1<<12)
#define MESH_FLAG_COLOR1    (1<<13)
#define MESH_FLAG_COLOR2    (1<<14)
#define MESH_FLAG_COLOR3    (1<<15)
#define MESH_FLAG_COLOR4    (1<<16)
#define MESH_FLAG_COLOR5    (1<<17)
#define MESH_FLAG_COLOR6    (1<<18)
#define MESH_FLAG_COLOR7    (1<<19)

int loadMesh(const char* filename)
{
    const struct aiScene* scene = aiImportFile(filename,
                                  aiProcess_CalcTangentSpace      |
                                  aiProcess_Triangulate           |
                                  aiProcess_JoinIdenticalVertices |
                                  aiProcess_SortByPType);

    if(!scene)
    {
        fprintf(stderr, "Error Loading %s\n%s\n", filename, aiGetErrorString());
        return -1;
    }

    printf("nummeshes:%i\n", scene->mNumMeshes);

    for(int i = 0; i < scene->mNumMeshes; i++)
    {
        struct aiMesh* assmesh = scene->mMeshes[i];
        printf("    mesh_name:%s\n", assmesh->mName.data);
        printf("    numBones:%i\n", assmesh->mNumBones);
        printf("    numFaces:%i\n", assmesh->mNumFaces);
        printf("    numVertices:%i\n", assmesh->mNumVertices);
    }

    /*
     * howto?
     * add all verts normals and tangents to a buffer
     * anims to texture?
     * or something
     */
    unsigned int out  = 0;

    for(int i = 0; i < nummeshes; i++)
    {
        if(strcmp(filename, meshes[i].name) == 0)
        {
            printf("already loaded mesh %s id:%i\n", filename, i);
            return i;
        }
    }

    if(nummeshes == 0)
    {
        meshes = malloc(sizeof(mesh));
    }
    else
    {
        meshes = realloc(meshes, sizeof(mesh) * (nummeshes + 1));
    }

    out  = nummeshes;
    nummeshes += 1;
    mesh* m = &meshes[out];
    m->name = malloc(strlen(filename) + 1);
    sprintf(m->name, "%s", filename);
    m->flags = malloc(sizeof(unsigned int) * scene->mNumMeshes);

    for(int i = 0; i < scene->mNumMeshes; i++)
    {
        struct aiMesh* assmesh = scene->mMeshes[i];
        m->flags[i] |= MESH_FLAG_POSITION;
        size_t size = sizeof(float) * 3;

        if(assmesh->mNormals != 0)
        {
            m->flags[i] |= MESH_FLAG_NORMAL;
            size += sizeof(float) * 3;
        }

        if(assmesh->mTangents != 0)
        {
            m->flags[i] |= MESH_FLAG_TANGENT;
            size += sizeof(float) * 3;
        }

        if(assmesh->mBitangents != 0)
        {
            m->flags[i] |= MESH_FLAG_BINORMAL;
            size += sizeof(float) * 3;
        }

        for(int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
        {
            if(assmesh->mTextureCoords[j] != 0)
            {
                m->flags[i] |= MESH_FLAG_TEXCOORD0 << i;
                size += sizeof(float) * 2;
            }
        }

        for(int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++)
        {
            if(assmesh->mColors[j] != 0)
            {
                m->flags[i] |= MESH_FLAG_COLOR0 << i;
                size += sizeof(float) * 4;
            }
        }

        printf("m->flags[%i]=0x%X size=%zu numverts=%i\n", i, m->flags[i], size, assmesh->mNumVertices);
        float* data = malloc(size * assmesh->mNumVertices);
        size_t offset = 0;

        for(int i = 0; i < assmesh->mNumVertices; i++)
        {
            data[offset] = assmesh->mVertices[i].x;
            data[offset + 1] = assmesh->mVertices[i].y;
            data[offset + 2] = assmesh->mVertices[i].z;
            offset += 3;
        }

        if(assmesh->mNormals)
        {
            for(int i = 0; i < assmesh->mNumVertices; i++)
            {
                data[offset] = assmesh->mNormals[i].x;
                data[offset + 1] = assmesh->mNormals[i].y;
                data[offset + 2] = assmesh->mNormals[i].z;
                offset += 3;
            }
        }

        if(assmesh->mTangents)
        {
            for(int i = 0; i < assmesh->mNumVertices; i++)
            {
                data[offset] = assmesh->mTangents[i].x;
                data[offset + 1] = assmesh->mTangents[i].y;
                data[offset + 2] = assmesh->mTangents[i].z;
                offset += 3;
            }
        }

        if(assmesh->mBitangents)
        {
            for(int i = 0; i < assmesh->mNumVertices; i++)
            {
                data[offset] = assmesh->mBitangents[i].x;
                data[offset + 1] = assmesh->mBitangents[i].y;
                data[offset + 2] = assmesh->mBitangents[i].z;
                offset += 3;
            }
        }

        for(int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
        {
            if(assmesh->mTextureCoords[j])
            {
                for(int i = 0; i < assmesh->mNumVertices; i++)
                {
                    data[offset] = assmesh->mTextureCoords[j][i].x;
                    data[offset + 1] = assmesh->mTextureCoords[j][i].y;
                    offset += 2;
                }
            }
        }

        for(int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++)
        {
            if(assmesh->mColors[j])
            {
                for(int i = 0; i < assmesh->mNumVertices; i++)
                {
                    data[offset] = assmesh->mColors[j][i].r;
                    data[offset + 1] = assmesh->mColors[j][i].g;
                    data[offset + 2] = assmesh->mColors[j][i].b;
                    data[offset + 3] = assmesh->mColors[j][i].a;
                    offset += 4;
                }
            }
        }

        free(data);
    }

    /*watchFile(filename, &reloadImage);*/
    aiReleaseImport(scene);
    return out;
}

void cleanupMeshes()
{
    for(int i = 0; i < nummeshes; i++)
    {
        if(meshes[i].name)
        {
            free(meshes[i].name);
        }

        if(meshes[i].flags)
        {
            free(meshes[i].flags);
        }
    }

    if(meshes)
    {
        free(meshes);
    }

    nummeshes = 0;
    meshes = 0;
}
