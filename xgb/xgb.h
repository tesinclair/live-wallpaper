#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct{
    Display* display;
    Screen* screen;
    XImage* ximg;
    Visual* visual;
    unsigned_int depth;
    GC gCtx;

} X11Conn;

typedef struct{
    unsigned char* data;
    unsigned int width;
    unsigned int height;
    unsigned int channels;
} XFrame;

int setup_x11(X11Conn* X);
int teardown_x11(X11Conn* X);

void get_screen_width(Screen* s);
void get_screen_height(Screen* s);

int send_frame(X11Conn* X, Frame* frame);

