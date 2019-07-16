/*
 * This is free and unencumbered software released into the public domain.

 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.

 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


#pragma once

#ifndef XPUTTY_H_
#define XPUTTY_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#include <math.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
					define debug print
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#ifndef DEBUG
#define DEBUG 0
#endif
#define debug_print(...) \
            ((void)((DEBUG) ? fprintf(stderr, __VA_ARGS__) : 0))

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				define min/max if not defined already
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x, y) ((x) < (y) ? (y) : (x))
#endif


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
 * @brief Widget_t     - struct to hold the basic widget info
 * 
 */

typedef struct {
    Display *dpy;               /** pointer to the Display in use     */
    Window widget;              /** pointer to the X11 Window         */
    evfunc event_callback;      /** the event main callback           */
    Func_t func;                /** struct holding the event callbacks*/
    cairo_surface_t *surface;   /** pointer to the cairo surface      */
    cairo_t *cr;                /** pointer to the cairo context      */
    int data;                   /** int to hold user data             */
    const char* label;          /** pointer to the widget label       */
    char input_label[32];       /** char array the widget input label */
    int state;                  /** int to hold the widget state      */
    bool has_pointer;           /** is mouse pointer in widget        */
    int pos_x, pos_y;           /** mouse pointer position            */
    int width, height;          /** widget size                       */
    float scale_x, scale_y;     /** scaling factor                    */
} Widget_t;



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
 * @param *win                - pointer to the Parrent Window 
 * @param Context             - a XContext to store Window informations
 * @param x,y,width,height    - the position/geometry to create the widget
 * @return Widget_t*          - pointer to the Widget_t struct
 */

Widget_t *create_widget(Display *dpy, Window win, XContext Context,
                          int x, int y, int width, int height);

/**
 * @brief quit              - exit the main loop
 * @param *w                - pointer to the Widget_t sending the request
 * @return void 
 */

void quit(Widget_t *w);

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


#endif //XPUTTY_H_

