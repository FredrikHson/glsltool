#ifndef __NOTIFY_H__
#define __NOTIFY_H__

int initFileWatcher(void);
void destroyFileWatcher(void);
void watchFile(const char* filename, void (*callback)(const char*));
void unwatchFile(const char* filename);
void watchChanges(void);

#endif //__NOTIFY_H__
