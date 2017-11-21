#include <stdio.h>

#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb/stb_rect_pack.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

    // config opengl
    glClearColor(39/255.0, 40/255.0, 34/255.0, 1.0);	

	Font *font = new_font("consola.ttf", 1024, 1024);

    // main loop
    while (!glfwWindowShouldClose(window)) {
    	glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

        glClear(GL_COLOR_BUFFER_BIT);

        float span_x, span_y;
        int num_drawn = update_instances_from_string(font, "yay\n  it\n finally worked! !", NULL, 0, &span_x, &span_y);
        //printf("%d %f %f\n", num_drawn, span_x, span_y); fflush(stdout);

        draw_instances(font, num_drawn, 40, 40);

        glfwSwapBuffers(window);
    }


    glfwTerminate();

}