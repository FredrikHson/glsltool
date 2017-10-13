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
    unsigned int* numindices;
    unsigned int* numverts;
} mesh;

typedef struct shader
{
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
    char working;
} shader;

int loadImage(const char* filename);
int loadMesh(const char* filename);
int loadShader(const char* vertex, const char* pixel, const char* geometry, const char* tesscontrol, const char* tesseval);
void reloadImage(const char* filename);
void reloadMesh(const char* filename);
void reloadShader(const char* filename);
void cleanupImages();
void cleanupMeshes();

#endif //__RESOURCES_H__
