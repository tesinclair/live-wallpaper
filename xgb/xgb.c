#include "xgb.h"

int setup_x11(X11Conn* X){
    X->display = XOpenDisplay(NULL);
    if (X->display == NULL){
        // TODO: Log error
        return 1;
    }

    X->screen = DefaultScreenOfDisplay(X->display);
    if (X->screen == NULL){
        // TODO: Log error
        return 1;
    }

    X->window = DefaultRootWindow(X->display);
    X->visual = DefaultVisual(X->display, DefaultScreen(X->display));
    if (X->visual == NULL){
        // TODO: Log error
        return 1;
    }
    X->depth = DefaultDepth(X->display, DefaultScreen(X->display));
    X->pixmap = XCreatePixmap(X->display, X->window,
                    WidthOfScreen(X->screen), HeightOfScreen(X->screen), X->depth);
    X->gCtx = XCreateGC(X->display, X->window, 0, NULL);
    if (X->gCtx == NULL){
        // TODO: Log error
        return 1;
    }
    return 0;
}

int teardown_x11(X11Conn* X){
    XCloseDisplay(X.display);
    if (X.display != NULL){
        // TODO: Log error
        return 1;
    }
}

unsigned int get_screen_width(Screen* s){
    return (unsigned int)WidthOfScreen(*s)
}

unsigned int get_screen_height(Screen* s){
    return (unsigned int)HeightOfScreen(*s)
}

int send_frame(X11Conn *X, Frame* frame){
    if (X->ximg != NULL){
        XDestroyImage(X->ximg);
        if (X->ximg != NULL){
            // TODO: Log error
            return 1;
        }
    }
    X->ximg = XCreateImage(X->display, X->visual, X->depth, ZPixmap, 0,
                           (char *)frame->data, frame->width, frame->height, 32, 0);
    if (x_img == NULL){
        // TODO: Log error
        return 1;
    }

    XPutImage(X->display, X->pixmap, X->gCtx, x_img, 0, 0, 0, 0,
               frame->width, frame->height);
    XSetWindowBackgroundPixmap(X->display, X->window, X->pixmap);
    XClearWindow(X->display, X->window);
    XFlush(X->display);
    XSync(X->display, False);
    return 0;
}
