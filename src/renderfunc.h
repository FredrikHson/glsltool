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

typedef struct vertattribute
{
    int flag;  // MESH_FLAGS
    char name[256];
} vertattribute;

unsigned int CreateRenderTarget(unsigned int width,
                                unsigned int height,
                                unsigned int layers,
                                unsigned int format,
                                unsigned int type,
                                unsigned int magfilter,
                                unsigned int minfilter);

void clear(float red, float green, float blue, float alpha);
void cleanupRender();
void beginPass(int target, int* width, int* height);
void endPass();

void drawMesh(int id, int submesh);
int bindAttrib(const char* name, int flag);
void resetAttribs();
void bindShader(int shader);
void setUniformf(const char* name, float* f, int argc);
void setUniformi(const char* name, int* f, int argc);
void setUniformui(const char* name, unsigned int* f, int argc);

#endif //__RENDERFUNC_H__
