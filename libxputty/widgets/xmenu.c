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


#include "xmenu.h"
#include "xmenu_private.h"

/**
 * @brief pop_menu_show       - pop up a menu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param *menu               - the menu to show
 * @return void
 */

void pop_menu_show(Widget_t *parent, Widget_t *menu) {
    _configure_menu(parent, menu);
    widget_show_all(menu);
    int err = XGrabPointer(menu->dpy, DefaultRootWindow(parent->dpy), True,
                 ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    menu->app->hold_grab = menu;

    if (err) debug_print("Error grap pointer\n");
}

/**
 * @brief create_menu         - create a menu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param height              - define the height of a single menu item
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* create_menu(Widget_t *parent, int height) {

    int x1, y1;
    Window child;
    XTranslateCoordinates( parent->dpy, parent->widget, DefaultRootWindow(parent->dpy), 0, 0, &x1, &y1, &child );
    Widget_t *wid = create_window(parent->app, DefaultRootWindow(parent->dpy), x1, y1, 10, height);
    Atom window_type = XInternAtom(wid->dpy, "_NET_WM_WINDOW_TYPE", False);
    long vale = XInternAtom(wid->dpy, "_NET_WM_WINDOW_TYPE_POPUP_MENU", False);
    XChangeProperty(wid->dpy, wid->widget, window_type,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &vale,1 );
    XSetTransientForHint(parent->dpy,wid->widget,parent->widget);
    wid->func.expose_callback = _draw_menu;
    childlist_add_child(parent->childlist, wid);
    return wid;

}

/**
 * @brief menu_add_item       - add a item to menu
 * @param *menu               - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t* menu_add_item(Widget_t *menu,const char * label) {
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->dpy, (Window)menu->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    int si = childlist_has_child(menu->childlist);
    Widget_t *wid = create_widget(menu->app, menu, 0, height*si, width, height);
    wid->scale.gravity = NORTHEAST;
    wid->label = label;
    wid->func.expose_callback = _draw_item;
    wid->func.button_press_callback = _item_button_pressed;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    return wid;
}

/**
 * @brief menu_add_check_item - add a item to menu
 * @param *menu               - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t* menu_add_check_item(Widget_t *menu,const char * label) {
    Widget_t *wid = menu_add_item(menu, label);
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,1.0, CL_TOGGLE);
    wid->adj = wid->adj_y;
    wid->func.expose_callback = _draw_check_item;
    wid->func.button_press_callback = _check_item_button_pressed;
    
    return wid;
}

