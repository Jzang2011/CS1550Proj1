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

#include <errno.h>

struct termios termSettings;
char* screen_buffer; 
char* offscreen_buffer; //you can get rid of this.
int sb_size; //size of the screen_buffer;
int yRez;
int xRez;
int xLineLength;
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
         write(1, "ERROR: open() failed\n", 23);
    }

    /* 3  Use typedef to make a color type color_t (declared in graphics.h)
        - Use ioctl() to get screen size and bits per pixels.
        - Compute length of frameBuffer by yRez * line_length
    */
    ioctl(frameBuffer, FBIOGET_VSCREENINFO, &vInfo);
    ioctl(frameBuffer, FBIOGET_FSCREENINFO, &fInfo);

    yRez = vInfo.yres_virtual;
    xLineLength = fInfo.line_length;
    xRez = xLineLength / 2;

    sb_size = yRez * xLineLength;
 
    /* 2. memory mapping mmap()
        - void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
        - length = yrez_virtual * line_length?
    */
    screen_buffer = (char*) mmap(NULL, sb_size, PROT_READ | PROT_WRITE ,MAP_SHARED, frameBuffer, 0);
    if(screen_buffer == MAP_FAILED) {
        write(1, "ERROR: mmap failed\n", 21);
    }

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
    ioctl(0, TCGETS, &termSettings); // loads terminal settings into termSettings. 
    
    /* 
    6. Disable canonical mode by unsetting the ICANON bit and disaling ECHO by forcing that bit to zero.
     - Unset the ICANON bit. This can be done by setting it to its complement (~)
     - Forces the ECHO bit to zero as well.
    */
    termSettings.c_lflag &= ~(ICANON | ECHO); //c_lflag = c_lflag AND !(ICANON OR ECHO)
    ioctl(0, TCSETS, &termSettings);
}

void exit_graphics() {
    int fD = 0; //fD = file Descriptor for keyboard. 
    /*
        - Undo disabling canonical mode
        - make call to ioctl() to reenable  echoing and buffering
        - Basically undo step 5 in init_graphics()
    */

    //Sets the settings back to the old settings which were saved in init.
    ioctl(fD, TCGETS, &termSettings);
    termSettings.c_lflag |= (ICANON | ECHO); 

    ioctl(fD, TCSETS, &termSettings);

    //unmap memory so there is no memory leaking
    munmap(screen_buffer, sb_size);
    munmap(offscreen_buffer, sb_size);

    //close files
    close(frameBuffer);

    //null out pointers. 
    screen_buffer = NULL;
    offscreen_buffer = NULL;
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
        read(fD, &key, 1);
    }
    return key; //value of key is the decimal value of the character. See ascii table. 
}

/* - Max value of ms is 999. Any more than that will cause invalid arguments to nanosleep()
    - This is because 1,000,000,000 ns = 1 second. If calling nanosleep() with more than 1,000,000,000 ns 
    in the tv_nsec parameter it is invalid because you shoulld use the tim.tv_sec parameter of the timespec struct. 
*/
void sleep_ms(long ms) {
	struct timespec tim;
	tim.tv_sec = 0;
	tim.tv_nsec = ms * 1000000L;
    nanosleep(&tim, NULL);
}

void clear_screen(void *img) {
	int i;
    for (i = 0; i < sb_size; i++) {
       *((char*) img+i) = 0; //iterate over the image and set each byte to 0;
    }
}

void draw_pixel(void *img, int x, int y, color_t color) {
    int offset = (x + (y * xRez)) * 2; //*2 for Each pixel is 2 bytes? 
	color_t* pixelAddr = img + (offset);
    *pixelAddr = color;
}

int abs(int x) {
    if (x < 0)
    {
        x = x * -1;
    }
    return x;
}

/*
    The folowing code has been taken and modified slightly 
    from: http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
*/
void draw_line(void *img, int x, int y, int x2, int y2, color_t c) {
    int w = x2 - x;
    int h = y2 - y;
    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
    if (w<0) dx1 = -1; else if (w>0) dx1 = 1;
    if (h<0) dy1 = -1; else if (h>0) dy1 = 1;
    if (w<0) dx2 = -1; else if (w>0) dx2 = 1;
    int longest = abs(w);
    int shortest = abs(h);
    if (!(longest>shortest)) {
        longest = abs(h);
        shortest = abs(w);
        if (h<0) dy2 = -1; else if (h>0) dy2 = 1;
        dx2 = 0;            
    }
    int numerator = longest >> 1;
    int i;
    for (i=0;i<=longest;i++) {
        draw_pixel(img, x, y, c);
        numerator += shortest ;
        if (!(numerator<longest)) {
            numerator -= longest;
            x += dx1;
            y += dy1;
        } else {
            x += dx2;
            y += dy2;
        }
    }
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

int get_xrez() {
    return xRez;
}

int get_yrez() {
    return yRez;
}
