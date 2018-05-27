#ifndef __DEBUG_H__
#define __DEBUG_H__

enum debugmodes
{
    DEBUG_OFF,
    DEBUG_RENDERALLSTEPS,
    DEBUG_RENDERSINGLESTEP,
    DEBUG_LAST
};

void drawRenderTargets();
void copyTargetToDebug(unsigned int textureid);
void setDebugMode(char mode);
void cleanupDebug();
void initDebug();

#endif //__DEBUG_H__
