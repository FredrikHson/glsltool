#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "options.h"
#include "script.h"

GLFWwindow* window = 0;
int should_quit = 0;
double deltaTime = 0;
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
float updateTime() // call once per frame
{
    double now = glfwGetTime();
    static double lastdelta = 0;
    deltaTime = now - lastdelta;
    lastdelta = now;
    static int framecounter = 0;
    framecounter++;

    if(now - lasttime > 1.0)
    {
        printf("%i fps frametime:%f\n", framecounter, deltaTime);
        framecounter = 0;
        lasttime = now;
    }
}

int main(int argc, char* argv[])
{
    if(!handle_options(argc, argv))
    {
        return 0;
    }

    if(!glfwInit())
    {
        fprintf(stderr, "Error: initing glfw\n");
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

    if(initScript(options.inputfile))
    {
        while(!glfwWindowShouldClose(window) && !should_quit)
        {
            run_loop();
            glfwPollEvents();
            updateTime();
        }
    }

    shutdownScript();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
