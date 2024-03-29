#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <stddef.h>

#define CLEAN_USED    0
#define CLEAN_DELETED 1
#define CLEAN_LATER   2

typedef struct image
{
    char cleanup;
    char* name;
    unsigned int glImage;
    int width;
    int height;
    int channels; /*1-4*/
    char origin;
} image;

typedef struct mesh
{
    char cleanup;
    char* name;
    unsigned int numsubmeshes;
    unsigned int* indices;
    unsigned int* vao;
    unsigned int* vbo;
    unsigned int* flags;
    unsigned int* numindices;
    unsigned int* numverts;
    float bboxmin[3];
    float bboxmax[3];
    unsigned int drawmode;
} mesh;

typedef struct shader
{
    char cleanup;
    char* controlname;
    char* evalname;
    char* fragname;
    char* geomname;
    char* vertname;
    unsigned int control;
    unsigned int eval;
    unsigned int frag;
    unsigned int geom;
    unsigned int vert;
    unsigned int program;
    char working;
} shader;

int loadImage(const char* filename, char origin);
int loadMesh(const char* filename);
int loadShader(const char* vertex, const char* pixel, const char* geometry, const char* tesscontrol, const char* tesseval);
void reloadImage(const char* filename);
void reloadMesh(const char* filename);
void reloadShader(const char* filename);
void cleanupImage(image* img);
void cleanupImages(void);
void cleanupMeshes(void);
void cleanupMesh(mesh* m);
void cleanupShaders(void);
void cleanupShader(shader* s);
void initImages(void);
int allocateMesh(const char* filename);
int getImageWidth(int id);
int getImageHeight(int id);
int ismesh(const char* filename);
void closeMesh(void);
void openMesh(int id);
void setMeshIndex(size_t index, const unsigned int data);
void setMeshVertexData(size_t index, float data);

void initResourceCleanup(void);
void endResourceCleanup(void);
unsigned int sizeOfVert(unsigned int flags);

#endif //__RESOURCES_H__
