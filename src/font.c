
// float to 11.5 fixed point, round to nearest fractional part
// `val` between 0.0 and 2047.0
unsigned short float_to_fixed(float val) {
    return 32.0*val + 0.5;
}
// 11.5 fixed point to float
float fixed_to_float(unsigned short val) {
    return val/32.0;
}

Vec3 colors[65535] = {{1,1,1}};
Glyph_Instance *glyph_instances;


Font *new_font(char *filename, int width, int max_height) 
{
	// load font stuff
    FILE *fp = fopen(filename, "rb");
    int ttf_size_max = 1e6; // most likely large enough, 1MB
    unsigned char *ttf_buffer = (unsigned char*)malloc(ttf_size_max);
    fread(ttf_buffer, 1, ttf_size_max, fp);
    fclose(fp);

    // most likely large enough
    unsigned char *bitmap = (unsigned char*)malloc(max_height*width);

    //
    Font *font = (Font*)malloc(sizeof(Font));

    //
    // @TODO: Make this input parameter
    font->num_sizes = 16;
    font->num_glyphs = 95;
    font->texture_width = width;

    Glyph_Metric *glyph_metrics_storage = malloc(sizeof(Glyph_Metric)*font->num_sizes*font->num_glyphs);
    font->glyph_metrics = (Glyph_Metric**)malloc(sizeof(Glyph_Metric*)*font->num_sizes);
    for (int i = 0; i < font->num_sizes; i++) {
    	font->glyph_metrics[i] = &glyph_metrics_storage[font->num_glyphs*i];
    }

    font->font_metrics = (Font_Metric*)malloc(sizeof(Font_Metric)*font->num_sizes);

    //
    stbtt_pack_range ranges[32] = {{72, 32, NULL, 95, font->glyph_metrics[0],  0, 0},
                                   {68, 32, NULL, 95, font->glyph_metrics[1],  0, 0},
                                   {64, 32, NULL, 95, font->glyph_metrics[2],  0, 0},
                                   {60, 32, NULL, 95, font->glyph_metrics[3],  0, 0},
                                   {56, 32, NULL, 95, font->glyph_metrics[4],  0, 0},
                                   {52, 32, NULL, 95, font->glyph_metrics[5],  0, 0},
                                   {48, 32, NULL, 95, font->glyph_metrics[6],  0, 0},
                                   {44, 32, NULL, 95, font->glyph_metrics[7],  0, 0},
                                   {40, 32, NULL, 95, font->glyph_metrics[8],  0, 0},
                                   {36, 32, NULL, 95, font->glyph_metrics[9],  0, 0},
                                   {32, 32, NULL, 95, font->glyph_metrics[10], 0, 0},
                                   {28, 32, NULL, 95, font->glyph_metrics[11], 0, 0},
                                   {24, 32, NULL, 95, font->glyph_metrics[12], 0, 0},
                                   {20, 32, NULL, 95, font->glyph_metrics[13], 0, 0},
                                   {16, 32, NULL, 95, font->glyph_metrics[14], 0, 0},
                                   {12, 32, NULL, 95, font->glyph_metrics[15], 0, 0}};

    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, bitmap, width, max_height, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1);
    stbtt_PackFontRanges(&pc, ttf_buffer, 0, ranges, font->num_sizes);
    stbtt_PackEnd(&pc);

    //
    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));

    for (int i = 0; i < font->num_sizes; i++) {
    	font->font_metrics[i].size = ranges[i].font_size;
        font->font_metrics[i].scale = stbtt_ScaleForPixelHeight(&info, ranges[i].font_size);
        int a, d, l;
        stbtt_GetFontVMetrics(&info, &a, &d, &l);
        
        font->font_metrics[i].ascent  = a*font->font_metrics[i].scale;
        font->font_metrics[i].descent = d*font->font_metrics[i].scale;
        font->font_metrics[i].linegap = l*font->font_metrics[i].scale;
    }

    //
    int filled = 0;
    font->texture_height = 0;
    for (int j = 0; j < font->num_sizes; j++) {
        for (int i = 0; i < 95; i++) {
            if (font->glyph_metrics[j][i].y1 > font->texture_height) font->texture_height = font->glyph_metrics[j][i].y1;
            filled += (font->glyph_metrics[j][i].x1 - font->glyph_metrics[j][i].x0)*(font->glyph_metrics[j][i].y1 - font->glyph_metrics[j][i].y0);
        }
    }

    printf("height = %d, fill rate = %.1f%%\n", font->texture_height, 100*filled/(double)(font->texture_width*font->texture_height)); fflush(stdout);
    stbi_write_png("font_1x1.png", font->texture_width, font->texture_height, 1, bitmap, 0);

    //
    free(ttf_buffer);


    //
    for (int i = 0; i < font->num_sizes; i++) {
    	//printf("size = %.0f, scale = %f, ascent = %.3f, descent = %.3f, linegap = %.3f\n",  font->font_metrics[i].size, font->font_metrics[i].scale, font->font_metrics[i].ascent, font->font_metrics[i].descent, font->font_metrics[i].linegap);
    }

    for (int j = 0; j < font->num_sizes; j++) {
    	for (int i = 0; i < font->num_glyphs; i++) {
    		Glyph_Metric m = font->glyph_metrics[j][i];
	    	printf("c = '%c', (x0,y0) = (%d,%d), (x1,y1) = (%d,%d), (xoff,yoff) = (%f,%f), (xoff2,yoff2) = (%f,%f), xadvance = %f\n", 32 + i, m.x0, m.y0, m.x1, m.y1, m.xoff, m.yoff, m.xoff2, m.yoff2, m.xadvance);
	    }
    }
    fflush(stdout);

    // @TODO: Inline shaders
    font->program = LoadShaders("src/vertex_shader.vs", "src/fragment_shader.fs");
    glUseProgram(font->program);

    //asd
    glGenVertexArrays(1, &font->vao);
    glBindVertexArray(font->vao);

    // @TODO: update to bindless textures?
    glGenTextures(1, &font->texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, font->texture_width, font->texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, &bitmap[0]);
    
    // SSBO's for general gpu storage, used for indirect lookup using instance ID
    glGenBuffers(1, &font->glyph_instance_buffer);
    glGenBuffers(1, &font->glyph_metric_buffer);
    glGenBuffers(1, &font->color_buffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, font->glyph_instance_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Glyph_Instance)*MAX_INSTANCES, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, font->glyph_instance_buffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, font->glyph_metric_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Glyph_Metric)*font->num_sizes*font->num_glyphs, &font->glyph_metrics[0][0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, font->glyph_metric_buffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, font->color_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(colors), &colors[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, font->color_buffer);

    printf("asdasd %zd\n", sizeof(colors)); fflush(stdout);

    //
    glyph_instances = (Glyph_Instance*)malloc(sizeof(Glyph_Instance)*MAX_INSTANCES);

    return font;
}




