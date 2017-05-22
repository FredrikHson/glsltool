#include "notify.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <stdio.h>

int inotify = 0;

typedef struct inotify_event inotify_event;
#define EVENT_SIZE (sizeof(inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

int initFileWatcher()
{
    inotify = inotify_init1(IN_NONBLOCK);

    if(inotify == -1)
    {
        return 0;
    }

    return 1;
}
void watchFile(const char* filename)
{
    int watch = inotify_add_watch(inotify, filename, IN_CLOSE_WRITE | IN_MOVED_TO);
    printf("watching %s for changes descriptor:%i\n", filename, watch);
}
void watchChanges()
{
    char buffer[BUF_LEN] = {0};
    int length = 0;
    int i = 0;

    do
    {
        i = 0;
        length = read(inotify, buffer, BUF_LEN);

        if(length <= 0) // war never changes
        {
            break;
        }

        do
        {
            inotify_event* event = (inotify_event*)&buffer[i];
            printf("watch changed descriptor:%i %s\n", event->wd, event->name);
            i += EVENT_SIZE + event->len;
        }
        while(i < length);
    }
    while(1);
}
