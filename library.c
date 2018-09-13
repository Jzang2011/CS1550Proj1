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

#include "graphics.h"

#include <stdio.h> //just for testing
#include <stdlib.h> // just for testing
#include <errno.h>

 //move to header file 

struct termios oldTermSettings; //used to convert to old terminal settings.

// void* mmapAddress; //address of the memory mapping. 
// void* offscreen_buffer;
    char* mmapAddress; //Current screenbuffer (consider renaming? )
    char* offscreen_buffer;


int mmapLength;

void init_graphics() {

    int fD = 0; //fD = fild Descriptor
    struct fb_var_screeninfo vInfo;
    struct fb_fix_screeninfo fInfo;
    struct termios termSettings;
    int yRez = 0;
    int xRez = 0;

    //test
	//1. Open graphics device 
    fD = open("/dev/fb0", O_RDWR);
    printf("fD = %d\n", fD);

    //Just for testing
    if (fD == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    //3  use typedef to make a color type color_t
    // use ioctl to get screen size and bits per pixels
    int retVal = ioctl(fD, FBIOGET_VSCREENINFO, &vInfo);
    int retVal2 = ioctl(fD, FBIOGET_FSCREENINFO, &fInfo);

    yRez = vInfo.yres_virtual;
    xRez = fInfo.line_length;

    mmapLength = yRez * xRez;

    //2. memory mapping mmap()
    // void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    // length = yrez_virtual * line_length?
    mmapAddress = (char*) mmap(NULL, mmapLength, PROT_READ | PROT_WRITE ,MAP_SHARED, fD, 0);
    if(mmapAddress == MAP_FAILED) {
        write(1, "ERROR mmap failed\n", 20);
        printf("errno is %d\n", errno);
    }


    //4. clear screen
    //Print "\033[2J" to tell the terminal to clear itself
    // write(1, "\033[2J", 8); //If you cant use printf() use this line.

	//5. Disable keypress echo.
    //TCGETS:
        // The argument is a pointer to a termios structure. The current terminal parameters are fetches and stored in that structure.
    //TCGETS: 
        // The argument is a pointer to a termios structure. The current terminal parameters are set from the values stored in that structure. The change is immediate.
    int retVal3 = ioctl(fD, TCGETS, &termSettings); // loads terminal settings into termSettings. 
    printf("retVal3 is %d\n", retVal3);
    printf("errno is %d\n", errno);
    int retVal4 = ioctl(fD, TCGETS, &oldTermSettings); //loads terminal settings into variable to be used later for reverting. 
    printf("retVal4 is %d\n", retVal4);
    printf("errno is %d\n", errno);
    
    // disable canonical mode by unsetting the ICANON bit and disaling ECHO by forcing that bit to zero.

    //unset the ICANON bit
    //forces the ECHO bit to zero as well
    termSettings.c_lflag &= ~(ICANON | ECHO); //c_lflag = c_lflag AND !(ICANON OR ECHO)
    ioctl(fD, TCSETS, &termSettings);

//     //force ECHO bit to zero 
//     termSettings.c_lflag = (ECHO - ECHO); //should force ECHO bit to zero.
//     ioctl(fD, TCSETS, &termSettings); //Set the new terminal settings
}

void exit_graphics() {
    int fD = 0; //fD = fild Descripto
	//Undo disabling canonical mode
    //make call to ioctl() to reenable  echoing and buffering
    //Basically undo step 5 in init_graphics()

    //Sets the settings back to the old settings which were saved in init.
    ioctl(fD, TCSETS, &oldTermSettings);
}

char getkey() {
    int key = 0;
    int fD = 0; //fD is the file descriptor. 0 for keyboard input. 
    int nfds = 1; //number of file descriptors. 1 since we are only worried about keyboard input. 
    fd_set fs; //declare a fd_set named fs
    char buffer;

    FD_ZERO(&fs);
    FD_SET(fD, &fs);

    //int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
    int numberOfKeysReady = select(nfds, &fs, NULL, NULL, NULL); //Look up man pages for select(2)

    //if numberOfKeysReady == 1 read from the file descriptor
    // use read sys call to get the key pressed 
    if (numberOfKeysReady > 0)
    {
        //ssize_t read(int fd, void *buf, size_t count);
       int retVal = read(fD, &buffer, 1);
       // printf("retVal from Read is %d\n", retVal);
       // printf("Errno is %d\n", errno);
        key = buffer;
        // printf("key pressed is %d\n", key);
    }
    return key; //value of key is the decimal value of the character. See ascii table. 
}

//Max value of ms is 999. Any more than that will cause invalid arguments to nanosleep()
// This is because 1,000,000,000 ns = 1 second. If calling funciton with more than 1,000,000,000 ns
// it is invalid because you shouldl use the tim.tv_sec parameter of the timespec struct. 
void sleep_ms(long ms) {
	int nanoSeconds = ms * 1000000; 
	struct timespec tim;
	tim.tv_sec = 0;
	tim.tv_nsec = ms * 1000000L;
	int retval = nanosleep(&tim, NULL);
}

void clear_screen(void *img) {
	
}

// void draw_pixel(void *img, int x, int y, color_t color) {
	
// }

// void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c) {
	
// }

void *new_offscreen_buffer() {
    int fD = -1; //File descriptor of -1 to call mmap() like malloc()
    //Use MAP_PRIVATE | MAP_ANONYMOUS to use mmap() like malloc()
	void* offscreen_buffer_adr = mmap(NULL, mmapLength, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fD, 0);
    offscreen_buffer = (char*) offscreen_buffer_adr;

    return offscreen_buffer_adr;
}

void blit(void *src) {
	int i = 0;
    for (i = 0; i < mmapLength; i++) {
       offscreen_buffer[i] =  mmapAddress[i];
    }
}
