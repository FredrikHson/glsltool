#include "generators.h"
#include "resources.h"
#include "generatorparams.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

generatedmesh* gen_meshes = 0;
unsigned int numgen_meshes = 0;

void startGeneratorCleanup()
{
    for(int i = 0; i < numgen_meshes; i++)
    {
        gen_meshes[i].cleanup = CLEAN_LATER;
    }
}

void endGeneratorCleanup()
{
    for(int i = 0; i < numgen_meshes; i++)
    {
        generatedmesh* m = &gen_meshes[i];

        if(m->cleanup & CLEAN_LATER)
        {
            if(!(m->cleanup & CLEAN_DELETED))
            {
                m->cleanup = CLEAN_DELETED;

                if(m->params)
                {
                    free(m->params);
                    m->params = 0;
                }

                printf("deleting generated mesh %i\n", i);
            }
        }
    }
}

unsigned char compareparams(gentype t, void* p1, void* p2)
{
    switch(t)
    {
        case GEN_PLANE:
        {
            plane_params* plane1 = (plane_params*)p1;
            plane_params* plane2 = (plane_params*)p2;

            if(plane1->w == plane2->w && plane1->h == plane2->h &&
               plane1->subw == plane2->subw && plane1->subh == plane2->subh)
            {
                return 1;
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return 0;
}

unsigned int allocateGenMesh(gentype t, void* params)
{
    unsigned int out = 0;

    for(unsigned int i = 0; i < numgen_meshes; i++)
    {
        if(gen_meshes[i].type == t)
        {
            if(compareparams(t, params, gen_meshes[i].params))
            {
                gen_meshes[i].cleanup = CLEAN_USED;
                return i;
            }
        }
    }

    if(numgen_meshes == 0)
    {
        gen_meshes = (generatedmesh*)malloc(sizeof(generatedmesh));
        numgen_meshes = 1;
        out = 0;
    }
    else
    {
        char reuse = 0;

        for(int i = 0; i < numgen_meshes; i++)
        {
            if((gen_meshes[i].cleanup & CLEAN_DELETED))
            {
                reuse = 1;
                out = i;
            }
        }

        if(reuse == 0)
        {
            void* newgenmeshes = realloc(gen_meshes, sizeof(generatedmesh) * (numgen_meshes + 1));

            if(newgenmeshes == 0)
            {
                fprintf(stderr, "out of memory while creating generatedmesh\n");
                exit(1);
            }
            else
            {
                gen_meshes = newgenmeshes;
                out = numgen_meshes;
                numgen_meshes += 1;
            }
        }
    }

    switch(t)
    {
        case GEN_PLANE:
        {
            gen_meshes[out].params = malloc(sizeof(plane_params));
            memcpy(gen_meshes[out].params, params, sizeof(plane_params));
            break;
        }

        default:
            break;
    }

    gen_meshes[out].meshid = ~0;  // if its not ~0 ignore the generation code
    return out;
}
