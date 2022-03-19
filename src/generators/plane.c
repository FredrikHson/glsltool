#include "generators.h"
#include "generatorparams.h"
#include <stdio.h>
#include <stdlib.h>
#include "opengl.h"
#include "defines.h"

int generatePlane(unsigned int subw, unsigned int subh, float w, float h)
{
    plane_params p;
    p.w = w;
    p.h = h;
    p.subw = subw;
    p.subh = subh;
    unsigned int out = allocateGenMesh(GEN_PLANE, &p);

    if(gen_meshes[out].meshid == ~0)
    {
        printf("\n");
        gen_meshes[out].meshid = allocateMesh(0);
        mesh* m = &meshes[gen_meshes[out].meshid];
        m->name = malloc(6);
        sprintf(m->name, "Plane");
        m->numsubmeshes = 1;
        m->flags        = malloc(sizeof(unsigned int));
        m->vbo          = malloc(sizeof(unsigned int));
        m->vao          = malloc(sizeof(unsigned int));
        m->indices      = malloc(sizeof(unsigned int));
        m->numindices   = malloc(sizeof(unsigned int));
        m->numverts     = malloc(sizeof(unsigned int));
        m->flags[0] = MESH_FLAG_POSITION |
                      MESH_FLAG_NORMAL |
                      MESH_FLAG_TANGENT |
                      MESH_FLAG_BINORMAL |
                      MESH_FLAG_TEXCOORD0;
        m->cleanup = CLEAN_USED;
        size_t vlen = sizeOfVert(m->flags[0]);
        unsigned int numverts = (subw + 2) * (subh + 2);
        m->numverts[0]   = numverts;
        float* data = (float*)malloc(numverts * vlen * sizeof(float));
        float dw = w / (subw + 1);
        float dh = h / (subh + 1);
        size_t noffset = numverts * 3;
        size_t toffset = noffset + numverts * 3;
        size_t boffset = toffset + numverts * 3;
        size_t uoffset = boffset + numverts * 3;
        size_t offset3 = 0;
        size_t offset2 = 0;

        for(float i = 0; i < subh + 2; i += 1.0f)
        {
            float y = (dh * i)  - h / 2.0f;

            for(float j = 0; j < subw + 2; j += 1.0f)
            {
                float x = (dw * j) - (w / 2.0f);
                float u = j / (float)(subw + 1);
                float v = i / (float)(subh + 1);
                // pos
                data[offset3] = x;
                data[offset3 + 1] = y;
                data[offset3 + 2] = 0.0f;
                // normals
                data[noffset + offset3] = 0.0f;
                data[noffset + offset3 + 1] = 0.0f;
                data[noffset + offset3 + 2] = 1.0f;
                // tangent
                data[toffset + offset3] = 1.0f;
                data[toffset + offset3 + 1] = 0.0f;
                data[toffset + offset3 + 2] = 0.0f;
                // binormal
                data[boffset + offset3] = 0.0f;
                data[boffset + offset3 + 1] = 1.0f;
                data[boffset + offset3 + 2] = 0.0f;
                // texcoordata
                data[uoffset + offset2] = u;
                data[uoffset + offset2 + 1] = v;
                offset3 += 3;
                offset2 += 2;
            }
        }

        m->numindices[0] = (subw + 1) * (subh + 1) * 6;
        m->drawmode = GL_TRIANGLES;
        glCreateVertexArrays(1, m->vao);
        glGenBuffers(1, m->vbo);
        glGenBuffers(1, m->indices);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vlen * sizeof(float)* numverts, data, GL_STATIC_DRAW);
        unsigned int* idata = malloc(sizeof(unsigned int) * m->numindices[0]);
        unsigned int* id = idata;

        for(unsigned int i = 0; i < subh + 1; i++)
        {
            unsigned int offset = i * (subw + 2);

            for(unsigned int j = 0; j < subw + 1; j++)
            {
                id[0] = offset + j;
                id[1] = offset + j + 1;
                id[2] = offset + j + (subw + 2);
                id[3] = offset + j + 1;
                id[4] = offset + j + (subw + 2) + 1;
                id[5] = offset + j + (subw + 2);
                id += 6;
            }
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indices[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m->numindices[0], idata, GL_STATIC_DRAW);

        if(idata != 0)
        {
            free(idata);
        }

        if(data != 0)
        {
            free(data);
        }
    }

    meshes[gen_meshes[out].meshid].cleanup = CLEAN_USED;
    return gen_meshes[out].meshid;
}