void upload_instances(Font *font, int num_instances) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, font->glyph_instance_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, font->glyph_instance_buffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Glyph_Instance)*num_instances, &glyph_instances[0]);
}

int update_instances_from_string(Font *font, char *str, unsigned short *palette, int idx, float *max_extent_x, float *max_extent_y) {
    // parse the string, place the glyphs appropriately and set the colors
    float cursor_x = 4.0;
    float cursor_y = 4.0 + (int)(1.0*font->font_metrics[idx].ascent + 0.5);

    float extent_x = 0.0;
    float extent_y = 0.0;

    int num_instances = 0;
    int len = strlen(str);
    for (int i = 0; i < len; i++) {

    	char c = str[i];
        if (c == '\n') {
            cursor_x  = 4.0;
            cursor_y += (int)(font->font_metrics[idx].size + 0.5);

            *max_extent_x = max(*max_extent_x, extent_x);
            extent_x = 0.0;
            extent_y += (int)(font->font_metrics[idx].size + 0.5);
            continue;
        }
        glyph_instances[i].x = float_to_fixed(cursor_x);
        glyph_instances[i].y = float_to_fixed(cursor_y);
        glyph_instances[i].index = idx*font->num_glyphs + (c - 32);
        
        if (palette) glyph_instances[i].palette = palette[i] % 65535;
        else glyph_instances[i].palette = 0;
        
        cursor_x += font->glyph_metrics[idx][c - 32].xadvance;
        extent_x += font->glyph_metrics[idx][c - 32].xadvance;
        num_instances += 1;

        //printf("%d %d %d %d\n", glyph_instances[i].x, glyph_instances[i].y, glyph_instances[i].index, glyph_instances[i].palette);
    }
    *max_extent_x = max(*max_extent_x, extent_x);
    if (extent_x > 0.0) extent_y += (int)(font->font_metrics[idx].size + 0.5);
    *max_extent_y = extent_y;

    upload_instances(font, num_instances);

    return num_instances;
}

void draw_instances(Font *font, int num_instances, float offset_x, float offset_y) {
    //save_state();

    // Change state
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, font->texture);
    glUseProgram(font->program);

    GLint dims[4];
    glGetIntegerv(GL_VIEWPORT, &dims[0]);
    glUniform2f(glGetUniformLocation(font->program, "window_resolution"), dims[2] - dims[0], dims[3]-dims[1]);
    
    glUniform2f(glGetUniformLocation(font->program, "string_offset"), offset_x, offset_y); 
    glUniform2f(glGetUniformLocation(font->program, "bitmap_resolution"), font->texture_width, font->texture_height); 
    glUniform1i(glGetUniformLocation(font->program, "sampler_bitmap"), 0);

    //printf("%d %d %d %d, %f %f, %d %d\n", dims[0], dims[1], dims[2], dims[3], offset_x, offset_y, font->texture_width, font->texture_height);
    glBindVertexArray(font->vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_instances);

    //restore_state();
}
