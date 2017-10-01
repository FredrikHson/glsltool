#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "opengl.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include "notify.h"
#include "resources.h"
struct aiColor4D;
struct aiVector3D;

mesh* meshes = 0;
int nummeshes = 0;

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

void printmeshflags(unsigned int flag)
{
    if(flag & MESH_FLAG_POSITION)
    {
        printf("MESH_FLAG_POSITION");
    }

    if(flag & MESH_FLAG_NORMAL)
    {
        printf(" NORMAL");
    }

    if(flag & MESH_FLAG_TANGENT)
    {
        printf(" TANGENT");
    }

    if(flag & MESH_FLAG_BINORMAL)
    {
        printf(" BINORMAL");
    }

    for(int i = 0; i < 8; i++)
    {
        if(flag & MESH_FLAG_TEXCOORD0 << i)
        {
            printf(" TEXCOORD%i", i);
        }
    }

    for(int i = 0; i < 8; i++)
    {
        if(flag & MESH_FLAG_COLOR0 << i)
        {
            printf(" COLOR%i", i);
        }
    }

    printf("\n");
}

int loadMeshfileOntoMesh(const char* filename, unsigned int meshid)
{
    if(meshid >= nummeshes)
    {
        return -1;
    }

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

    mesh* m = &meshes[meshid];

    if(m->flags)
    {
        free(m->flags);
        m->flags = 0;
    }

    if(m->vbo)
    {
        glDeleteBuffers(m->numsubmeshes, m->vbo);
        free(m->vbo);
        m->vbo = 0;
    }

    if(m->indices)
    {
        glDeleteBuffers(m->numsubmeshes, m->indices);
        free(m->indices);
        m->indices = 0;
    }

    if(m->numindices)
    {
        free(m->numindices);
        m->numindices = 0;
    }

    if(m->numverts)
    {
        free(m->numverts);
        m->numverts = 0;
    }

    m->numsubmeshes = scene->mNumMeshes;
    m->flags      = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->vbo        = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->indices    = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->numindices = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->numverts   = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    glGenBuffers(scene->mNumMeshes, m->vbo);
    glGenBuffers(scene->mNumMeshes, m->indices);

    for(int i = 0; i < scene->mNumMeshes; i++)
    {
        struct aiMesh* assmesh = scene->mMeshes[i];
        m->flags[i] = MESH_FLAG_POSITION;
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
                m->flags[i] |= MESH_FLAG_TEXCOORD0 << j;
                size += sizeof(float) * 2;
            }
        }

        for(int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++)
        {
            if(assmesh->mColors[j] != 0)
            {
                m->flags[i] |= MESH_FLAG_COLOR0 << j;
                size += sizeof(float) * 4;
            }
        }

        m->numverts[i] = assmesh->mNumVertices;
        printf("m->flags[%i]=0x%X size=%zu numverts=%i\n", i, m->flags[i], size, assmesh->mNumVertices);
        printmeshflags(m->flags[i]);
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

        glBindBuffer(GL_ARRAY_BUFFER, m->vbo[i]);
        glBufferData(GL_ARRAY_BUFFER, size * assmesh->mNumVertices, data, GL_STATIC_DRAW);
        // assume triangles
        m->numindices[i] = assmesh->mNumFaces * 3;
        unsigned int* indexdata = malloc(sizeof(unsigned int) * assmesh->mNumFaces * 3);

        for(int j = 0; j < assmesh->mNumFaces; j++)
        {
            size_t o = j * 3;
            indexdata[o] = assmesh->mFaces[j].mIndices[0];
            indexdata[o + 1] = assmesh->mFaces[j].mIndices[1];
            indexdata[o + 2] = assmesh->mFaces[j].mIndices[2];
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indices[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*assmesh->mNumFaces * 3, indexdata, GL_STATIC_DRAW);

        if(data)
        {
            free(data);
        }

        if(indexdata)
        {
            free(indexdata);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    watchFile(filename, &reloadMesh);
    aiReleaseImport(scene);
    return meshid;
}
int loadMesh(const char* filename)
{
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
        void* newmeshes = realloc(meshes, sizeof(mesh) * (nummeshes + 1));

        if(newmeshes == 0)
        {
            fprintf(stderr, "out of memory while creating mesh %s\n", filename);
            exit(1);
        }
        else
        {
            meshes = newmeshes;
        }
    }

    out = nummeshes;
    nummeshes += 1;
    mesh* m = &meshes[out];
    memset(m, 0, sizeof(mesh));
    m->name = malloc(strlen(filename) + 1);
    sprintf(m->name, "%s", filename);
    out = loadMeshfileOntoMesh(filename, out);
    return out;
}
void reloadMesh(const char* filename)
{
    for(int i = 0; i < nummeshes; i++)
    {
        if(strcmp(filename, meshes[i].name) == 0)
        {
            printf("wants to reload:%i %s\n", i, filename);
            loadMeshfileOntoMesh(filename, i);
            break;
        }
    }
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

        if(meshes[i].indices)
        {
            glDeleteBuffers(meshes[i].numsubmeshes, meshes[i].indices);
            free(meshes[i].indices);
        }

        if(meshes[i].vbo)
        {
            glDeleteBuffers(meshes[i].numsubmeshes, meshes[i].vbo);
            free(meshes[i].vbo);
        }

        if(meshes[i].numindices)
        {
            free(meshes[i].numindices);
            meshes[i].numindices = 0;
        }

        if(meshes[i].numverts)
        {
            free(meshes[i].numverts);
            meshes[i].numverts = 0;
        }
    }

    if(meshes)
    {
        free(meshes);
    }

    nummeshes = 0;
    meshes = 0;
}

