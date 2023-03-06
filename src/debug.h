#ifndef __DEBUG_H__
#define __DEBUG_H__

enum debugmodes
{
    DEBUG_OFF,
    DEBUG_RENDERALLSTEPS,
    DEBUG_RENDERSINGLESTEP,
    DEBUG_LAST
};

void drawRenderTargets(void);
void copyTargetToDebug(unsigned int id);
char setDebugMode(char mode);
void setSingleDebugStep(int step);
void cleanupDebug(void);
void initDebug(void);

extern char debugmode;
extern float debug_maxvalue;
extern float debug_minvalue;
extern char debug_clip;

#endif //__DEBUG_H__
