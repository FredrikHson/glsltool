#include "script.h"
#include "v7.h"
#include <stdio.h>
#include "opengl.h"
#include "defines.h"
#include "keys.h"

extern double xpos;
extern double ypos;
extern char mousebuttons[8];
extern char mouseinside;
extern double lastxpos;
extern double lastypos;
extern char lastmousebuttons[8];
extern char lastmouseinside;
extern char keyboard[GLFW_KEY_LAST + 1];
extern char keyboardlast[GLFW_KEY_LAST + 1];

typedef struct v7 v7;
extern v7* v7g;

char getstate(char last, char now)
{
    char state = 0;

    if(now == 1)
    {
        state |= PRESSED;

        if(last == 0)
        {
            state |= PRESSED_NOW;
        }
    }

    return state;
}

void updateinput(void)
{
    v7_set(v7g, v7_get_global(v7g), "MOUSE_X", 7, v7_mk_number(v7g, xpos));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_Y", 7, v7_mk_number(v7g, ypos));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_DELTA_X", 13, v7_mk_number(v7g, lastxpos - xpos));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_DELTA_Y", 13, v7_mk_number(v7g, lastypos - ypos));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_INSIDE", 12, v7_mk_number(v7g, mouseinside));
    char anybuttonstate = 0;

    for(int i = 0; i < 8; i++)
    {
        char buttondef[8] = {0};
        snprintf(buttondef, 8, "MOUSE_%i", i + 1);
        char state = getstate(lastmousebuttons[i], mousebuttons[i]);
        anybuttonstate |= state;
        v7_set(v7g, v7_get_global(v7g), buttondef, 7, v7_mk_number(v7g, state));
    }

    char anykeystate = 0;

    for(int i = 0; keys[i].name[0] != 0; i++)
    {
        int id = keys[i].glfwid;
        char state = getstate(keyboardlast[id], keyboard[id]);
        anykeystate |= state;
        v7_set(v7g, v7_get_global(v7g), keys[i].name, ~0, v7_mk_number(v7g, state));
    }

    v7_set(v7g, v7_get_global(v7g), "KEY_ANY", 7, v7_mk_number(v7g, anykeystate));
    v7_set(v7g, v7_get_global(v7g), "MOUSE_ANY", 9, v7_mk_number(v7g, anybuttonstate));
}
