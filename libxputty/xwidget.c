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


#include "xwidget.h"

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
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Down))
        return 5;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Left))
        return 6;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Home))
        return 7;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Insert))
        return 8;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_End))
        return 9;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Return))
        return 10;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_BackSpace))
        return 11;
    // keypad
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Subtract))
        return 1;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Add))
        return 2;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Up))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Right))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Down))
        return 5;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Left))
        return 6;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Home))
        return 7;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Insert))
        return 8;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_End))
        return 9;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Enter))
        return 10;
    else return 0;
}

/**
 * @brief destroy_widget    - destroy a widget and remove it from the Context
 * @param *w                - pointer to the Widget_t sending the request
 * @param *main             - pointer to main struct
 * @return void 
 */

void destroy_widget(Widget_t * w, Xputty *main) {
    int count = childlist_find_child(main->childlist, w);
    if (count == 0 && main->run == true) {
        quit(w);
    } else if(childlist_find_child(main->childlist, w)>=0) {
        childlist_remove_child(main->childlist, w);
        if(w->is_widget) {
            Widget_t *p = (Widget_t *) w->parent;
            childlist_remove_child(p->childlist, w);
        }
        delete_adjustment(w->adj_x);
        delete_adjustment(w->adj_y);
        childlist_destroy(w->childlist);
        cairo_destroy(w->crb);
        cairo_surface_destroy(w->buffer);
        cairo_destroy(w->cr);
        cairo_surface_destroy(w->surface);
        
        XDestroyIC(w->xic);
        XCloseIM(w->xim);
        XUnmapWindow(w->dpy, w->widget);
        XDestroyWindow(w->dpy, w->widget);
        free(w->childlist);
        free(w);
    }
}

/**
 * @brief _resize_surface    - intern check if surfaces needs resizing
 * @param *wid               - pointer to the Widget_t receive the event
 * @param width              - the new width
 * @param height             - the new height
 * @return void 
 */

