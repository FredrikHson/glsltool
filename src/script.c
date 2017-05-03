#include "script.h"
#include <stdio.h>
#include "v7.h"

unsigned char validscript = 0;
struct v7* context = 0;
int initScript(const char* filename)
{
    context = v7_create();

    v7_val_t result;
    enum v7_err rcode = v7_exec_file(context, filename, &result);

    if(rcode != V7_OK)
    {
        v7_print_error(stderr, context, "Error", result);

    }

    run_loop();
    return 1;
}

void run_loop()
{
    v7_val_t function;
    v7_val_t result;
    function = v7_get(context, v7_get_global(context), "loop", 4);
    enum v7_err rcode = v7_apply(context, function, V7_UNDEFINED, V7_UNDEFINED, &result);

    if(rcode != V7_OK)
    {
        v7_print_error(stderr, context, "Error", result);

    }
}

int shutdownScript()
{
    v7_destroy(context);

    return 1;
}
