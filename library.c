// Jeremy Zang (jez30)
//for compiling project
// gcc -c library.c 
// gcc -c driver.c (driver.c is my own testing driver similar to hilbert.c)
// gcc -o driver driver.o library.o

#include <time.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <sys/select.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <stdio.h> //just for testing
#include <stdlib.h> // just for testing

#define RGB(r,g,b) (((r & 31) << 11) + ((g & 63) << 5) + (b & 31))


typedef unsigned short color_t;

void init_graphics() {

    int fD = 0; //fD = fild Descriptor
   // struct fb_var_screeninfo vInfo;
   // struct fb_fix_screeninfo fInfo;
    int yRez = 0;
    int xRez = 0;

	//1. Open graphics device 
		// using a framebuffer
	char filePath[] = "/dev/fb0"; // path to framebuffer 
	printf("%s", filePath);
    fD = open("/dev/fb0", O_RDWR);

    //Just for testing
    if (fD == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    //3  use typedef to make a color type color_t
    // use ioctl to get screen size and bits per pixels
    struct fb_fix_screeninfo fInfo = ioctl(fD, FBIOGET_FSCREENINFO);
    struct fb_fix_screeninfo fInfo = ioctl(fD, FBIOGET_VSCREENINFO);

    yRez = fb_var_screeninfo.yres_virtual;
    xRez = fb_fix_screeninfo.line_length;

    int mmapLength = yRez * xRez;

    //2. memory mapping mmap()
    // void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    // length = yrez_virtual * line_length?
    void* mmapAddress = mmap(NULL, mmapLength, PROT_READ | PROT_WRITE ,MAP_SHARED, fD, 0);

    //4. clear screen
    //Print "\033[2J" to tell the terminal to clear itself
    printf("\033[2J");


    struct termios termSettings;
	//5. Disable keypress echo.
    int temp1 = ioctl(fD, TCGETS, &termSettings);
    // disable canonical mode by unsetting the ICANON bit and disaling ECHO by forcing that bit to zero.

}

void exit_graphics() {
	//Undo disabling canonical mode
    //make call to ioctl() to reenable  echoing and buffering
    //Basically undo step 5 in init_graphics()
}

char getkey() {
    int fD = 0; //fD = fild Descriptor

	int returnVale = select(fD, );
    //Figure out how to use select system call.

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

int main() {
	init_graphics();
	return 0;
}
