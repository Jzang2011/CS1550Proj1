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
	printf("WELCOME TO SNAKE GAME:\n");
	printf("use w,s,a,d keys to move the snake\n");
	printf("press q to quit.\n");
	printf("game will start in 10 seconds.\n");

	char key = getkey();
	
	int i;
	for (i = 10; i > 0; --i)
	{
		printf("--%d--\n", i);
		sleep_ms(999);
	}

	init_graphics();
	char* offscreen_buffer = (char*) new_offscreen_buffer();

	int x_coord = 319;
	int y_coord = 239;

	color_t green = RGB(0,63,0);
	color_t black = RGB(0,0,0);

	draw_pixel(offscreen_buffer, x_coord, y_coord, green);

	blit(offscreen_buffer);

	int last_Y_Coord = get_yrez() - 1;
	while (key != 'q') {
		draw_pixel(offscreen_buffer, x_coord, y_coord, black);

		if (key == 'w') {
			if ((y_coord - 2) >= 0)
			{
				y_coord -= 2;
			} else {
				y_coord += (get_yrez() - 2);
			}
		} else if (key == 's') {
			if ((y_coord + 2) <= last_Y_Coord)
			{
				y_coord += 2;
			} else {
				y_coord -= (last_Y_Coord);
			}
		} else if (key == 'a') {
			x_coord -= 2;
		} else if (key == 'd') {
			x_coord += 2;
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


