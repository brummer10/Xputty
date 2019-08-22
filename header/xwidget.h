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
 * @param *main       - pointer to Xputty main struct
 * @param *event      - void pointer to the XEvent
 * @param *user_data  - void pointer to attached user_data
 * @return void
 */

typedef void (*vfunc)(void * widget, void * event, Xputty *main, void* user_data);

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
    xevfunc adj_callback;
    xevfunc value_changed_callback;
    xevfunc user_callback;

    evfunc button_press_callback;
    evfunc button_release_callback;
    evfunc motion_callback;
    evfunc key_press_callback;
    evfunc key_release_callback;
} Func_t;

/**
 * 
 * @brief Gravity              - enum to indicate how to resize a widget
 * @param NORTHWEST            - Widget_t adjust nord/west
 * @param NORTHEAST            - Widget_t adjust nord/east
 * @param SOUTHWEST            - Widget_t adjust south/west
 * @param SOUTHEAST            - Widget_t adjust south/east
 * @param CENTER               - Widget_t adjust centered
 * @param ASPECT               - Widget_t adjust in a aspect frame
 * @param NONE                 - Widget_t request no adjustment in frame
 */

typedef enum {
/** Widget_t adjust nord/west */
    NORTHWEST    = 0x0001,
/** Widget_t adjust nord/east */
    NORTHEAST    = 0x0002,
/** Widget_t adjust south/west */
    SOUTHWEST     = 0x0004,
/** Widget_t adjust south/east */
    SOUTHEAST     = 0x0008,
/** Widget_t adjust centered */
    CENTER        = 0x0016,
/** Widget_t adjust in a aspect frame */
    ASPECT        = 0x0032,
/** Widget_t request no adjustment in frame */
    NONE          = 0x10000   
}Gravity;

/**
 * 
 * @brief Resize_t             - struct used to resize child widgets
 * @param init_x               - initial x position on Parent
 * @param init_y               - initial y position on Parent
 * @param init_width           - initial width
 * @param init_height          - initial height
 * @param scale_x              - scalling size of the x axsis
 * @param scale_y              - scalling size of the y axsis
 * @param cscale_x             - scalling factor of the x axsis
 * @param cscale_y             - scalling factor of the y axsis
 * @param ascale               - scalling factor for aspect scalling
 */

typedef struct {
/** indicate how the widget wish to be resized */
    Gravity gravity;
/** initial x position on Parent */
    int init_x;
/** initial y position on Parent */
    int init_y;
/** initial width */
    int init_width;
/** initial height */
    int init_height;
/** scalling size of the x axsis */
    float scale_x;
/** scalling size of the y axsis */
    float scale_y;
/** scalling factor of the x axsis */
    float cscale_x;
/** scalling factor of the y axsis */
    float cscale_y;
/** rescalling factor of the x axsis */
    float rcscale_x;
/** rescalling factor of the y axsis */
    float rcscale_y;
/** scalling factor for aspect scalling */
    float ascale;
} Resize_t;

/**
 * 
 * @brief Widget_t           - struct to hold the basic widget info
 * @param *dpy               - pointer to the Display to use
 * @param *app               - pointer to the main struct
 * @param widget             - the X11 Window
 * @param *parent            - pointer to the Parent Window or Widget_t
 * @param event_callback     - the main XEvent callback
 * @param func               - struct holding the event callbacks
 * @param *surface           - pointer to the cairo xlib surface
 * @param *cr                - pointer to the cairo xlib surface context
 * @param *buffer            - pointer to the cairo buffer surface
 * @param *crb               - pointer to the cairo buffer surface context
 * @param *image             - pointer to the cairo image surface
 * @param *normal_color      - struct to set Widget_t normal colors
 * @param *active_color      - struct to set Widget_t active colors
 * @param *prelight_color    - struct to set Widget_t prelight colors
 * @param *selected_color    - struct to set Widget_t selected colors
 * @param data               - int to hold user data 
 * @param *label             - pointer to the widget label
 * @param input_label        - char array the widget input label
 * @param state              - int to hold the widget state
 * @param has_focus          - indicate the mouse pointer state
 * @param has_pointer        - is mouse pointer in widget
 * @param pos_x              - mouse pointer x position on button press
 * @param pos_y              - mouse pointer y position on button press
 * @param x                  - x position of Window on Parent
 * @param y                  - y position of Window on Parent
 * @param width              - widget width
 * @param height             - widget height
 * @param is_widget          - set/check if the parent is Widget_t
 * @param is_radio           - set/check if the Widget_t is part of a radio group
 * @param transparency       - flag to set/check transparent drawing
 * @param scale              - struct used to resize child widgets
 * @param *adj_x             - pointer to the x axis adjustment
 * @param *adj_y             - pointer to the y axis adjustment
 * @param *adj               - pointer to the adjustment in use
 * @param *childlist         - pointer to Widget_t child list
 * @param xic                - Locale and UTF 8 support interface
 * @param xim                - Context to Locale and UTF 8 support
 */

