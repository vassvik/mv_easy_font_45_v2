#include <stdio.h>

#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#include "utils.h"
#include "font.h"

#include "utils.c"
#include "font.c"


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main() {

	// Setup glfw
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return 1;
    }

    // create window
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int resx = 1280, resy = 720;
    GLFWwindow* window = glfwCreateWindow(resx, resy, "Font renderer OpenGL 4.5", NULL, NULL);
    if (window == NULL) {
    	glfwTerminate();
        return 2;
    }
    
    // config
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Enable vsync

    // glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Could not load Opengl functions");
        glfwTerminate();
        return 3;
    }

    // load shaders
    GLuint program = LoadShaders("src/vertex_shader.vs", "src/fragment_shader.fs");

    // config opengl
    glClearColor(39/255.0, 40/255.0, 34/255.0, 1.0);	

    // main loop
    while (!glfwWindowShouldClose(window)) {
    	glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }

    glfwTerminate();

}