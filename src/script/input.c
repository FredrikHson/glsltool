#include "script.h"
#include "v7.h"
#include <stdio.h>
#include "defines.h"

extern double xpos;
extern double ypos;
extern char mousebuttons[8];
extern char mouseinside;
extern double lastxpos;
extern double lastypos;
extern char lastmousebuttons[8];
extern char lastmouseinside;

typedef struct v7 v7;
extern v7* v7g;
void updateinput()
{
    v7_set(v7g, v7_get_global(v7g), "MOUSE_X", 7, v7_mk_number(v7g, xpos));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_Y", 7, v7_mk_number(v7g, ypos));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_DELTA_X", 13, v7_mk_number(v7g, lastxpos - xpos));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_DELTA_Y", 13, v7_mk_number(v7g, lastypos - ypos));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_INSIDE", 12, v7_mk_number(v7g, mouseinside));

    for(int i = 0; i < 8; i++)
    {
        char buttondef[8] = {0};
        snprintf(buttondef, 8, "MOUSE_%i", i + 1);
        char state = 0;

        if(mousebuttons[i] == 1)
        {
            state |= MOUSE_PRESSED;

            if(lastmousebuttons[i] == 0)
            {
                state |= MOUSE_PRESSED_NOW;
            }
        }

        v7_set(v7g, v7_get_global(v7g), buttondef, 7, v7_mk_number(v7g, state));
    }
}
