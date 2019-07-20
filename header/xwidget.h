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


#pragma once

#ifndef XWIDGET_H
#define XWIDGET_H

#include "xputty.h"

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
					basic X11 widgets
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief *evfunc     - function pointer to connect Xevents from the widgets
 * @param *widget     - void pointer to the widget
 * @param *event      -  void pointer to the XEvent
 * @param *user_data  -  void pointer to attached user_data
 * @return void
 */

typedef void (*evfunc)(void * widget, void * event, void* user_data);


/**
 * @brief *xevfunc     - function pointer to connect XEvents from the widgets
 * @param *widget      - void pointer to the widget
 * @param *user_data   -  void pointer to attached user_data
 * @return void
 */

typedef void (*xevfunc)(void * widget, void* user_data);

/**
 * 
 * @brief Func_t     - struct to hold all supported event callbacks
 * 
 */

typedef struct {
    xevfunc expose_callback;
    xevfunc configure_callback;
    xevfunc enter_callback;
    xevfunc leave_callback;
    xevfunc user_callback;

    evfunc button_press_callback;
    evfunc button_release_callback;
    evfunc motion_callback;
    evfunc key_press_callback;
    evfunc key_release_callback;
} Func_t;

/**
 * 
 * @brief Widget_t           - struct to hold the basic widget info
 * @param *dpy               - pointer to the Display to use
 * @param widget             - the X11 Window
 * @param *parent            - pointer to the Parent Window or Widget_t
 * @param event_callback     - the main XEvent callback
 * @param func               - struct holding the event callbacks
 * @param *surface           - pointer to the cairo xlib surface
 * @param *cr                - pointer to the cairo xlib surface context
 * @param *buffer            - pointer to the cairo buffer surface
 * @param *crb               - pointer to the cairo buffer surface context
 * @param data               - int to hold user data 
 * @param *label             - pointer to the widget label
 * @param input_label        - char array the widget input label
 * @param state              - int to hold the widget state
 * @param has_pointer        - is mouse pointer in widget
 * @param pos_x              - mouse pointer x position on button press
 * @param pos_y              - mouse pointer y position on button press
 * @param x                  - x position of Window on Parent
 * @param y                  - y position of Window on Parent
 * @param width              - widget width
 * @param height             - widget height
 * @param scale_x            - scaling factor on x axis
 * @param scale_y            - scaling factor on y axis
 * @param transparency       - flag to set/check transparent drawing
 * @param *adj_x             - pointer to the x axis adjustment
 * @param *adj_y             - pointer to the y axis adjustment
 * @param *childlist         - pointer to Widget_t child list
 */

struct Widget_t {
/** pointer to the Display to use */
    Display *dpy;
/** the X11 newly created Window */
    Window widget;
/** pointer to the Parent Window or Widget_t */
    void *parent;
/** the main XEvent callback */
    evfunc event_callback;
/** struct holding the event callbacks */
    Func_t func;
/** pointer to the cairo xlib surface */
    cairo_surface_t *surface;
/** pointer to the cairo xlib surface context */
    cairo_t *cr;
/** pointer to the cairo buffer surface used for transparency */
    cairo_surface_t *buffer;
/** pointer to the cairo buffer surface context */
    cairo_t *crb;
/** int to hold user data */
    int data;
/** pointer to the widget label */
    const char* label;
/** char array to hold user input */
    char input_label[32];
/** int to hold the widget state default = 0 */
    int state;
/** indicate if mouse pointer is in widget */
    bool has_pointer;
/** mouse pointer x position on button press */
    int pos_x;
/** mouse pointer y position on button press */
    int pos_y;
/** x position of Window related to the Parent */
    int x;
/** y position of Window related to the Parent */
    int y;
/** the widget size x-axis */
    int width;
/** the widget size y-axis */
    int height;
/** the x scaling factor used to resize child widgets */
    float scale_x;
/** the y scaling factor used to resize child widgets */
    float scale_y;
/** indicaate if the widget use transparent drawing, default = true */
    bool transparency;
/** pointer to the x axis adjustment */
    Adjustment_t *adj_x;
/** pointer to the y axis adjustment */
    Adjustment_t *adj_y;
/** pointer to Widget_t child list */
    Childlist_t *childlist;
};


/**
 * @brief *create_window     - create a Window 
 * @param *dpy               - pointer to the Display to use
 * @param *win               - pointer to the Parrent Window (may be Root)
 * @param Context            - a XContext to store Window informations
 * @param x,y,width,height   - the position/geometry to create the window
 * @return Widget_t*         - pointer to the Widget_t struct
 */

Widget_t *create_window(Display *dpy, Window win, XContext Context,
                          int x, int y, int width, int height);

/**
 * @brief *create_widget      - create a widget
 * @param *dpy                - pointer to the Display to use
 * @param *parent             - pointer to the Parrent Widget_t
 * @param Context             - a XContext to store Window informations
 * @param x,y,width,height    - the position/geometry to create the widget
 * @return Widget_t*          - pointer to the Widget_t struct
 */

Widget_t *create_widget(Display *dpy, Widget_t *win, XContext Context,
                          int x, int y, int width, int height);

/**
 * @brief quit              - exit the main loop
 * @param *w                - pointer to the Widget_t sending the request
 * @return void 
 */

void quit(Widget_t *w);

/**
 * @brief _transparency     - copy parent surface to child surface
 * @param *wid              - pointer to the Widget_t receiving a event
 * @param *user_data        - void pointer to attached user_data
 * @return void 
 */

void transparent_draw(void * wid, void* user_data);

/**
 * @brief destroy_widget    - destroy a widget and remove it from the Context
 * @param *w                - pointer to the Widget_t sending the request
 * @param Context           - the Context holding the widget info
 * @return void 
 */

void destroy_widget(Widget_t *w, XContext Context);

/**
 * @brief widget_event_loop - the internal widget event loop
 * @param *w                - pointer to the Widget_t receiving a event
 * @param *event            - void pointer to the XEvent
 * @param *user_data        - void pointer to attached user_data
 * @return void 
 */

void widget_event_loop(void *w_, void* event, void* user_data);

/**
 * @brief loop              - the event loop
 * @param *w                - pointer to the main Window
 * @param Context           - the Context holding all the widget infos
 * @param *run              - pointer to bool used to quit the loop
 * @return void 
 */

void loop(Widget_t *w, XContext Context, bool *run);

/**
 * @brief key_mapping       - modifier key's mapped to a integer value
 * @param *dpy              - pointer to the Display in use
 * @param *xkey             - the key to map
 * @return int              - value (1-10) or 0 when not mapped 
 */

int key_mapping(Display *dpy, XKeyEvent *xkey);


#endif //XWIDGET_H

