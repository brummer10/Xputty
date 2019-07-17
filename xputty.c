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
 * @brief key_mapping       - modifier key's mapped to a integer value
 * @param *dpy              - pointer to the Display in use
 * @param *xkey             - the key to map
 * @return int              - value (1-10) or 0 when not mapped 
 */

int key_mapping(Display *dpy, XKeyEvent *xkey) {
    if (xkey->keycode == XKeysymToKeycode(dpy,XK_Tab))
        return (xkey->state == ShiftMask) ? 1 : 2;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Up))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Right))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Down))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Left))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Home))
        return 5;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Insert))
        return 6;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_End))
        return 7;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Return))
        return 8;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_BackSpace))
        return 9;
    // keypad
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Subtract))
        return 1;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Add))
        return 2;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Up))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Right))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Down))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Left))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Home))
        return 5;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Insert))
        return 6;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_End))
        return 7;
    else return 0;
}

/**
 * @brief destroy_widget    - destroy a widget and remove it from the Context
 * @param *w                - pointer to the Widget_t sending the request
 * @param Context           - the Context holding the widget info
 * @return void 
 */

void destroy_widget(Widget_t * w, XContext Context) {
    XPointer w_;
    if(!XFindContext(w->dpy, w->widget, Context,  &w_)) {
        w->adj_x = delete_adjustment(w->adj_x);
        w->adj_y = delete_adjustment(w->adj_y);
        cairo_destroy(w->cr);
        cairo_surface_destroy(w->surface);
        XDeleteContext(w->dpy, w->widget, Context);
        XUnmapWindow(w->dpy, w->widget);
        XDestroyWindow(w->dpy, w->widget);
        free(w);
    }
}

/**
 * @brief configure_event    - default callback when a widget receive a ConfigureNotify
 * @param *w                 - pointer to the Widget_t receive the event
 * @param user_data          - void pointer to attached user_data
 * @return void 
 */

