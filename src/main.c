#include <stdio.h>

#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#include "utils.h"
#include "font.h"

#include "utils.c"
#include "font.c"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb/stb_rect_pack.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

typedef struct Font_Metric {
	float ascent, descent, linegap, scale;
} Font_Metric;

/*typedef struct
{
   unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
   float xoff,yoff,xadvance;
   float xoff2,yoff2;
} stbtt_packedchar;*/
typedef stbtt_packedchar Glyph_Metric;

typedef struct Glyph_Instance {
	unsigned short x,y;
	unsigned short index, palette;
} Glyph_Instance;

typedef struct Vec4 {
	float x, y, z, w;
} Vec4;


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

    // load font stuff
    FILE *fp = fopen("consola.ttf", "rb");

    int ttf_size_max = 1e6; // most likely large enough, 1MB
    unsigned char *ttf_buffer = (unsigned char*)malloc(ttf_size_max);
    fread(ttf_buffer, 1, ttf_size_max, fp);
    
    fclose(fp);

    // probably large enough
    int width = 1024;
    int height = 1024;
    unsigned char *bitmap = (unsigned char*)malloc(height*width);
    
    #define NUM_SIZES 16

    stbtt_pack_context pc;
    Glyph_Metric glyph_metrics[NUM_SIZES][95];
    stbtt_pack_range ranges[99] = {{72, 32, NULL, 95, glyph_metrics[0], 0, 0},
                                   {68, 32, NULL, 95, glyph_metrics[1], 0, 0},
                                   {64, 32, NULL, 95, glyph_metrics[2], 0, 0},
                                   {60, 32, NULL, 95, glyph_metrics[3], 0, 0},
                                   {56, 32, NULL, 95, glyph_metrics[4], 0, 0},
                                   {52, 32, NULL, 95, glyph_metrics[5], 0, 0},
                                   {48, 32, NULL, 95, glyph_metrics[6], 0, 0},
                                   {44, 32, NULL, 95, glyph_metrics[7], 0, 0},
                                   {40, 32, NULL, 95, glyph_metrics[8], 0, 0},
                                   {36, 32, NULL, 95, glyph_metrics[9], 0, 0},
                                   {32, 32, NULL, 95, glyph_metrics[10], 0, 0},
                                   {28, 32, NULL, 95, glyph_metrics[11], 0, 0},
                                   {24, 32, NULL, 95, glyph_metrics[12], 0, 0},
                                   {20, 32, NULL, 95, glyph_metrics[13], 0, 0},
                                   {16, 32, NULL, 95, glyph_metrics[14], 0, 0},
                                   {12, 32, NULL, 95, glyph_metrics[15], 0, 0}};

    stbtt_PackBegin(&pc, bitmap, width, height, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1);
    stbtt_PackFontRanges(&pc, ttf_buffer, 0, ranges, NUM_SIZES);
    stbtt_PackEnd(&pc);

    Font_Metric font_metrics[NUM_SIZES];

    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
    for (int i = 0; i < NUM_SIZES; i++) {
        font_metrics[i].scale = stbtt_ScaleForPixelHeight(&info, ranges[i].font_size);
        int a, d, l;
        stbtt_GetFontVMetrics(&info, &a, &d, &l);
        
        font_metrics[i].ascent  = a*font_metrics[i].scale;
        font_metrics[i].descent = d*font_metrics[i].scale;
        font_metrics[i].linegap = l*font_metrics[i].scale;
    }

    int filled = 0, max_y = 0;
    for (int j = 0; j < NUM_SIZES; j++) {
        for (int i = 0; i < 95; i++) {
            if (glyph_metrics[j][i].y1 > max_y) max_y = glyph_metrics[j][i].y1;
            filled += (glyph_metrics[j][i].x1 - glyph_metrics[j][i].x0)*(glyph_metrics[j][i].y1 - glyph_metrics[j][i].y0);
        }
    }

    printf("max_y = %d, fill rate = %.1f%%\n", max_y, 100*filled/(double)(width*max_y));
    fflush(stdout);

    stbi_write_png("font_1x1.png", width, max_y, 1, bitmap, 0);


    // main loop
    while (!glfwWindowShouldClose(window)) {
    	glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }

    free(ttf_buffer);

    glfwTerminate();

}