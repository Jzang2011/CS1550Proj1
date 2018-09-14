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

struct termios oldTermSettings; //used to convert to old terminal settings.
struct termios termSettings;

// void* screen_buffer; //address of the memory mapping. 
// void* offscreen_buffer;
    char* screen_buffer; //perhaps it may be easier to work with a screen buffer of color_t type? 
    char* offscreen_buffer; //you can get rid of this.

int sb_size; //size of the screen_buffer;
int yRez;
int xRez;
int frameBuffer = 0;

void init_graphics() {

     //fD = fild Descriptor
    struct fb_var_screeninfo vInfo;
    struct fb_fix_screeninfo fInfo;
    
    
    //test
	//1. Open graphics device 
    frameBuffer = open("/dev/fb0", O_RDWR);

    //Just for testing
    if (frameBuffer == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    /* 3  Use typedef to make a color type color_t (declared in graphics.h)
        - Use ioctl() to get screen size and bits per pixels.
        - Compute length of frameBuffer by yRez * line_length
    */
    int retVal = ioctl(frameBuffer, FBIOGET_VSCREENINFO, &vInfo);
    int retVal2 = ioctl(frameBuffer, FBIOGET_FSCREENINFO, &fInfo);

    yRez = vInfo.yres_virtual;
    xRez = fInfo.line_length;

    // printf("yRez is %d\n", yRez); //prints 480
    // printf("xRez is %d\n", xRez); //prints 1280

    sb_size = yRez * xRez;
 
    /* 2. memory mapping mmap()
        - void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
        - length = yrez_virtual * line_length?
    */
    screen_buffer = (char*) mmap(NULL, sb_size, PROT_READ | PROT_WRITE ,MAP_SHARED, frameBuffer, 0);
    if(screen_buffer == MAP_FAILED) {
        write(1, "ERROR mmap failed\n", 20);
        printf("errno is %d\n", errno);
    }
    // printf("screen_buffer is %p\n", screen_buffer);
    // printf("screen_buffer + 1 is %p\n", screen_buffer + 1);

    /* 4. Clear screen
        - Print "\033[2J" to tell the terminal to clear itself
    */
    write(1, "\033[2J", 8); //If you cant use printf() use this line.
    
	/*
    5. Disable keypress echo.
    - TCGETS:
        - The argument is a pointer to a termios structure. The current terminal parameters are fetches and stored in that structure.
    - TCGETS: 
        - The argument is a pointer to a termios structure. The current terminal parameters are set from the values stored in that structure. The change is immediate.
    - Pass fD of 0 to since ioctl() is being used to access the keyboard settings. 
    */
    int retVal3 = ioctl(0, TCGETS, &termSettings); // loads terminal settings into termSettings. 
    int retVal4 = ioctl(0, TCGETS, &oldTermSettings); //loads terminal settings into variable to be used later for reverting. 
    
    /* 
    6. Disable canonical mode by unsetting the ICANON bit and disaling ECHO by forcing that bit to zero.
     - Unset the ICANON bit. This can be done by setting it to its complement (~)
     - Forces the ECHO bit to zero as well.
    */
    termSettings.c_lflag &= ~(ICANON | ECHO); //c_lflag = c_lflag AND !(ICANON OR ECHO)
    int retVal5 = ioctl(0, TCSETS, &termSettings);

//  alternative code. 
//     //force ECHO bit to zero 
//     termSettings.c_lflag = (ECHO - ECHO); //should force ECHO bit to zero.
//     ioctl(fD, TCSETS, &termSettings); //Set the new terminal settings
}

void exit_graphics() {
    int fD = 0; //fD = file Descriptor for keyboard. 
    /*
        - Undo disabling canonical mode
        - make call to ioctl() to reenable  echoing and buffering
        - Basically undo step 5 in init_graphics()
    */

    //Sets the settings back to the old settings which were saved in init.
    int retVal = ioctl(fD, TCGETS, &termSettings);
    termSettings.c_lflag |= (ICANON | ECHO); 

    int retVal2 = ioctl(fD, TCSETS, &termSettings);
    // int retVal = ioctl(fD, TCSETS, &oldTermSettings);
    
    // printf("retVal is %d\n", retVal);
    // printf("errno is %d\n", errno);

    //unmap memory so there is no memory leaking
    int retVal5 = munmap(screen_buffer, sb_size);
    int retVal3 = munmap(offscreen_buffer, sb_size);

    //close files
    int retVal4 = close(frameBuffer);

    //null out pointers. 
    screen_buffer = NULL;
    offscreen_buffer = NULL;

    //It may be a good idea to clear the screen one last time. 
}

char getkey() {
    char key = 0;
    int fD = 0; //fD is the file descriptor. 0 for keyboard input. 
    int nfds = 1; //number of file descriptors. 1 since we are only worried about keyboard input. 
    fd_set fs; //declare a fd_set named fs


    FD_ZERO(&fs);
    FD_SET(fD, &fs);

    struct timeval time; 
    time.tv_sec = 0;
    time.tv_usec = 0;

    //int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
    int numberOfKeysReady = select(nfds, &fs, NULL, NULL, &time); //Look up man pages for select(2)

    //if numberOfKeysReady == 1 read from the file descriptor
    // use read sys call to get the key pressed 
    if (numberOfKeysReady > 0)
    {
        //ssize_t read(int fd, void *buf, size_t count);
       int retVal = read(fD, &key, 1);
       // printf("retVal from Read is %d\n", retVal);
       // printf("Errno is %d\n", errno);
        // printf("key pressed is %d\n", key);
    }
    return key; //value of key is the decimal value of the character. See ascii table. 
}

/* - Max value of ms is 999. Any more than that will cause invalid arguments to nanosleep()
    - This is because 1,000,000,000 ns = 1 second. If calling nanosleep() with more than 1,000,000,000 ns 
    in the tv_nsec parameter it is invalid because you shoulld use the tim.tv_sec parameter of the timespec struct. 
*/
void sleep_ms(long ms) {
	int nanoSeconds = ms * 1000000; 
	struct timespec tim;
	tim.tv_sec = 0;
	tim.tv_nsec = ms * 1000000L;
	int retval = nanosleep(&tim, NULL);
}

void clear_screen(void *img) {
	int i;
    for (i = 0; i < sb_size; i++) {
       *((char*) img+i) = 0; //iterate over the image and set each byte to 0;
    }
}

void draw_pixel(void *img, int x, int y, color_t color) {
    int offset = (x + (y * (xRez/2))) * 2; //*2 for Each pixel is 2 bytes? 
	color_t* pixelAddr = img + (offset);
    *pixelAddr = color;
}

void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c) {
	
}

void *new_offscreen_buffer() {
    int fD = -1; //Use File descriptor of -1 to call mmap() like malloc()
    //Use MAP_PRIVATE | MAP_ANONYMOUS to use mmap() like malloc()
	offscreen_buffer = mmap(NULL, sb_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fD, 0);

    return offscreen_buffer;
}

void blit(void *src) {
	int i;
    for (i = 0; i < sb_size; i++) {
        //set the value at screenbuffer[i] to the value of the src[i]
        *(screen_buffer + i) = *((char*) src + i); //Is this really copying memory? 
    }

}


//Add funciton to clear offscreen buffer.
