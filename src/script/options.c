#include "opengl.h"
#include "script.h"
#include "v7.h"
#include <stdio.h>
#include "options.h"
#include <string.h>

typedef struct v7 v7;
extern v7* v7g;
extern GLFWwindow* window;

enum v7_err js_get_optional_string(v7* v7e, v7_val_t* res)
{
    if(v7_argc(v7e) != 2)
    {
        fprintf(stderr, "invalid number of arguments to getoptionalstring\n");
        return V7_SYNTAX_ERROR;
    }
    else
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
                return V7_OK;
            }
        }

        *res = v7_mk_string(v7e, def, ~0, 0);
    }

    return V7_OK;
}

enum v7_err js_set_window_title(v7* v7e, v7_val_t* res)
{
    if(v7_argc(v7e) != 1)
    {
        fprintf(stderr, "invalid number of arguments to setwindowtitle\n");
        return V7_SYNTAX_ERROR;
    }
    else
    {
        size_t len = 0;
        v7_val_t arg1 = v7_arg(v7e, 0);
        const char* newtitle = v7_get_string(v7e, &arg1, &len);
        glfwSetWindowTitle(window, newtitle);
    }

    return V7_OK;
}

enum v7_err js_set_clipboard(v7* v7e, v7_val_t* res)
{
    if(v7_argc(v7e) != 1)
    {
        fprintf(stderr, "invalid number of arguments to setclipboard\n");
        return V7_SYNTAX_ERROR;
    }
    else
    {
        size_t len = 0;
        v7_val_t arg1 = v7_arg(v7e, 0);
        const char* clipboardstring = v7_get_string(v7e, &arg1, &len);
        glfwSetClipboardString(window, clipboardstring);
    }

    return V7_OK;
}

enum v7_err js_get_clipboard(v7* v7e, v7_val_t* res)
{
    if(v7_argc(v7e) != 0)
    {
        fprintf(stderr, "invalid number of arguments to getclipboard\n");
        return V7_SYNTAX_ERROR;
    }
    else
    {
        const char* clipboardstring = glfwGetClipboardString(window);
        *res = v7_mk_string(v7e, clipboardstring, ~0, 0);
        return V7_OK;
    }

    return V7_OK;
}
