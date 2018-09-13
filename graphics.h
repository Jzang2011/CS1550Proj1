//Public interface for.... library? 

#define RGB(r,g,b) (((r & 31) << 11) + ((g & 63) << 5) + (b & 31)) // move to header file

typedef unsigned short color_t;

void draw_pixel(void *img, int x, int y, color_t color);

void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c);

void init_graphics();

void exit_graphics();

char getkey();

void sleep_ms(long ms);

void clear_screen(void *img);

void *new_offscreen_buffer();

void blit(void *src);