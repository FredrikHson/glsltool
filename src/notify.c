#include "notify.h"
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int inotify = 0;

typedef struct inotify_event inotify_event;
#define EVENT_SIZE (sizeof(inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

typedef struct notify_file
{
    int descriptor;
    int inode;
    char filename[4096];
    void (*callback)(const char*);
} notify_file;

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
    for(int i = 0; i < numWatchedFiles; i++)
    {
        if(strcmp(watchlist[i].filename, filename) == 0 && watchlist[i].callback == callback)
        {
            return;
        }
    }

    int watch = inotify_add_watch(inotify, filename, IN_CLOSE_WRITE | IN_MOVE);
    printf("watching %s for changes\n", filename);

    if(numWatchedFiles == 0)
    {
        watchlist = malloc(sizeof(notify_file));
    }
    else
    {
        void* newlist = realloc(watchlist, sizeof(notify_file) * (numWatchedFiles + 1));

        if(newlist == 0)
        {
            fprintf(stderr, "out of memory allocating the watch for %s\n", filename);
            exit(1);
        }
        else
        {
            watchlist = newlist;
        }
    }

    strncpy(watchlist[numWatchedFiles].filename, filename, 4096);
    watchlist[numWatchedFiles].descriptor = watch;
    watchlist[numWatchedFiles].callback = callback;
    struct stat filestat;
    stat(filename, &filestat);
    watchlist[numWatchedFiles].inode = filestat.st_ino;
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
            void* newlist   = realloc(watchlist, sizeof(notify_file) * numWatchedFiles);

            if(newlist == 0)
            {
                fprintf(stderr, "out of memory allocating the watch for %s\n", filename);
                exit(1);
            }
            else
            {
                watchlist = newlist;
            }
        }
    }
}
void watchChanges()
{
    char buffer[BUF_LEN] = {0};

    do
    {
        int length = 0;
        int i = 0;
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
                    printf("watch changed descriptor:%i %s %p\n", event->wd, watchlist[i].filename, (void*)watchlist[i].callback);

                    if(watchlist[i].callback != 0)
                    {
                        usleep(50000); /* add a small delay to make sure the file is done writing */
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

    /* maybe not recommended with large amounts of files but probably better than just relying on inotify
     * since inotify does not detect when you write to temporary file then move to the original location  */
    for(int i = 0; i < numWatchedFiles; i++)
    {
        struct stat filestat;
        stat(watchlist[i].filename, &filestat);

        if(filestat.st_ino != watchlist[i].inode) /* the inode has changed so the file must have changed */
        {
            inotify_rm_watch(inotify, watchlist[i].descriptor);
            watchlist[i].inode = filestat.st_ino;
            watchlist[i].descriptor = inotify_add_watch(inotify, watchlist[i].filename, IN_CLOSE_WRITE | IN_MOVE);
            usleep(50000);
            (*watchlist[i].callback)(watchlist[i].filename);
        }
    }
}
