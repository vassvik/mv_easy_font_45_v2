#define MAX_INSTANCES 1000000

typedef struct Font_Metric {
	float size;
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
	unsigned short x, y;
	unsigned short index, palette;
} Glyph_Instance;

typedef struct Vec3 {
	float x, y, z;
} Vec3;

typedef struct Font {
	int num_sizes;
	int num_glyphs;

	Font_Metric *font_metrics;
	Glyph_Metric **glyph_metrics;

	GLuint texture;
	int texture_width, texture_height;

	GLuint vao;
	GLuint program;

	GLuint glyph_instance_buffer, glyph_metric_buffer, color_buffer;
} Font;

unsigned short float_to_fixed(float val);
float fixed_to_float(unsigned short val);

Font *new_font(char *filename, int width, int max_height);