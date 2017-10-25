#include "opengl.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <stdio.h>
#include "options.h"
#include "script.h"
#include "resources.h"
#include <unistd.h>
#include "notify.h"
#include <stdlib.h>

GLFWwindow* window = 0;
int should_quit = 0;
double deltaTime = 0;
double currenttime = 0;
double lasttime = 0;

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    options.width = width;
    options.height = height;
}
void key_handler_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        should_quit = 1;
        printf("abort abort!\n");
    }
}
void updateTime() // call once per frame
{
    currenttime = glfwGetTime();
    static double lastdelta = 0;
    deltaTime = currenttime - lastdelta;
    lastdelta = currenttime;
    static int framecounter = 0;
    framecounter++;

    if(currenttime - lasttime > 1.0)
    {
        printf("%i fps frametime:%f\n", framecounter, deltaTime);
        framecounter = 0;
        lasttime = currenttime;
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

    glfwSetErrorCallback(error_callback);
    window = glfwCreateWindow(options.width, options.height, "glsltool", NULL, NULL);
    printf("using inputfile:%s\n", options.inputfile);

    if(!window)
    {
        fprintf(stderr, "Error: creating the window\n");
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_handler_callback);
    glfwSwapInterval(1);
    glfwSwapBuffers(window);
    ilInit();
    iluInit();
    watchFile(options.inputfile, reloadScript);
    initScript(options.inputfile);

    while(!glfwWindowShouldClose(window) && !should_quit)
    {
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
        usleep(16666);
    }

    fprintf(stderr, "after initscript\n");
    shutdownScript();
    cleanupImages();
    cleanupMeshes();
    cleanupShaders();
    destroyFileWatcher();
    glfwDestroyWindow(window);
    glfwTerminate();

    if(options.inputfile)
    {
        free(options.inputfile);
    }

    return 0;
}
