#include "resources.h"
#include "generators.h"
#include <stdio.h>

extern mesh* meshes;
extern int nummeshes;
extern image* textures;
extern int numtextures;

void initResourceCleanup()
{
    for(int i = 0; i < nummeshes; i++)
    {
        meshes[i].cleanup |= CLEAN_LATER;
    }

    for(int i = 0; i < numtextures; i++)
    {
        textures[i].cleanup |= CLEAN_LATER;
    }
}

void endResourceCleanup()
{
    for(int i = 0; i < nummeshes; i++)
    {
        mesh* m = &meshes[i];

        if(m->cleanup & CLEAN_LATER)
        {
            if(!(m->cleanup & CLEAN_DELETED))
            {
                m->cleanup = CLEAN_DELETED;

                if(meshes[i].name != 0)
                {
                    printf("deleting %s\n", meshes[i].name);
                }
                else
                {
                    printf("deleting nameless mesh %i\n", i);
                }

                cleanupMesh(m);
            }
        }
    }

    for(int i = 0; i < numtextures; i++)
    {
        image* img = &textures[i];

        if(img->cleanup & CLEAN_LATER)
        {
            if(!(img->cleanup & CLEAN_DELETED))
            {
                printf("deleting %s\n", textures[i].name);
                cleanupImage(img);
            }
        }
    }

}
