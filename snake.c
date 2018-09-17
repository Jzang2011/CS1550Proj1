//Driver for program. 

#include "graphics.h"
#include <stdio.h> 

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
	int go_x;
	int go_y;
	int go_in_pos_dir;
	while (key != 'q') {
		draw_pixel(offscreen_buffer, x_coord, y_coord, black); // clear previous pixel.

		if (key != 0)
		{
			if (key == 'w') {
				go_x = 0;
				go_y = 1;
				go_in_pos_dir = 0;
			} else if (key == 's') {
				go_x = 0;
				go_y = 1;
				go_in_pos_dir = 1;
			} else if (key == 'a') {
				go_x = 1;
				go_y = 0;
				go_in_pos_dir = 0;
			} else if (key == 'd') {
				go_x = 1;
				go_y = 0;
				go_in_pos_dir = 1;
			}
		}

		if ((go_x == 0) && (go_y == 1) && (go_in_pos_dir == 0)) {
			// Go up
			if ((y_coord - 1) >= 0) {
				y_coord -= 1;
			} else {
				y_coord += (last_Y_Coord);
			}
		} else if ((go_x == 0) && (go_y == 1) && (go_in_pos_dir == 1)) {
			// go down
			if ((y_coord + 1) <= last_Y_Coord) {
				y_coord += 1;
			} else {
				//wrap arround to top of screen
				y_coord -= (last_Y_Coord);
			}
		} else if ((go_x == 1) && (go_y == 0) && (go_in_pos_dir == 0)) {
			// go left
			x_coord -= 1;
		} else if ((go_x == 1) && (go_y == 0) && (go_in_pos_dir == 1)) {
			// go right 
			x_coord += 1;
		}
		
		draw_pixel(offscreen_buffer, x_coord, y_coord, green);
		sleep_ms(1); //determins speed 
		blit(offscreen_buffer);
		key = getkey();
	}

	//game over. Clean up.
	clear_screen(offscreen_buffer);
	blit(offscreen_buffer);
	exit_graphics();
	printf("ALL Done.\n");
	return 0;
}


