#ifndef __RENDERFUNC_H__
#define __RENDERFUNC_H__

extern struct GLFWwindow* window;

typedef struct rendertarget
{
    unsigned int width;
    unsigned int height;
    unsigned int layers;
    unsigned int format;
    unsigned int type;
    unsigned int buffer;
    unsigned int* textures;
    unsigned int depth;
} rendertarget;


unsigned int CreateRenderTarget(unsigned int width,
                                unsigned int height,
                                unsigned int layers,
                                unsigned int format,
                                unsigned int type,
                                unsigned int magfilter,
                                unsigned int minfilter);

void clear(float red, float green, float blue, float alpha);
void cleanupRender();
void beginPass(int target);
void endPass();

void drawMesh(int id, int submesh);

#endif //__RENDERFUNC_H__
