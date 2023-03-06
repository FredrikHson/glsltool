#ifndef __SCRIPT_H__
#define __SCRIPT_H__

int initScript(const char* filename);
void run_loop(void);
void run_filechange(char* filename);
void run_droppedfiles(const char** filename, int count);
void run_resize(void);
int shutdownScript(void);
void reloadScript(const char* filename);
extern unsigned char validscript;

#endif //__SCRIPT_H__
