#ifndef __RESOURCES_H__
#define __RESOURCES_H__

typedef struct image
{
    char* name;
    unsigned int glImage;
    int width;
    int height;
    int channels; /*1-4*/
} image;

typedef struct mesh
{
    char* name;
    unsigned int numsubmeshes;
    unsigned int* indices;
    unsigned int* vbo;
    unsigned int* flags;
} mesh;

int loadImage(const char* filename);
int loadMesh(const char* filename);
void reloadImage(const char* filename);
void cleanupImages();
void cleanupMeshes();

#endif //__RESOURCES_H__
