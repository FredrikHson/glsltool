#include "generators.h"
#include "generatorparams.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opengl.h"
#include "defines.h"

int generateGenericMesh(unsigned int numverts, unsigned int numelements, unsigned int drawmode, unsigned int flags, const char* identifier)
{
    genericmesh_params p;
    p.drawmode = drawmode;
    p.flags = flags;
    p.numelements = numelements;
    p.numverts = numverts;
    fprintf(stderr, "identifier:%s|\n", identifier);
    SHA256((unsigned char*)identifier, strlen(identifier), p.sha256);
    unsigned int out = allocateGenMesh(GEN_MESH, &p);
    printf("generatedid:%i\n", out);

    printf("sha256: ");

    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%x", p.sha256[i]);
    }

    printf(" identifier:%s \n", identifier);

    /*fwrite(p.sha256, 32, 1, stdout);*/

    if(gen_meshes[out].meshid == ~0)
    {
        printf("\n");
        gen_meshes[out].meshid = allocateMesh(0);

        mesh* m = &meshes[gen_meshes[out].meshid];
        m->name = malloc(strlen(identifier) + 13);
        sprintf(m->name, "genericmesh_%s", identifier);
        m->numsubmeshes  = 1;
        m->flags = malloc(sizeof(unsigned int));
        m->vbo = malloc(sizeof(unsigned int));
        m->vao = malloc(sizeof(unsigned int));
        m->indices = malloc(sizeof(unsigned int));
        m->numindices = malloc(sizeof(unsigned int));
        m->numverts = malloc(sizeof(unsigned int));

        m->numverts[0] = numverts;
        m->numindices[0] = numelements;
        m->drawmode = drawmode;
        m->flags[0] = flags;

        float* data = (float*)malloc(numverts * sizeOfVert(flags));
        unsigned int* idata = malloc(sizeof(unsigned int) * m->numindices[0]);
        memset(data, 0, numverts * sizeOfVert(flags));
        memset(idata, 0, sizeof(unsigned int) * m->numindices[0]);

        glCreateVertexArrays(1, m->vao);
        glGenBuffers(1, m->vbo);
        glGenBuffers(1, m->indices);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, numverts * sizeOfVert(flags), data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indices[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m->numindices[0], idata, GL_DYNAMIC_DRAW);

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
