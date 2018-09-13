//Driver for program. 

#include "graphics.h"
#include <stdio.h> //just for testing


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


int main(int argc, char **argv) {
	init_graphics_test();
	// getKeyTest();
	// sleepMsTest();

	printf("ALL Done.\n");
	return 0;
}


