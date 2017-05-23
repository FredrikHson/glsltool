#include "notify.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int inotify = 0;

typedef struct inotify_event inotify_event;
#define EVENT_SIZE (sizeof(inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

struct notify_file
{
    int descriptor;
    char filename[4096];
    void (*callback)(const char*);
};

typedef struct notify_file notify_file;

notify_file* watchlist = 0;
size_t numWatchedFiles = 0;

int initFileWatcher()
{
    inotify = inotify_init1(IN_NONBLOCK);

    if(inotify == -1)
    {
        return 0;
    }

    return 1;
}
void destroyFileWatcher()
{
    for(int i = 0; i < numWatchedFiles; i++)
    {
        inotify_rm_watch(inotify, watchlist[i].descriptor);
    }

    close(inotify);

    if(watchlist)
    {
        free(watchlist);
        watchlist = 0;
    }

    numWatchedFiles = 0;
}

void watchFile(const char* filename, void (*callback)(const char*))
{
    int watch = inotify_add_watch(inotify, filename, IN_CLOSE_WRITE | IN_MOVED_TO);
    printf("watching %s for changes descriptor:%i\n", filename, watch);

    if(numWatchedFiles == 0)
    {
        watchlist = malloc(sizeof(notify_file));
    }
    else
    {
        watchlist = realloc(watchlist, sizeof(notify_file) * (numWatchedFiles + 1));
    }

    strncpy(watchlist[numWatchedFiles].filename, filename, 4096);
    watchlist[numWatchedFiles].descriptor = watch;
    watchlist[numWatchedFiles].callback = callback;
    numWatchedFiles += 1;
}
void unwatchFile(const char* filename)
{
    for(int i = 0; i < numWatchedFiles; i++)
    {
        if(strcmp(filename, watchlist[i].filename) == 0)
        {
            inotify_rm_watch(inotify, watchlist[i].descriptor);
            numWatchedFiles -= 1;
            watchlist[i].descriptor = watchlist[numWatchedFiles].descriptor;
            strncpy(watchlist[i].filename, watchlist[numWatchedFiles].filename, 4096);
            watchlist = realloc(watchlist, sizeof(notify_file) * numWatchedFiles);
        }
    }
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

            for(int i = 0; i < numWatchedFiles; i++)
            {
                if(watchlist[i].descriptor == event->wd)
                {
                    printf("watch changed descriptor:%i %s %p\n", event->wd, watchlist[i].filename, watchlist[i].callback);

                    if(watchlist[i].callback != 0)
                    {
                        (*watchlist[i].callback)(watchlist[i].filename);
                    }

                    break;
                }
            }

            i += EVENT_SIZE + event->len;
        }
        while(i < length);
    }
    while(1);
}
