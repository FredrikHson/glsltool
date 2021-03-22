#include "opengl.h"
#include "renderfunc.h"
#include "options.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <stdlib.h>

extern rendertarget* rendertargets;
extern unsigned int numrendertargets;
/* TODO: write the render target part and add it to scripts */
void saveRenderTarget(int id, int layer, const char* filename)
{
    fprintf(stderr, "saving backbuffer to %s\n", filename);
    ILuint imageid = 0;
    int w, h;

    if(id == -1)
    {
        w = options.width;
        h = options.height;
    }
    else
    {
        if(id < numrendertargets)
        {
            w = rendertargets[id].width;
            h = rendertargets[id].height;
        }
        else
        {
            return;
        }
    }

    ilGenImages(1, &imageid);
    ilBindImage(imageid);
    unsigned char* imagedata = malloc(w * h * 4);
    glFinish();

    if(id == -1)
    {
        glReadBuffer(GL_BACK);
        glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, imagedata);
    }

    if(options.supersample)
    {
        // manually resize the image since iluScale refuses to do the pixel average
        unsigned char* smallimagedata = malloc(w * h);
        size_t pixeloffset = 0;
        int newsize_w = w / 2;
        int newsize_h = h / 2;

        for(int i = 0; i < newsize_h; i++)
        {
            size_t largeoffset = i * 2 * w * 4;

            for(int j = 0; j < newsize_w; j++)
            {
                for(int k = 0; k < 4; k++)
                {
                    int pixel = 0;
                    pixel += imagedata[largeoffset + j * 2 * 4 + k];
                    pixel += imagedata[largeoffset + j * 2 * 4 + 4 + k];
                    pixel += imagedata[largeoffset + w * 4 + j * 2 * 4 + k];
                    pixel += imagedata[largeoffset + w * 4 + j * 2 * 4 + 4 + k];
                    smallimagedata[pixeloffset] = (char)(pixel / 4)  ;
                    pixeloffset++;
                }
            }
        }

        w /= 2;
        h /= 2;
        free(imagedata);
        imagedata = smallimagedata;
    }

    ilTexImage(w, h, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, imagedata);
    ilEnable(IL_FILE_OVERWRITE);
    ilSave(IL_PNG, filename);
    ilDeleteImages(1, &imageid);

    if(imagedata)
    {
        free(imagedata);
    }
}
