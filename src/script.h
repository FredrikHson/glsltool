#ifndef __SCRIPT_H__
#define __SCRIPT_H__

int initScript(const char* filename);
void run_loop();
void run_filechange(char* filename);
void run_resize();
int shutdownScript();
void reloadScript(const char* filename);
extern unsigned char validscript;

#endif //__SCRIPT_H__