void configure_event(void *w_, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(wid->dpy, (Window)wid->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    wid->scale_x = (float)wid->width-width;
    wid->scale_y = (float)wid->height-height;
    wid->width = width;
    wid->height = height;
    debug_print("Widget_t configure callback width %i height %i\n", width, height);
    cairo_xlib_surface_set_size( wid->surface, width, height);
    Window root, parent, *children;
    int num_children;
    int status = XQueryTree(wid->dpy, wid->widget, &root, &parent,
                            &children, &num_children);
    if (status) {
        XSizeHints hints;
        memset(&hints, 0, sizeof(hints));
        long supplied;
        int i = num_children -1;
        for (; i >= 0; i--) {
            XGetWindowAttributes(wid->dpy, (Window)children[i], &attrs);
            XGetWMNormalHints(wid->dpy,(Window)children[i],&hints, &supplied);
            debug_print("Widget_t win_gravity = %i\n", hints.win_gravity);
            if(hints.win_gravity == CenterGravity) {
                XResizeWindow (wid->dpy, children[i], max(max(1,hints.min_width),attrs.width-(wid->scale_x)), max(max(1,hints.min_height),attrs.height-(wid->scale_y)));
            } else {
                XMoveWindow(wid->dpy,children[i],attrs.x-wid->scale_x,attrs.y-wid->scale_y);
            }
        }
        debug_print("Widget_t find %i\n ", num_children);
        XFree(children);
    }
}

/**
 * @brief dummy1_callback    - default debuging callback for evfunc's
 * @param *w                 - pointer to the Widget_t receive the event
 * @param user_data          - void pointer to attached user_data
 * @return void 
 */

static inline void dummy1_callback(void *w_, void* _data, void* user_data) {
    debug_print("Widget_t dummy callback\n");
}

/**
 * @brief dummy1_callback    - default debuging callback for xevfunc's
 * @param *w                 - pointer to the Widget_t receive the event
 * @param user_data          - void pointer to attached user_data
 * @return void 
 */

static inline void dummy_callback(void *w_, void* user_data) {
    debug_print("Widget_t dummy callback\n");
}

/**
 * @brief *create_window     - create a Window 
 * @param *dpy               - pointer to the Display to use
 * @param *win               - pointer to the Parrent Window (may be Root)
 * @param Context            - a XContext to store Window informations
 * @param x,y,width,height   - the position/geometry to create the window
 * @return Widget_t*         - pointer to the Widget_t struct
 */

Widget_t *create_window(Display *dpy, Window win, XContext Context,
                          int x, int y, int width, int height) {

    Widget_t *w = (Widget_t*)malloc(sizeof(Widget_t));
    assert(w);
    debug_print("assert(w)\n");
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = 0;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(dpy, win , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent, &attributes);
    debug_print("XCreateWindow\n");

    XSelectInput(dpy, w->widget, event_mask);

    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize;
    win_size_hints->min_width = width/2;
    win_size_hints->min_height = height/2;
    XSetWMNormalHints(dpy, w->widget, win_size_hints);
    XFree(win_size_hints);

    w->surface =  cairo_xlib_surface_create (dpy, w->widget,  
                  DefaultVisual(dpy, DefaultScreen(dpy)), width, height);

    w->cr = cairo_create(w->surface);
    if(XSaveContext(dpy, w->widget, Context, (XPointer) w)) {
        debug_print("contex save faild\n");
    }
    w->dpy = dpy;
    w->label = NULL;
    w->state = 0;
    w->data = 0;
    w->width = width;
    w->height = height;
    w->scale_x = 1.0;
    w->scale_y = 1.0;
    w->adj_x = NULL;
    w->adj_y = NULL;
    w->event_callback = widget_event_loop;
    w->func.expose_callback = dummy_callback;
    w->func.configure_callback = configure_event;
    w->func.button_press_callback = dummy1_callback;
    w->func.button_release_callback = dummy1_callback;
    w->func.motion_callback = dummy1_callback;
    w->func.key_press_callback = dummy1_callback;
    w->func.key_release_callback = dummy1_callback;
    w->func.enter_callback = dummy_callback;
    w->func.leave_callback = dummy_callback;
    w->func.user_callback = dummy_callback;

    XMapWindow(dpy, w->widget);
    return w;
}

/**
 * @brief *create_widget      - create a widget
 * @param *dpy                - pointer to the Display to use
 * @param *win                - pointer to the Parrent Window 
 * @param Context             - a XContext to store Window informations
 * @param x,y,width,height    - the position/geometry to create the widget
 * @return Widget_t*          - pointer to the Widget_t struct
 */

Widget_t *create_widget(Display *dpy, Window win, XContext Context,
                          int x, int y, int width, int height) {

    Widget_t *w = (Widget_t*)malloc(sizeof(Widget_t));
    assert(w);
    debug_print("assert(w)\n");
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = True;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(dpy, win , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent|CWOverrideRedirect, &attributes);
    debug_print("XCreateWindow\n");

    XSelectInput(dpy, w->widget, event_mask);

    w->surface =  cairo_xlib_surface_create (dpy, w->widget,  
                  DefaultVisual(dpy, DefaultScreen(dpy)), width, height);

    w->cr = cairo_create(w->surface);
    if(XSaveContext(dpy, w->widget, Context, (XPointer) w)) {
        debug_print("contex save faild\n");
    }
    w->dpy = dpy;
    w->label = NULL;
    w->state = 0;
    w->data = 0;
    w->width = width;
    w->height = height;
    w->scale_x = 1.0;
    w->scale_y = 1.0;
    w->adj_x = NULL;
    w->adj_y = NULL;
    w->event_callback = widget_event_loop;
    w->func.expose_callback = dummy_callback;
    w->func.configure_callback = configure_event;;
    w->func.button_press_callback = dummy1_callback;
    w->func.button_release_callback = dummy1_callback;
    w->func.motion_callback = dummy1_callback;
    w->func.key_press_callback = dummy1_callback;
    w->func.key_release_callback = dummy1_callback;
    w->func.enter_callback = dummy_callback;
    w->func.leave_callback = dummy_callback;
    w->func.user_callback = dummy_callback;

    XMapWindow(dpy, w->widget);
    return w;
}

/**
 * @brief _has_pointer      - check if the widget has the pointer on Button release 
 * @param *w                - pointer to the Widget_t sending the request
 * @param *button           - pointer to the XButtonEvent sending the notify
 * @return bool             - true if pointer is in widget 
 */

static inline bool _has_pointer(Widget_t *w, XButtonEvent *button) {
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    
    if ((button->x<attrs.width && button->y<attrs.height) &&
                                (button->x>0 && button->y>0)){
        return true;
    } else {
        return false;
    }

}

/**
 * @brief widget_event_loop - the internal widget event loop
 * @param *w                - pointer to the Widget_t receiving a event
 * @param *event            - void pointer to the XEvent
 * @param *user_data        - void pointer to attached user_data
 * @return void 
 */

void widget_event_loop(void *w_, void* event, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;
    XEvent *xev = (XEvent*)event;
    
    switch(xev->type) {
        case ConfigureNotify:
            wid->func.configure_callback(w_, user_data);
            debug_print("Widget_t ConfigureNotify \n");
        break;

        case Expose:
            if (xev->xexpose.count == 0) {
                wid->func.expose_callback(w_, user_data);
                debug_print("Widget_t Expose \n");
            }
        break;

        case ButtonPress:
            wid->state = 2;
            wid->has_pointer = _has_pointer(wid, &xev->xbutton);
            wid->pos_x = xev->xbutton.x;
            wid->pos_y = xev->xbutton.y;
            adj_set_start_value(wid);
            wid->func.button_press_callback(w_, &xev->xbutton, user_data);
            debug_print("Widget_t  ButtonPress %i\n", xev->xbutton.button);
        break;

        case ButtonRelease:
            wid->state = 0;
            wid->has_pointer = _has_pointer(wid, &xev->xbutton);
            wid->func.button_release_callback(w_, &xev->xbutton, user_data);
            debug_print("Widget_t  ButtonRelease %i\n", xev->xbutton.button);
        break;

        case KeyPress:
            wid->func.key_press_callback(w_, &xev->xkey, user_data);
            debug_print("Widget_t KeyPress %u\n", xev->xkey.keycode);
        break;

        case KeyRelease:
            wid->func.key_release_callback(w_, &xev->xkey, user_data);
            debug_print("Widget_t KeyRelease %u\n", xev->xkey.keycode);
        break;

        case LeaveNotify:
            if(!(xev->xcrossing.state & Button1Mask)) {
                wid->state = 0;
                wid->func.leave_callback(w_, user_data);
            }
            debug_print("Widget_t LeaveNotify \n");
        break;

        case EnterNotify:
            if(!(xev->xcrossing.state & Button1Mask)) {
                wid->state = 1;
                wid->func.enter_callback(w_, user_data);
            }
            debug_print("Widget_t EnterNotify \n");
        break;

        case MotionNotify:
            adj_set_state(wid, xev->xmotion.x, xev->xmotion.y);
            wid->func.motion_callback(w_,&xev->xmotion, user_data);
            debug_print("Widget_t MotionNotify x = %i Y = %i \n",xev->xmotion.x,xev->xmotion.y );
        break;

        default:
        break;
    }
}

/**
 * @brief quit              - exit the main loop
 * @param *w                - pointer to the Widget_t sending the request
 * @return void 
 */

void quit(Widget_t *w) {
    Atom WM_DELETE_WINDOW = XInternAtom(w->dpy, "WM_DELETE_WINDOW", True);
    XClientMessageEvent xevent;
    xevent.type = ClientMessage;
    xevent.message_type = WM_DELETE_WINDOW;
    xevent.display = w->dpy;
    xevent.window = w->widget;
    xevent.format = 16;
    xevent.data.l[0] = WM_DELETE_WINDOW;
    XSendEvent(w->dpy, w->widget, 0, 0, (XEvent *)&xevent);
}

/**
 * @brief loop              - the event loop
 * @param *w                - pointer to the main Window
 * @param Context           - the Context holding all the widget infos
 * @param *run              - pointer to bool used to quit the loop
 * @return void 
 */

void loop(Widget_t *w, XContext context, bool *run) {
    Widget_t * wid = (Widget_t*)w;
    Atom WM_DELETE_WINDOW;
    WM_DELETE_WINDOW = XInternAtom(wid->dpy, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(wid->dpy, wid->widget, &WM_DELETE_WINDOW, 1);

    XEvent xev;
    XPointer w_;
    while (*run && (XNextEvent(wid->dpy, &xev)>=0)) {
        if(!XFindContext(wid->dpy, xev.xany.window, context,  &w_)) {
            Widget_t * wid = (Widget_t*)w_;
            wid->event_callback(wid,&xev,NULL);
        }
    
    switch (xev.type) {
        case ClientMessage:
        /* delete window event */
        if (xev.xclient.data.l[0] == WM_DELETE_WINDOW)
          (*run) = false;
          break;
        }
    }
}

