/*
 *                           0BSD 
 * 
 *                    BSD Zero Clause License
 * 
 *  Copyright (c) 2019 Hermann Meyer
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "xputty.h"


/**
 * @brief main_init         - init the Xputty struct
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */


void main_init(Xputty *main) {
    main->dpy = XOpenDisplay(0);
    assert(main->dpy);
    main->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(main->childlist);
    childlist_init(main->childlist);
    main->run = true;
}

/**
 * @brief main_run           - the event loop
 * @param *main              - pointer to the main Xputty struct
 * @return void 
 */

void main_run(Xputty *main) {
    Widget_t * wid = main->childlist->childs[0]; 
    Atom WM_DELETE_WINDOW;
    WM_DELETE_WINDOW = XInternAtom(wid->dpy, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(wid->dpy, wid->widget, &WM_DELETE_WINDOW, 1);

    XEvent xev;
    XPointer w_;
    int ew;

    while (main->run && (XNextEvent(wid->dpy, &xev)>=0)) {
        ew = childlist_find_widget(main->childlist, xev.xany.window);
        if(ew  >= 0) {
            Widget_t * w = main->childlist->childs[ew];
            w->event_callback(w,&xev,NULL);
        }

        switch (xev.type) {
            case ClientMessage:
                /* delete window event */
                if (xev.xclient.data.l[0] == WM_DELETE_WINDOW)
                    main->run = false;
            break;
        }
    }
}

/**
 * @brief main_quit         - clean up afterr quiting the main loop
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */

void main_quit(Xputty *main) {
    int i = main->childlist->elem-1;
    for(;i>-1;i--) {
        Widget_t *w = main->childlist->childs[i];
        destroy_widget(w, main);
    }
    childlist_destroy(main->childlist);
    free(main->childlist);
    XCloseDisplay(main->dpy);
    debug_print("quit\n");
}
