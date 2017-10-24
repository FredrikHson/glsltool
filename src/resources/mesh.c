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
#include "defines.h"

struct aiColor4D;
struct aiVector3D;

extern unsigned int currentprogram;

typedef struct vertattribute
{
    int flag;  // MESH_FLAGS
    char name[256];
    char used;
} vertattribute;

mesh* meshes = 0;
int nummeshes = 0;
vertattribute* attribs = 0;
int maxattribs = 0;
unsigned int numActiveAttribs = 0;

void printmeshflags(unsigned int flag)
{
    if(flag & MESH_FLAG_POSITION)
    {
        printf("POSITION");
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

void bindAttribute(int flag, unsigned int* attrib, int components, size_t offset)
{
    for(int i = 0; i < maxattribs; i++)
    {
        if(attribs[i].flag & flag)
        {
            attribs[i].used = 1;
            glEnableVertexAttribArray(*attrib);
            glVertexAttribPointer(*attrib, components, GL_FLOAT, GL_FALSE, 0, (void*)offset);
            glBindAttribLocation(currentprogram, *attrib, attribs[i].name);
            *attrib += 1;
            return;
        }
    }
}

void drawSubmesh(int id, int submesh)
{
    mesh* m = &meshes[id];
    /*fprintf(stdout, "drawing:%s id:%i submesh:%i\n", m->name,id, submesh);*/
    // vertex index format pos.xyz[numverts],next.xyz[numverts]
    unsigned int flags = m->flags[submesh];
    unsigned int numverts = m->numverts[submesh];
    size_t offset = 0;
    size_t stride = sizeof(float) * numverts;
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo[submesh]);
    unsigned int currattrib = 0;

    if(flags & MESH_FLAG_POSITION)
    {
        bindAttribute(MESH_FLAG_POSITION, &currattrib, 3, offset);
        offset += stride * 3;
    }

    if(flags & MESH_FLAG_NORMAL)
    {
        bindAttribute(MESH_FLAG_NORMAL, &currattrib, 3, offset);
        offset += stride * 3;
    }

    if(flags & MESH_FLAG_TANGENT)
    {
        bindAttribute(MESH_FLAG_TANGENT, &currattrib, 3, offset);
        offset += stride * 3;
    }

    if(flags & MESH_FLAG_BINORMAL)
    {
        bindAttribute(MESH_FLAG_BINORMAL, &currattrib, 3, offset);
        offset += stride * 3;
    }

    for(int i = 0; i < 8; i++)
    {
        if(flags & (MESH_FLAG_TEXCOORD0 << i))
        {
            bindAttribute((MESH_FLAG_TEXCOORD0 << i), &currattrib, 2, offset);
            offset += stride * 2;
        }
    }

    for(int i = 0; i < 8; i++)
    {
        if(flags & (MESH_FLAG_COLOR0 << i))
        {
            bindAttribute((MESH_FLAG_COLOR0 << i), &currattrib, 3, offset);
            offset += stride * 3;
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indices[submesh]);
    glDrawElements(GL_TRIANGLES, m->numindices[submesh], GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    currattrib = 0;

    for(int i = 0; i < maxattribs; i++)
    {
        if(attribs[i].used)
        {
            attribs[i].used = 0;
            glDisableVertexAttribArray(currattrib);
            currattrib += 1;
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
    }
    else if(submesh < meshes[id].numsubmeshes)
    {
        drawSubmesh(id, submesh);
    }
}
void resetAttribs()
{
    numActiveAttribs = 0;
}

int bindAttrib(const char* name, int flag)
{
    if(0 == attribs)
    {
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxattribs);
        fprintf(stdout, "GL_MAX_VERTEX_ATTRIBS:%i\n", maxattribs);
        attribs = malloc(sizeof(vertattribute) * maxattribs);
        memset(attribs, 0, sizeof(vertattribute) * maxattribs);
        numActiveAttribs = 0;
    }

    if(numActiveAttribs >= maxattribs)
    {
        fprintf(stderr, "Already assigned %i attributes can't do any more\n", maxattribs);
        return 0;
    }

    if(flag && !(flag & (flag - 1)))
    {
        attribs[numActiveAttribs].flag = flag;
        snprintf(attribs[numActiveAttribs].name, 256, "%s", name);
        numActiveAttribs++;
    }
    else
    {
        fprintf(stderr, "More than one mesh attribute specified\n");
        return 0;
    }

    return 1;
}
