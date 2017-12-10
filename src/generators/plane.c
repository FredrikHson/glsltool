#include "generators.h"
#include "generatorparams.h"
#include <stdio.h>
#include <stdlib.h>
#include "opengl.h"
#include "defines.h"

int generatePlane(unsigned int subw, unsigned int subh, float w, float h)
{
    printf("starting to generate an amazing plane!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
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
        unsigned int size = 14 * sizeof(float);
        unsigned int numverts = (subw + 2) * (subh + 2);
        float* data = (float*)malloc(numverts * size);
        float dw = w / (subw + 1);
        float dh = h / (subh + 1);
        float* d = data;

        for(float i = 0; i < subh + 2; i += 1.0f)
        {
            float y = dh * i;

            for(unsigned int j = 0; j < subw + 2; j++)
            {
                float x = dw * j;
                float u = j / (float)(subw + 1);
                float v = i / (float)(subh + 1);
                printf("x:%f y:%f z:%f u:%f v:%f\n", x, y, 0.0f, u, v);
                // pos
                d[0] = x;
                d[1] = y;
                d[2] = 0.0f;
                // normals
                d[3] = 0.0f;
                d[4] = 0.0f;
                d[5] = 1.0f;
                // tangent
                d[6] = 1.0f;
                d[7] = 0.0f;
                d[8] = 0.0f;
                // binormal
                d[9] = 0.0f;
                d[10] = 1.0f;
                d[11] = 0.0f;
                // texcoord
                d[12] = u;
                d[13] = v;
                d += size;
            }
        }

        m->flags      = malloc(sizeof(unsigned int));
        m->vbo        = malloc(sizeof(unsigned int));
        m->indices    = malloc(sizeof(unsigned int));
        m->numindices = malloc(sizeof(unsigned int));
        m->numverts   = malloc(sizeof(unsigned int));
        m->numverts[0] = numverts;
        m->flags[0] = MESH_FLAG_POSITION;
        m->flags[0] |= MESH_FLAG_NORMAL;
        m->flags[0] |= MESH_FLAG_TANGENT;
        m->flags[0] |= MESH_FLAG_BINORMAL;
        m->flags[0] |= MESH_FLAG_TEXCOORD0;
        glGenBuffers(1, m->vbo);
        glGenBuffers(1, m->indices);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, size * numverts, data, GL_STATIC_DRAW);
        free(data);
    }

    printf("done %u generating an amazing plane!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", out);
    return out;
}
