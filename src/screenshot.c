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
    char* imagedata = malloc(w * h * 4);

    if(id == -1)
    {
        glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, imagedata);
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
