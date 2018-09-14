//Driver for program. 

#include "graphics.h"
#include <stdio.h> //just for testing

#define w_key	119
#define a_key	97
#define s_key	115
#define d_key	100


void getKeyTest() {
	char key = getkey();
	while (key != 113) {
		printf("Key is %d\n", key);
		key = getkey();
	}
	printf("getKeyTest() Done.\n");
}

void sleepMsTest() {
	printf("Starting sleepMSTest()\n");
	int i;
	for (i = 0; i < 10; ++i)
	{
		//sleep for 500ms = .5 second
		sleep_ms(500); //max number of ms is 999 any more will cause nanosleep to error with invalid arguments. 
		printf("i = %d\n", i);
	}
}

void init_graphics_test() {
	init_graphics();
}

void exit_graphics_test() {
	exit_graphics();
}

void draw_big_pixel(void *img, int x, int y, color_t color){
	int i;
	int j;
	for (i = x; i < (x + 50); ++i)
	{
		for (j = y; i < (y + 50); ++j)
		{
			draw_pixel(img, i, j, color);
		}
	}
}

int main(int argc, char **argv) {
	// init_graphics_test();
	// exit_graphics_test();
	// getKeyTest();
	// sleepMsTest();

	init_graphics();
	char* offscreen_buffer = (char*) new_offscreen_buffer();

	int x_coord = 319;
	int y_coord = 239;

	color_t green = RGB(0,63,0);
	color_t black = RGB(0,0,0);

	draw_pixel(offscreen_buffer, x_coord, y_coord, green);

	blit(offscreen_buffer);

	char key = getkey();
	while (key != 113) {
		draw_pixel(offscreen_buffer, x_coord, y_coord, black);
		if (key == w_key) {
			y_coord++;
		} else if (key == s_key) {
			y_coord--;
		} else if (key == a_key) {
			x_coord--;
		} else if (key == d_key) {
			x_coord++;
		}
		draw_pixel(offscreen_buffer, x_coord, y_coord, green);
		sleep_ms(5);
		blit(offscreen_buffer);
		key = getkey();
	}

	exit_graphics();
	printf("ALL Done.\n");
	return 0;
}