struct Widget_t {
/** pointer to the Display to use */
    Display *dpy;
/** pointer to the main struct */
    Xputty *app;
/** the X11 newly created Window */
    Window widget;
/** pointer to the Parent Window or Widget_t */
    void *parent;
/** pointer to the Parent struct */
    void *parent_struct;
/** the main XEvent callback */
    vfunc event_callback;
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
/** pointer to the cairo image surface used to load a png */
    cairo_surface_t *image;
/** int to hold user data */
    int data;
/** pointer to the widget label */
    const char* label;
/** char array to hold user input */
    char input_label[32];
/** int to hold the widget state default = 0 */
    int state;
/** indicate the mouse pointer state */
    bool has_focus;
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
/** indicaate if the widget has parent Widget_t or XWindow */
    bool is_widget;
/** indicaate if the widget is part of a radio group */
    bool is_radio;
/** indicaate if the widget is a popup Widget_t */
    bool is_pop_widget;
/** indicaate if the widget use transparent drawing, default = true */
    bool transparency;
/** struct used to resize child widgets */
    Resize_t scale;
/** pointer to the x axis adjustment */
    Adjustment_t *adj_x;
/** pointer to the y axis adjustment */
    Adjustment_t *adj_y;
/** pointer to the adjustment in use*/
    Adjustment_t *adj;
/** pointer to Widget_t child list */
    Childlist_t *childlist;
/** Locale and UTF 8 support */
    XIC xic;
/** Context to Locale and UTF 8 support */
    XIM xim;
};


/**
 * @brief *create_window     - create a Window 
 * @param *dpy               - pointer to the Display to use
 * @param win                - pointer to the Parrent Window (may be Root)
 * @param x,y,width,height   - the position/geometry to create the window
 * @return Widget_t *        - pointer to the Widget_t struct
 */

Widget_t *create_window(Xputty *app, Window win,
                          int x, int y, int width, int height);

/**
 * @brief *create_widget      - create a widget
 * @param *dpy                - pointer to the Display to use
 * @param *parent             - pointer to the Parrent Widget_t
 * @param x,y,width,height    - the position/geometry to create the widget
 * @return Widget_t*          - pointer to the Widget_t struct
 */

Widget_t *create_widget(Xputty *app, Widget_t *win,
                          int x, int y, int width, int height);

/**
 * @brief widget_show       - map/show widget
 * @param *w                - pointer to the Widget_t to map
 * @return void 
 */

void widget_show(Widget_t *w);

/**
 * @brief pop_widget_show_all   - map/show popup widget with all childs
 * @param *w                    - pointer to the Widget_t to map
 * @return void 
 */

void pop_widget_show_all(Widget_t *w);

/**
 * @brief widget_hide       - unmap/hide widget
 * @param *w                - pointer to the Widget_t to unmap
 * @return void 
 */

void widget_hide(Widget_t *w);

/**
 * @brief widget_show_all   - map/show widget with all childs
 * @param *w                - pointer to the Widget_t to map
 * @return void 
 */

void widget_show_all(Widget_t *w);

/**
 * @brief *get_toplevel_widget - get pointer to the top level Widget_t
 * @param *main                - pointer to the main Xputty struct
 * @return void 
 */

Widget_t *get_toplevel_widget(Xputty *main);

/**
 * @brief quit              - exit the main loop
 * @param *w                - pointer to the Widget_t sending the request
 * @return void 
 */

void quit(Widget_t *w);

/**
 * @brief quit_widget       - remove a widget from the processing loop
 * @param *w                - pointer to the Widget_t sending the request
 * @return void 
 */

void quit_widget(Widget_t *w);

/**
 * @brief transparent_draw  - copy parent surface to child surface
 * @param *wid              - pointer to the Widget_t receiving a event
 * @param *user_data        - void pointer to attached user_data
 * @return void 
 */

void transparent_draw(void * wid, void* user_data);

/**
 * @brief widget_reset_scale - reset scaling mode after image surface
 * @param *w                 - pointer to the Widget_t sending the request
 * @return void 
 */

void widget_reset_scale(Widget_t *w);

/**
 * @brief widget_set_scale   - set scaling mode for image surface
 * @param *w                 - pointer to the Widget_t sending the request
 * @return void 
 */

void widget_set_scale(Widget_t *w);

/**
 * @brief destroy_widget    - destroy a widget
 * @param *w                - pointer to the Widget_t sending the request
 * @param *main             - pointer to main struct
 * @return void 
 */

void destroy_widget(Widget_t *w, Xputty *main);

/**
 * @brief widget_event_loop - the internal widget event loop
 * @param *w                - pointer to the Widget_t receiving a event
 * @param *event            - void pointer to the XEvent
 * @param *user_data        - void pointer to attached user_data
 * @return void 
 */

void widget_event_loop(void *w_, void* event, Xputty *main, void* user_data);

/**
 * @brief send_configure_event - send ConfigureNotify to Widget_t
 * @param *w                   - pointer to the Widget_t to send the notify
 * @param x,y                  - the new Widget_t position
 * @param width,height         - the new Widget_t size
 * @return void 
 */

void send_configure_event(Widget_t *w,int x, int y, int width, int height);

/**
 * @brief expose_widgets    - send expose expose event to window
 * @param w                 - the Widget_t to send the event to
 * @return void 
 */

void expose_widget(Widget_t *w);

/**
 * @brief _key_mapping       - modifier key's mapped to a integer value
 * @param *dpy              - pointer to the Display in use
 * @param *xkey             - the key to map
 * @return int              - value (1-10) or 0 when not mapped 
 */

int key_mapping(Display *dpy, XKeyEvent *xkey);


#endif //XWIDGET_H

