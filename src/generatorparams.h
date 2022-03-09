#ifndef __GENERATORPARAMS_H__
#define __GENERATORPARAMS_H__
#include <openssl/sha.h>

typedef struct plane_params
{
    unsigned int subw;
    unsigned int subh;
    float w;
    float h;
} plane_params;

typedef struct genericmesh_params
{
    unsigned int drawmode;
    unsigned int flags;
    unsigned int numelements;
    unsigned int numverts;
    unsigned char sha256[SHA256_DIGEST_LENGTH];
} genericmesh_params;

#endif //__GENERATORPARAMS_H__
