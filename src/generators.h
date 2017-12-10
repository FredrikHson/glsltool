#ifndef __GENERATORS_H__
#define __GENERATORS_H__

#include "resources.h"
#include "vector.h"

typedef enum gentype
{
    GEN_PLANE,
    GEN_CUBE
} gentype;

typedef struct generatedmesh
{
    unsigned int meshid;
    gentype type;
    char cleanup;
    void* params; // pointer to a struct of x type
} generatedmesh;

extern generatedmesh* gen_meshes;
extern unsigned int numgen_meshes;
extern mesh* meshes;
extern int nummeshes;

void startGeneratorCleanup();
void endGeneratorCleanup();
unsigned int allocateGenMesh(gentype t, void* params);

int generatePlane(unsigned int subw, unsigned int subh, float w, float h);

#endif //__GENERATORS_H__