void drawSubmesh(int id, int submesh)
{
    mesh* m = &meshes[id];
    fprintf(stdout, "drawing:%i %i\n", id, submesh);
    // vertex index format pos.xyz[numverts],next.xyz[numverts]
    unsigned int flags = m->flags[submesh];
    unsigned int numverts = m->numverts[submesh];
    size_t offset = 0;
    printmeshflags(flags);
    size_t stride = sizeof(float) * numverts;
    fprintf(stdout, "verts:%i indices:%i\n", numverts, m->numindices[submesh]);
    fprintf(stdout, "stride:%zu\n", stride);

    if(flags & MESH_FLAG_POSITION)
    {
        fprintf(stdout, "has pos    offset:%zu\n", offset);
        offset += stride * 3;
    }

    if(flags & MESH_FLAG_NORMAL)
    {
        fprintf(stdout, "has normal    offset:%zu\n", offset);
        offset += stride * 3;
    }

    if(flags & MESH_FLAG_TANGENT)
    {
        fprintf(stdout, "has tangent    offset:%zu\n", offset);
        offset += stride * 3;
    }

    if(flags & MESH_FLAG_BINORMAL)
    {
        fprintf(stdout, "has binormal    offset:%zu\n", offset);
        offset += stride * 3;
    }

    for(int i = 0; i < 8; i++)
    {
        if(flags & (MESH_FLAG_TEXCOORD0 << i))
        {
            fprintf(stdout, "has texcoord:%i    offset:%zu\n", i, offset);
            offset += stride * 2;
        }
    }

    for(int i = 0; i < 8; i++)
    {
        if(flags & (MESH_FLAG_COLOR0 << i))
        {
            fprintf(stdout, "has vertex color:%i    offset:%zu\n", i, offset);
            offset += stride * 3;
        }
    }
}

void drawMesh(int id, int submesh)
{
    if(id >= nummeshes)
    {
        return;
    }

    if(submesh == -1)
    {
        for(int i = 0; i < meshes[id].numsubmeshes; i++)
        {
            drawSubmesh(id, i);
        }

        exit(0);
    }
    else if(submesh < meshes[id].numsubmeshes)
    {
        drawSubmesh(id, submesh);
    }
}
