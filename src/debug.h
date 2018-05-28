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
char setDebugMode(char mode);
void setSingleDebugStep(int step);
void cleanupDebug();
void initDebug();

extern char debugmode;

#endif //__DEBUG_H__
