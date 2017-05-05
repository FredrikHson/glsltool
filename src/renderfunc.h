#ifndef __RENDERFUNC_H__
#define __RENDERFUNC_H__

struct rendertarget
{
    unsigned int width;
    unsigned int height;
    unsigned int layers;
    unsigned int format;
    unsigned int type;
    unsigned int buffer;
    unsigned int* textures;
    unsigned int depth;
};

unsigned int CreateRenderTarget(unsigned int width,
                                unsigned int height,
                                unsigned int layers,
                                unsigned int format,
                                unsigned int type,
                                unsigned int magfilter,
                                unsigned int minfilter);
void cleanRender();

#endif //__RENDERFUNC_H__
