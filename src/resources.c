#include "resources.h"
#include "generators.h"
#include <stdio.h>

extern mesh* meshes;
extern int nummeshes;

void initResourceCleanup()
{
    for(int i = 0; i < nummeshes; i++)
    {
        meshes[i].cleanup |= CLEAN_LATER;
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
                if(meshes[i].name!=0)
                printf("deleting %s\n", meshes[i].name);
                else
                printf("deleting generated mesh\n");
                cleanupMesh(m);
            }
        }
    }
}
