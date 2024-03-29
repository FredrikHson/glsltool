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
#include <float.h>

struct aiColor4D;
struct aiVector3D;

extern unsigned int currentprogram;
extern unsigned int usingtessellation;

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
int editmesh = -1;
float* editmeshvbodata = 0;
unsigned int* editmeshindexdata = 0;

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
unsigned int sizeOfVert(unsigned int flag)
{
    unsigned int size = 0;

    if(flag & MESH_FLAG_POSITION)
    {
        size += sizeof(float) * 3;
    }

    if(flag & MESH_FLAG_NORMAL)
    {
        size += sizeof(float) * 3;
    }

    if(flag & MESH_FLAG_TANGENT)
    {
        size += sizeof(float) * 3;
    }

    if(flag & MESH_FLAG_BINORMAL)
    {
        size += sizeof(float) * 3;
    }

    for(int i = 0; i < 8; i++)
    {
        if(flag & MESH_FLAG_TEXCOORD0 << i)
        {
            size += sizeof(float) * 3;
        }
    }

    for(int i = 0; i < 8; i++)
    {
        if(flag & MESH_FLAG_COLOR0 << i)
        {
            size += sizeof(float) * 3;
        }
    }

    return size;
}

int loadMeshfileOntoMesh(const char* filename, unsigned int meshid)
{
    if(meshid >= nummeshes)
    {
        return -1;
    }

    unsigned int flags = aiProcess_CalcTangentSpace |
                         aiProcess_GenNormals       |
                         aiProcess_Triangulate      |
                         aiProcess_SortByPType;
    const struct aiScene* scene = aiImportFile(filename, flags);

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

    if(m->vao)
    {
        glDeleteVertexArrays(m->numsubmeshes, m->vao);
        free(m->vao);
        m->vao = 0;
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
    m->vao        = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->vbo        = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->indices    = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->numindices = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->numverts   = malloc(sizeof(unsigned int) * scene->mNumMeshes);
    m->drawmode   = GL_TRIANGLES;
    m->bboxmax[0] = -FLT_MAX;
    m->bboxmax[1] = -FLT_MAX;
    m->bboxmax[2] = -FLT_MAX;
    m->bboxmin[0] = FLT_MAX;
    m->bboxmin[1] = FLT_MAX;
    m->bboxmin[2] = FLT_MAX;
    glCreateVertexArrays(scene->mNumMeshes, m->vao);
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

        for(int j = 0; j < assmesh->mNumVertices; j++)
        {
            if(m->bboxmax[0] < assmesh->mVertices[j].x)
            {
                m->bboxmax[0] = assmesh->mVertices[j].x;
            }

            if(m->bboxmax[1] < assmesh->mVertices[j].y)
            {
                m->bboxmax[1] = assmesh->mVertices[j].y;
            }

            if(m->bboxmax[2] < assmesh->mVertices[j].z)
            {
                m->bboxmax[2] = assmesh->mVertices[j].z;
            }

            if(m->bboxmin[0] > assmesh->mVertices[j].x)
            {
                m->bboxmin[0] = assmesh->mVertices[j].x;
            }

            if(m->bboxmin[1] > assmesh->mVertices[j].y)
            {
                m->bboxmin[1] = assmesh->mVertices[j].y;
            }

            if(m->bboxmin[2] > assmesh->mVertices[j].z)
            {
                m->bboxmin[2] = assmesh->mVertices[j].z;
            }

            data[offset] = assmesh->mVertices[j].x;
            data[offset + 1] = assmesh->mVertices[j].y;
            data[offset + 2] = assmesh->mVertices[j].z;
            offset += 3;
        }

        if(assmesh->mNormals)
        {
            for(int j = 0; j < assmesh->mNumVertices; j++)
            {
                data[offset] = assmesh->mNormals[j].x;
                data[offset + 1] = assmesh->mNormals[j].y;
                data[offset + 2] = assmesh->mNormals[j].z;
                offset += 3;
            }
        }

        if(assmesh->mTangents)
        {
            for(int j = 0; j < assmesh->mNumVertices; j++)
            {
                data[offset] = assmesh->mTangents[j].x;
                data[offset + 1] = assmesh->mTangents[j].y;
                data[offset + 2] = assmesh->mTangents[j].z;
                offset += 3;
            }
        }

        if(assmesh->mBitangents)
        {
            for(int j = 0; j < assmesh->mNumVertices; j++)
            {
                data[offset] = assmesh->mBitangents[j].x;
                data[offset + 1] = assmesh->mBitangents[j].y;
                data[offset + 2] = assmesh->mBitangents[j].z;
                offset += 3;
            }
        }

        for(int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
        {
            if(assmesh->mTextureCoords[j])
            {
                for(int k = 0; k < assmesh->mNumVertices; k++)
                {
                    data[offset] = assmesh->mTextureCoords[j][k].x;
                    data[offset + 1] = assmesh->mTextureCoords[j][k].y;
                    offset += 2;
                }
            }
        }

        for(int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++)
        {
            if(assmesh->mColors[j])
            {
                for(int k = 0; k < assmesh->mNumVertices; k++)
                {
                    data[offset] = assmesh->mColors[j][k].r;
                    data[offset + 1] = assmesh->mColors[j][k].g;
                    data[offset + 2] = assmesh->mColors[j][k].b;
                    data[offset + 3] = assmesh->mColors[j][k].a;
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

int allocateMesh(const char* filename)
{
    unsigned int out  = 0;

    if(filename != 0)
    {
        for(int i = 0; i < nummeshes; i++)
        {
            if(!(meshes[i].cleanup & CLEAN_DELETED) && meshes[i].name != 0)
            {
                if(strcmp(filename, meshes[i].name) == 0)
                {
                    meshes[i].cleanup = CLEAN_USED;
                    printf("already loaded mesh %s id:%i\n", filename, i);
                    return i;
                }
            }
        }
    }

    if(nummeshes == 0)
    {
        meshes = malloc(sizeof(mesh));
        out = 0;
        nummeshes = 1;
    }
    else
    {
        char reuse = 0;

        for(int i = 0; i < nummeshes; i++)
        {
            if((meshes[i].cleanup & CLEAN_DELETED))
            {
                reuse = 1;
                out = i;
            }
        }

        if(reuse == 0)
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
                out = nummeshes;
                nummeshes += 1;
            }
        }
    }

    meshes[out].cleanup = CLEAN_USED;
    meshes[out].flags = 0;
    meshes[out].indices = 0;
    meshes[out].name = 0;
    meshes[out].numindices = 0;
    meshes[out].numverts = 0;
    meshes[out].vao = 0;
    meshes[out].vbo = 0;
    return out;
}

int loadMesh(const char* filename)
{
    unsigned int out  = allocateMesh(filename);
    mesh* m = &meshes[out];

    if(m->name == 0)
    {
        memset(m, 0, sizeof(mesh));
        m->name = malloc(strlen(filename) + 1);
        sprintf(m->name, "%s", filename);
        out = loadMeshfileOntoMesh(filename, out);
    }

    return out;
}

void reloadMesh(const char* filename)
{
    for(int i = 0; i < nummeshes; i++)
    {
        if(meshes[i].name != 0)
        {
            if(strcmp(filename, meshes[i].name) == 0)
            {
                printf("wants to reload:%i %s\n", i, filename);
                loadMeshfileOntoMesh(filename, i);
                break;
            }
        }
    }
}

void cleanupMesh(mesh* m)
{
    if(m == 0)
    {
        return;
    }

    if(editmesh != -1 && editmesh < nummeshes)
    {
        if(m == &meshes[editmesh])
        {
            closeMesh(); // really just closes the mesh
        }
    }

    if(m->name)
    {
        unwatchFile(m->name);
        free(m->name);
        m->name = 0;
    }

    if(m->flags)
    {
        free(m->flags);
        m->flags = 0;
    }

    if(m->indices)
    {
        glDeleteBuffers(m->numsubmeshes, m->indices);
        free(m->indices);
        m->indices = 0;
    }

    if(m->vao)
    {
        glDeleteVertexArrays(m->numsubmeshes, m->vao);
        free(m->vao);
        m->vao = 0;
    }

    if(m->vbo)
    {
        glDeleteBuffers(m->numsubmeshes, m->vbo);
        free(m->vbo);
        m->vbo = 0;
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
}

void cleanupMeshes(void)
{
    for(int i = 0; i < nummeshes; i++)
    {
        cleanupMesh(&meshes[i]);
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
    unsigned int flags = m->flags[submesh];
    unsigned int numverts = m->numverts[submesh];
    size_t offset = 0;
    size_t stride = sizeof(float) * numverts;
    glBindVertexArray(m->vao[submesh]);
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

    if(usingtessellation)
    {
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        glDrawElements(GL_PATCHES, m->numindices[submesh], GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawElements(m->drawmode, m->numindices[submesh], GL_UNSIGNED_INT, 0);
    }

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
    if(id >= nummeshes || id < 0)
    {
        return;
    }

    if(editmesh == id)
    {
        closeMesh();
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
void resetAttribs(void)
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

int ismesh(const char* filename)
{
    const char* ext = strrchr(filename, '.');

    if(ext == 0)
    {
        return 0;
    }
    else
    {
        return aiIsExtensionSupported(ext) == AI_TRUE;
    }
}

void closeMesh(void)
{
    if(editmesh == -1 || editmesh >= nummeshes)
    {
        return;
    }

    if(editmeshvbodata)
    {
        glBindBuffer(GL_ARRAY_BUFFER, meshes[editmesh].vbo[0]);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        editmeshvbodata = 0;
    }

    if(editmeshindexdata)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[editmesh].indices[0]);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        editmeshindexdata = 0;
    }

    // "unset" the editmesh
    editmesh = -1;

}

void openMesh(int id)
{
    if(editmesh != id && editmesh != -1)
    {
        closeMesh();
    }

    if(editmesh == id)
    {
        return;
    }

    if(id >= nummeshes)
    {
        fprintf(stderr, "trying to open a mesh that does not exist: id:%i maxid:%i\n", id, nummeshes - 1);
        return;
    }

    editmesh = id;
    glBindBuffer(GL_ARRAY_BUFFER, meshes[editmesh].vbo[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[editmesh].indices[0]);
    editmeshvbodata = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    editmeshindexdata = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void setMeshIndex(size_t index, const unsigned int data)
{
    editmeshindexdata[index] = data;
}

size_t flagToOffset(unsigned int flags, unsigned int targetflag, size_t stride)
{
    size_t offset = 0;
    int i = 0;

    for(; i < 4; i++)
    {
        if(flags & (1 << i))
        {
            if(targetflag & (1 << i))
            {
                return offset;
            }

            offset += stride * 3;
        }
    }

    for(; i < 12; i++)
    {
        if(flags & (1 << i))
        {
            if(targetflag & (1 << i))
            {
                return offset;
            }

            offset += stride * 2;
        }
    }

    for(; i < 20; i++)
    {
        if(flags & (1 << i))
        {
            if(targetflag & (1 << i))
            {
                return offset;
            }

            offset += stride * 3;
        }
    }

    return ~0;
}

size_t flagToSize(unsigned int flag)
{
    switch(flag)
    {
        case MESH_FLAG_POSITION:
        case MESH_FLAG_NORMAL:
        case MESH_FLAG_TANGENT:
        case MESH_FLAG_BINORMAL:
            return 3;
            break;

        case MESH_FLAG_TEXCOORD0:
        case MESH_FLAG_TEXCOORD1:
        case MESH_FLAG_TEXCOORD2:
        case MESH_FLAG_TEXCOORD3:
        case MESH_FLAG_TEXCOORD4:
        case MESH_FLAG_TEXCOORD5:
        case MESH_FLAG_TEXCOORD6:
        case MESH_FLAG_TEXCOORD7:
            return 2;
            break;

        case MESH_FLAG_COLOR0:
        case MESH_FLAG_COLOR1:
        case MESH_FLAG_COLOR2:
        case MESH_FLAG_COLOR3:
        case MESH_FLAG_COLOR4:
        case MESH_FLAG_COLOR5:
        case MESH_FLAG_COLOR6:
        case MESH_FLAG_COLOR7:
            return 3;
            break;
    }

    return 0;
}

void setMeshVertexData(size_t index, float data)
{
    editmeshvbodata[index] = data;
}
