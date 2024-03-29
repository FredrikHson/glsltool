#include "opengl.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include "options.h"
#include "script.h"
#include "resources.h"
#include <unistd.h>
#include "notify.h"
#include <stdlib.h>
#include "defines.h"
#include "renderfunc.h"
#include "debug.h"
#include <sanitizer/lsan_interface.h>

GLFWwindow* window = 0;
int should_quit = 0;
double deltaTime = 0;
double currenttime = 0;
double lasttime = 0;
double xpos = 0;
double ypos = 0;
char mousebuttons[8] = {0};
char mouseinside = 0;
double lastxpos = 0;
double lastypos = 0;
char lastmousebuttons[8] = {0};
char lastmouseinside = 0;

char keyboard[GLFW_KEY_LAST + 1] = {0};
char keyboardlast[GLFW_KEY_LAST + 1] = {0};

extern char avoid_debugging;



void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    options.width = width;
    options.height = height;
    avoid_debugging = 1;
    resizeTargets();

    if(validscript)
    {
        run_resize();
    }
}

void key_handler_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        should_quit = 1;
        printf("abort abort!\n");
    }
}

void updateTime(void) // call once per frame
{
    currenttime = glfwGetTime();
    static double lastdelta = 0;
    deltaTime = currenttime - lastdelta;
    lastdelta = currenttime;

    if(options.fps)
    {
        static int framecounter = 0;
        framecounter++;

        if(currenttime - lasttime > 1.0)
        {
            printf("%i fps frametime:%f\n", framecounter, deltaTime);
            framecounter = 0;
            lasttime = currenttime;
        }
    }
}

void handleMouse(void)
{
    lastxpos = xpos;
    lastypos = ypos;
    lastmouseinside = mouseinside;
    glfwGetCursorPos(window, &xpos, &ypos);
    mouseinside = 1;

    if(xpos < 0 || xpos >= options.width)
    {
        mouseinside = 0;
    }

    if(ypos < 0 || ypos >= options.height)
    {
        mouseinside = 0;
    }

    for(int i = 0; i < 8; i++)
    {
        lastmousebuttons[i] = mousebuttons[i];

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1 + i) == GLFW_PRESS)
        {
            mousebuttons[i] = 1;
        }
        else
        {
            mousebuttons[i] = 0;
        }
    }
}

#ifndef NDEBUG
void GLAPIENTRY OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type, severity, message);
    }
}
#endif

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    run_droppedfiles(paths, count);
}

void handleKeys(void)
{
    for(int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST + 1; i++)
    {
        keyboardlast[i] = keyboard[i];
        keyboard[i] = glfwGetKey(window, i) == GLFW_PRESS;
    }
}

int main(int argc, char* argv[])
{
    if(!handle_options(argc, argv))
    {
        return 0;
    }

    if(access(options.inputfile, F_OK) != 0)
    {
        fprintf(stderr, "could not find: %s\n", options.inputfile);
        return 1;
    }

    glfwSetErrorCallback(error_callback);

    if(!glfwInit())
    {
        fprintf(stderr, "Error: initing glfw\n");
        return 1;
    }

    if(!initFileWatcher())
    {
        fprintf(stderr, "Error: initing filewatcher\n");
        return 1;
    }

    char* filename = strdup(options.inputfile);
    char title[80] = {0};

    if(filename != 0)
    {
        snprintf(title, 80, "glsltool - %s", basename(filename));
        free(filename);
    }
    else
    {
        snprintf(title, 80, "glsltool");
    }

    if(options.transp)
    {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    if(options.outputfile == 0 && options.supersample)
    {
        glfwWindowHint(GLFW_SAMPLES, 4);
    }

    if(options.outputfile != 0)
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        if(options.supersample)
        {
            options.width *= 2;
            options.height *= 2;
        }
    }

    window = glfwCreateWindow(options.width, options.height, title, NULL, NULL);
    printf("using inputfile:%s\n", options.inputfile);

    if(!window)
    {
        fprintf(stderr, "Error: creating the window\n");
        return 1;
    }

    if(!(options.x == INT_MIN || options.y == INT_MIN))
    {
        printf("%i,%i\n", options.x, options.y);
        glfwSetWindowPos(window, options.x, options.y);
    }

    glfwSetDropCallback(window, drop_callback);
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_handler_callback);
    glfwPollEvents();
    glfwSwapInterval(0);
    glfwSwapBuffers(window);
    #ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGLMessageCallback, 0);
    #endif
    ilInit();
    iluInit();
    initImages();
    initDebug();
    watchFile(options.inputfile, reloadScript);
    initScript(options.inputfile);

    while(!glfwWindowShouldClose(window) && !should_quit)
    {
        handleMouse();
        handleKeys();

        if(validscript)
        {
            run_loop();
        }
        else
        {
            glClearColor(1, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glfwSwapBuffers(window);
        }

        glfwPollEvents();
        watchChanges();
        updateTime();

        if(options.outputfile)
        {
            static int frame = 0;

            if(frame++ > options.outputframe)
            {
                saveRenderTarget(-1, 0, options.outputfile);
                should_quit = 1;
            }
        }
    }

    fprintf(stderr, "after initscript\n");
    shutdownScript();
    cleanupImages();
    cleanupMeshes();
    cleanupShaders();
    cleanupDebug();
    destroyFileWatcher();
    glfwDestroyWindow(window);
    glfwTerminate();
    cleanup_options();

    fprintf(stderr, "shutting down\n");
    return 0;
}
