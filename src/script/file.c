#include "script.h"
#include "notify.h"
#include "v7.h"
#include <string.h>
#include <strings.h>

typedef struct v7 v7;
extern v7* v7g;
enum v7_err js_watchfile(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(argc == 1)
    {
        size_t len = 0;
        v7_val_t val = v7_arg(v7e, 0);
        const char* filename = v7_get_string(v7e, &val, &len);
        const char* ext = strrchr(filename, '.');

        if(strcasecmp(".js", ext) == 0)
        {
            watchFile(filename, reloadScript);
        }
        else
        {
            watchFile(filename, 0);
        }
    }
    else
    {
        fprintf(stderr, "watchfile needs a filename as an argument\n");
        return V7_SYNTAX_ERROR;
    }

    return V7_OK;
}
