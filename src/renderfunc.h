#ifndef __RENDERFUNC_H__
#define __RENDERFUNC_H__

extern struct GLFWwindow* window;

typedef struct rendertarget
{
    int width;
    int height;
    unsigned int layers;
    unsigned int format;
    int type;
    unsigned int buffer;
    unsigned int* textures;
    unsigned int depth;
    char relative;
} rendertarget;

typedef struct vertattribute
{
    int flag;  // MESH_FLAGS
    char name[256];
} vertattribute;
extern int currentPassTarget;

unsigned int CreateRenderTarget(int width,
                                int height,
                                unsigned int layers,
                                unsigned int format,
                                int type,
                                char relative);
void resizeTargets(void);

void clear(float red, float green, float blue, float alpha, char color, char depth, int attachment);
void cleanupRender(void);
void beginPass(int target, int* width, int* height);
void endPass(void);

void drawMesh(int id, int submesh);
int bindAttrib(const char* name, int flag);
void resetAttribs(void);
void bindShader(int id);
void setUniformf(const char* name, float* f, unsigned long argc);
void setUniformd(const char* name, double* f, unsigned long argc);
void setUniformi(const char* name, int* f, unsigned long argc);
void setUniformui(const char* name, unsigned int* f, unsigned long argc);
void bindTexture(const char* name, int id,
                 int magfilter, int minfilter,
                 int clamp_s, int clamp_t);
void bindRendertarget(const char* name, int id, int layer,
                      int magfilter, int minfilter,
                      int clamp_s, int clamp_t);
void resetTexturebindings(void);
void bindFragDataLocation(int shaderid, const char* name, int rtid, int layer);

void saveRenderTarget(int id, int layer, const char* filename);

#endif //__RENDERFUNC_H__
