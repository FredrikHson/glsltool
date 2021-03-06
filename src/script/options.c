#include "script.h"
#include "v7.h"
#include <stdio.h>
#include "options.h"
#include <string.h>

typedef struct v7 v7;
extern v7* v7g;

enum v7_err js_get_optional_string(v7* v7e, v7_val_t* res)
{
    int argc = v7_argc(v7e);

    if(v7_argc(v7e) != 2)
    {
        fprintf(stderr, "invalid number of arguments to get_optional_string\n");
        return V7_SYNTAX_ERROR;
    }

    if(argc == 2)
    {
        size_t len = 0;
        v7_val_t arg1 = v7_arg(v7e, 0);
        const char* name = v7_get_string(v7e, &arg1, &len);
        size_t len2 = 0;
        v7_val_t arg2 = v7_arg(v7e, 1);
        const char* def = v7_get_string(v7e, &arg2, &len2);

        for(int i = 0; i < options.numoptional_variables; i++)
        {
            if(strcmp(name, options.variables[i].name) == 0)
            {
                *res = v7_mk_string(v7e, options.variables[i].value, ~0, 0);
                fprintf(stderr, "returning string %s\n", options.variables[i].value);
                return V7_OK;
            }
        }

        *res = v7_mk_string(v7e, def, ~0, 0);
    }

    return V7_OK;
}
