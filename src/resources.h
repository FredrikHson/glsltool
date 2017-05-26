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

int loadImage(const char* filename);
int loadMesh(const char* filename);
void reloadImage(const char* filename);
void cleanupImages();

#endif //__RESOURCES_H__
