// Jeremy Zang (jez30)


#include <time.h>

void init_graphics() {

}

void exit_graphics() {
	
}

char getkey() {
	
}

void sleep_ms(long ms) {
	// int nanoSeconds = ms * 1000000; 
	// struct timespec req;
	// req.tv_sec = 0;
	// req.tv_nsec = ms * 1000000L;
	// nanosleep(&req, NULL);
	nanosleep((const struct timespec[]){{0, (ms *1000000L)}}, NULL);

}

void clear_screen(void *img) {
	
}

// void draw_pixel(void *img, int x, int y, color_t color) {
	
// }

// void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c) {
	
// }

void *new_offscreen_buffer() {
	
}

void blit(void *src) {
	
}
