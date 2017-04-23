#include <GLFW/glfw3.h>
#include <stdio.h>
#include "options.h"

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
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
    GLFWwindow* window = glfwCreateWindow(options.width, options.height, "glsltool", NULL, NULL);

    printf("using inputfile:%s\n", options.inputfile);

    if(!window)
    {
        fprintf(stderr, "Error: creating the window\n");
        return 1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
