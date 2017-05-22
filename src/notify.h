#ifndef __NOTIFY_H__
#define __NOTIFY_H__

int initFileWatcher();
void watchFile(const char* filename); // TODO:add callback?
void watchChanges();

#endif //__NOTIFY_H__