static inline void _resize_surface(Widget_t *wid, int width, int height) {
    wid->width = width;
    wid->height = height;
    cairo_xlib_surface_set_size( wid->surface, wid->width, wid->height);
    cairo_destroy(wid->crb);
    cairo_surface_destroy(wid->buffer);
    wid->buffer = cairo_surface_create_similar (wid->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    wid->crb = cairo_create (wid->buffer);
}

/**
 * @brief _resize_childs     - intern check if child widgets needs resizing
 * @param *wid               - pointer to the Widget_t receive the event
 * @return void 
 */

static inline void _resize_childs(Widget_t *wid) {
    if(!childlist_has_child(wid->childlist)) return;
    int i = 0;
    for(;i<wid->childlist->elem;i++) {
        Widget_t *w = wid->childlist->childs[i];
        switch(w->scale.gravity) {
            case(NORTHWEST):
                XResizeWindow (w->dpy, w->widget, max(1,
                    w->scale.init_width - (wid->scale.scale_x)), 
                    max(1,w->scale.init_height - (wid->scale.scale_y)));
            break;
            case(NORTHEAST):
                XResizeWindow (w->dpy, w->widget, max(1,
                    w->scale.init_width - (wid->scale.scale_x)), w->scale.init_height);
            break;
            case(SOUTHWEST):

            XMoveWindow(wid->dpy,w->widget,w->scale.init_x,
                                        w->scale.init_y-wid->scale.scale_y);
            
            break;
            case(SOUTHEAST):
                XMoveWindow(wid->dpy,w->widget,w->scale.init_x,
                                            w->scale.init_y-wid->scale.scale_y);
            break;
            case(CENTER):
                XMoveWindow(w->dpy,w->widget,w->scale.init_x /
                    wid->scale.cscale_x,w->scale.init_y / wid->scale.cscale_y);
                XResizeWindow (w->dpy, w->widget, max(1,
                    w->scale.init_width / (wid->scale.cscale_x)), 
                    max(1,w->scale.init_height / (wid->scale.cscale_y)));
            break;
            case(ASPECT):
                XMoveWindow(w->dpy,w->widget,w->scale.init_x /
                    wid->scale.ascale,w->scale.init_y / wid->scale.ascale);
                XResizeWindow (w->dpy, w->widget, max(1,
                    w->scale.init_width / (wid->scale.ascale)), 
                    max(1,w->scale.init_height / (wid->scale.ascale)));
            
            break;
            case(NONE):
            XMoveWindow(wid->dpy,w->widget,w->scale.init_x-wid->scale.scale_x,
                                        w->scale.init_y-wid->scale.scale_y);
            break;
            default:
            break;
        }
        
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
    if (wid->width != attrs.width || wid->height != attrs.height) {
        wid->scale.scale_x    = (float)wid->scale.init_width - attrs.width;
        wid->scale.scale_y    = (float)wid->scale.init_height - attrs.height;
        wid->scale.cscale_x   = (float)(((float)wid->scale.init_width/(float)attrs.width));
        wid->scale.cscale_y   = (float)(((float)wid->scale.init_height/(float)attrs.height));
        wid->scale.ascale     = wid->scale.cscale_x < wid->scale.cscale_y ? 
                                wid->scale.cscale_y : wid->scale.cscale_x;

        _resize_surface(wid, attrs.width, attrs.height); 

        debug_print("Widget_t configure callback width %i height %i\n", attrs.width, attrs.height);

        _resize_childs(wid);
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
 * @param win               - pointer to the Parrent Window (may be Root)
 * @param x,y,width,height   - the position/geometry to create the window
 * @return Widget_t*         - pointer to the Widget_t struct
 */

Widget_t *create_window(Xputty *app, Window win,
                          int x, int y, int width, int height) {

    Widget_t *w = (Widget_t*)malloc(sizeof(Widget_t));
    assert(w != NULL);
    debug_print("assert(w)\n");
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = 0;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(app->dpy, win , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent, &attributes);
    debug_print("XCreateWindow\n");

    XSetLocaleModifiers("");
    w->xim = XOpenIM(app->dpy, 0, 0, 0);
    if(!w->xim){
        XSetLocaleModifiers("@im=none");
        w->xim = XOpenIM(app->dpy, 0, 0, 0);
    }

    w->xic = XCreateIC(w->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                    XNClientWindow, w->widget, XNFocusWindow,  w->widget, NULL);

    XSetICFocus(w->xic);

    XSelectInput(app->dpy, w->widget, event_mask);

    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize|PWinGravity;
    win_size_hints->min_width = width/2;
    win_size_hints->min_height = height/2;
    win_size_hints->win_gravity = CenterGravity;
    XSetWMNormalHints(app->dpy, w->widget, win_size_hints);
    XFree(win_size_hints);

    w->surface =  cairo_xlib_surface_create (app->dpy, w->widget,  
                  DefaultVisual(app->dpy, DefaultScreen(app->dpy)), width, height);

    w->cr = cairo_create(w->surface);

    w->buffer = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    w->crb = cairo_create (w->buffer);

    w->is_widget = false;
    w->dpy = app->dpy;
    w->parent = &win;
    w->label = NULL;
    w->state = 0;
    w->data = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->scale.init_x = x;
    w->scale.init_y = y;
    w->scale.init_width = width;
    w->scale.init_height = height;
    w->scale.scale_x  = 0.0;
    w->scale.scale_y  = 0.0;
    w->scale.cscale_x = 1.0;
    w->scale.cscale_y = 1.0;
    w->scale.ascale   = 1.0;
    w->scale.gravity  = NONE;
    w->transparency = false;
    w->adj_x = NULL;
    w->adj_y = NULL;
    w->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(w->childlist != NULL);
    childlist_init(w->childlist);
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

    childlist_add_child(app->childlist,w);
    XMapWindow(app->dpy, w->widget);
    return w;
}

/**
 * @brief *create_widget      - create a widget
 * @param *dpy                - pointer to the Display to use
 * @param *parent             - pointer to the Parrent Widget_t
 * @param x,y,width,height    - the position/geometry to create the widget
 * @return Widget_t*          - pointer to the Widget_t struct
 */

Widget_t *create_widget(Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {

    Widget_t *w = (Widget_t*)malloc(sizeof(Widget_t));
    assert(w != NULL);
    debug_print("assert(w)\n");
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = True;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(app->dpy, parent->widget , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent|CWOverrideRedirect, &attributes);
    debug_print("XCreateWindow\n");

    XSetLocaleModifiers("");
    w->xim = XOpenIM(app->dpy, 0, 0, 0);
    if(!w->xim){
        XSetLocaleModifiers("@im=none");
        w->xim = XOpenIM(app->dpy, 0, 0, 0);
    }

    w->xic = XCreateIC(w->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                    XNClientWindow, w->widget, XNFocusWindow,  w->widget, NULL);

    XSetICFocus(w->xic);

    XSelectInput(app->dpy, w->widget, event_mask);

    w->surface =  cairo_xlib_surface_create (app->dpy, w->widget,  
                  DefaultVisual(app->dpy, DefaultScreen(app->dpy)), width, height);

    w->cr = cairo_create(w->surface);

    w->buffer = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    
    w->crb = cairo_create (w->buffer);

    w->is_widget = true;
    w->dpy = app->dpy;
    w->parent = parent;
    w->label = NULL;
    w->state = 0;
    w->data = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->scale.gravity = CENTER;
    w->scale.init_width = width;
    w->scale.init_height = height;
    w->scale.init_x   = x;
    w->scale.init_y   = y;
    w->scale.scale_x  = 0.0;
    w->scale.scale_y  = 0.0;
    w->scale.cscale_x = 1.0;
    w->scale.cscale_y = 1.0;
    w->scale.ascale   = 1.0;
    w->transparency = true;
    w->adj_x = NULL;
    w->adj_y = NULL;
    w->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(w->childlist != NULL);
    childlist_init(w->childlist);
    childlist_add_child(parent->childlist, w);
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

    childlist_add_child(app->childlist,w);
    XMapWindow(app->dpy, w->widget);
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
 * @brief _set_adj_value    - set value to adjustment from key event
 * @param *w                - pointer to the Widget_t receiving the event
 * @param x                 - use x or y-axis
 * @return void
 */

static inline void _set_adj_value(void *w_, bool x, int direction) {
    Widget_t *wid = (Widget_t*)w_;
    if (x && wid->adj_x) {
        wid->adj_x->value = min(wid->adj_x->max_value,max(wid->adj_x->min_value, 
        wid->adj_x->value + (wid->adj_x->step * direction)));
       // expose_widget(wid);
    } else if (!x && wid->adj_y) {
        wid->adj_y->value = min(wid->adj_y->max_value,max(wid->adj_y->min_value, 
        wid->adj_y->value + (wid->adj_y->step * direction)));
      //  expose_widget(wid);
    }
}

/**
 * @brief _check_keymap     - check if key is in map, send requests if so 
 * @param *w                - pointer to the Widget_t receiving the event
 * @param xkey              - the XKeyEvent to check
 * @return void
 */

static inline void _check_keymap (void *w_ ,XKeyEvent xkey) {
    Widget_t *wid = (Widget_t*)w_;

    int nk = key_mapping(wid->dpy, &xkey);
    if (nk) {
        switch (nk) {
            case 3: _set_adj_value(w_, false, 1);
            break;
            case 4: _set_adj_value(w_, true, 1);
            break;
            case 5: _set_adj_value(w_, false, -1);
            break;
            case 6: _set_adj_value(w_, true, -1);
            break;
            default:
            break;
        }
    }
}

/**
 * @brief expose_widgets    - send expose expose event to window
 * @param w                 - the Widget_t to send the event to
 * @return void 
 */

void expose_widget(Widget_t *w) {
    XEvent exp;
    memset(&exp, 0, sizeof(exp));
    exp.type = Expose;
    exp.xexpose.window = w->widget;
    XSendEvent(w->dpy, w->widget, False, ExposureMask, (XEvent *)&exp);
}

/**
 * @brief _propagate_childs - send expose to child window
 * @param *wid              - pointer to the Widget_t send the event
 * @return void 
 */

static inline void _propagate_child_expose(Widget_t *wid) {

    if (childlist_has_child(wid->childlist)) {
        int i = 0;
        for(;i<wid->childlist->elem;i++) {
            Widget_t *w = wid->childlist->childs[i];
            if (w->transparency)
                expose_widget(w);
        }
    }      
}

/**
 * @brief transparent_draw  - copy parent surface to child surface
 * @param *wid              - pointer to the Widget_t receiving a event
 * @param *user_data        - void pointer to attached user_data
 * @return void 
 */

void transparent_draw(void * w_, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;

    cairo_push_group (wid->cr);

    if (wid->transparency) {
        Widget_t *parent = (Widget_t*)wid->parent;
        XWindowAttributes attrs;
        XGetWindowAttributes(wid->dpy, wid->widget, &attrs);

        debug_print("Widget_t _transparency \n");
        cairo_set_source_surface (wid->crb, parent->buffer, -attrs.x, -attrs.y);
        cairo_paint (wid->crb);
    }

    cairo_push_group (wid->crb);
    wid->func.expose_callback(wid, user_data);
    cairo_pop_group_to_source (wid->crb);
    cairo_paint (wid->crb);

    cairo_set_source_surface (wid->cr, wid->buffer,0,0);
    cairo_paint (wid->cr);

    cairo_pop_group_to_source (wid->cr);
    cairo_paint (wid->cr);
    _propagate_child_expose(wid);
}

/**
 * @brief widget_event_loop - the internal widget event loop
 * @param *w                - pointer to the Widget_t receiving a event
 * @param *event            - void pointer to the XEvent
 * @param *user_data        - void pointer to attached user_data
 * @return void 
 */

void widget_event_loop(void *w_, void* event, Xputty *main, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;
    XEvent *xev = (XEvent*)event;
    
    switch(xev->type) {
        case ConfigureNotify:
            wid->func.configure_callback(w_, user_data);
            transparent_draw(w_, user_data);
            debug_print("Widget_t ConfigureNotify \n");
        break;

        case Expose:
            if (xev->xexpose.count == 0) {
                transparent_draw(w_, user_data);
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
            _check_keymap(wid, xev->xkey);
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

        case ClientMessage:
            if (xev->xclient.message_type == XInternAtom(wid->dpy, "WIDGET_DESTROY", 1)) {
                int ch = childlist_has_child(wid->childlist);
                if (ch) {
                    int i = ch;
                    for(;i>0;i--) {
                        destroy_widget(wid->childlist->childs[i-1],main);
                    }
                    destroy_widget(wid,main);
                } else {
                    destroy_widget(wid,main);
                }
            }

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
 * @brief quit_widget       - remove a widget from the processing loop
 * @param *w                - pointer to the Widget_t sending the request
 * @return void 
 */

void quit_widget(Widget_t *w) {
    Atom QUIT_WIDGET = XInternAtom(w->dpy, "WIDGET_DESTROY", False);
    XClientMessageEvent xevent;
    xevent.type = ClientMessage;
    xevent.message_type = QUIT_WIDGET;
    xevent.display = w->dpy;
    xevent.window = w->widget;
    xevent.format = 16;
    xevent.data.l[0] = 1;
    XSendEvent(w->dpy, w->widget, 0, 0, (XEvent *)&xevent);
}

