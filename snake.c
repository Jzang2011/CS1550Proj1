//Driver for program. 

#include "graphics.h"
#include <stdio.h> //just for testing

//ASCII codes for keys;
#define w_key	119
#define a_key	97
#define s_key	115
#define d_key	100
#define q_key 	113

/*
-	Just used for testing purposes
*/
void getKeyTest() {
	char key = getkey();
	while (key != 113) {
		printf("Key is %d\n", key);
		key = getkey();
	}
	printf("getKeyTest() Done.\n");
}

/*
-	Just used for testing purposes
*/
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

/*
-	Just used for testing purposes
*/
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

	printf("WELCOME TO SNAKE GAME:\n");
	printf("use w,s,a,d keys to move the snake\n");
	printf("press q to quit.\n");
	printf("game will start in 10 seconds.\n");

	char key = getkey();
	
	// int i;
	// for (i = 10; i > 0; --i)
	// {
	// 	printf("--%d--\n", i);
	// 	sleep_ms(999);
	// }

	init_graphics();
	char* offscreen_buffer = (char*) new_offscreen_buffer();

	int x_coord = 319;
	int y_coord = 239;

	color_t green = RGB(0,63,0);
	color_t black = RGB(0,0,0);

	draw_pixel(offscreen_buffer, x_coord, y_coord, green);

	blit(offscreen_buffer);

	//Testing corner cases. Round corners of QEMU window prevent seeing botom corners. 
	color_t red = RGB(31,0,0);
	// draw_pixel(offscreen_buffer, 639, 0, red);
	// draw_pixel(offscreen_buffer, 0, 477, red);
	// draw_pixel(offscreen_buffer, 0, 0, red);
	// draw_pixel(offscreen_buffer, 639, 477, red);

	int x = 0;
	int y = 0;
	
	//working
	for (y = 0; y < 480; y++)
	{
		draw_line(offscreen_buffer, 0, 0, 639, y, green);
	}

	//not working
	// y = 0;
	// for (y = 0; y < 480; y++)
	// {
	// 	draw_line(offscreen_buffer, 639, 0, 0, y, red);
	// }
	draw_line(offscreen_buffer, 0, 479, 639, 0, red);


	blit(offscreen_buffer);

	while (key != 'q') {
		draw_pixel(offscreen_buffer, x_coord, y_coord, black);

		// draw_line(offscreen_buffer, 0, 0, 639, 479, green);
		if (key == 'w') {
			y_coord--;
		} else if (key == 's') {
			y_coord++;
		} else if (key == 'a') {
			x_coord--;
		} else if (key == 'd') {
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


