#ifndef __NOTIFY_H__
#define __NOTIFY_H__

int initFileWatcher();
void destroyFileWatcher();
void watchFile(const char* filename); // TODO:add callback?
void unwatchFile(const char* filename);
void watchChanges();

#endif //__NOTIFY_H__
