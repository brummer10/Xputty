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

#include "xwidget_private.h"

/**
 * @brief _scroll_event     - internal check which adjustment needs update
 * @param *wid              - pointer to the Widget_t receiving a event
 * @param direction         - up/down scroll diretion
 * @return void 
 */

void _scroll_event(Widget_t * wid, int direction) {
    Adjustment_t *adj = NULL;
    if (wid->adj_y) {
        adj = wid->adj_y;
    } else if(wid->adj_x) {
        adj = wid->adj_x;
    }
    if (adj) {
        float value = adj->value;
        switch(adj->type) {
            case (CL_CONTINUOS):
                value = min(adj->max_value,max(adj->min_value, 
                                adj->value + (adj->step * direction)));
            break;
            case (CL_TOGGLE):
                value = adj->value ? 1.0 : 0.0;
            break;
            default:
            break;
        }
        check_value_changed(adj, &value);
    } 
}

/**
 * @brief _toggle_event     - internal check which adjustment needs update
 * @param *wid              - pointer to the Widget_t receiving a event
 * @param direction         - up/down scroll diretion
 * @return void 
 */

void _toggle_event(Widget_t * wid) {
    Adjustment_t *adj = NULL;
    if (wid->adj_y) {
        adj = wid->adj_y;
    } else if(wid->adj_x) {
        adj = wid->adj_x;
    }
    if (adj && adj->type != CL_TOGGLE) {
        adj_set_start_value(wid);
    }
}

/**
 * @brief _button_press     - internal check which button is pressed
 * @param *wid              - pointer to the Widget_t receiving a event
 * @param *xbutton          - pointer to the XButtonEvent
 * @param *user_data        - void pointer to attached user_data
 * @return void 
 */

void _button_press(Widget_t * wid, XButtonEvent *xbutton, void* user_data) {
    switch(xbutton->button) {
        case Button1:
            wid->state = 2;
            wid->has_pointer = _has_pointer(wid, xbutton);
            wid->pos_x = xbutton->x;
            wid->pos_y = xbutton->y;
            _toggle_event(wid);
            wid->func.button_press_callback(wid, xbutton, user_data);
        break;
        case Button2:
            debug_print("Button2 \n");
        break;
        case Button3:
            debug_print("Button3 \n");
        break;
        case  Button4:
            _scroll_event(wid, 1);
        break;
        case Button5:
            _scroll_event(wid, -1);
        break;
        default:
        break;
    }
}

/**
 * @brief _propagate_childs - send expose to child window
 * @param *wid              - pointer to the Widget_t send the event
 * @return void 
 */

void _propagate_child_expose(Widget_t *wid) {

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
 * @brief _check_keymap     - check if key is in map, send requests if so 
 * @param *w                - pointer to the Widget_t receiving the event
 * @param xkey              - the XKeyEvent to check
 * @return void
 */

void _check_keymap (void *w_ ,XKeyEvent xkey) {
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
 * @brief _has_pointer      - check if the widget has the pointer on Button release 
 * @param *w                - pointer to the Widget_t sending the request
 * @param *button           - pointer to the XButtonEvent sending the notify
 * @return bool             - true if pointer is in widget 
 */

bool _has_pointer(Widget_t *w, XButtonEvent *button) {
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

void _set_adj_value(void *w_, bool x, int direction) {
    Widget_t *wid = (Widget_t*)w_;
    Adjustment_t *adj = NULL;
    if (x && wid->adj_x) {
        adj = wid->adj_x;
    } else if (!x && wid->adj_y) {
        adj = wid->adj_y;
    }
    if (adj) {
        float value = min(adj->max_value,max(adj->min_value, 
        adj->value + (adj->step * direction)));
        check_value_changed(adj, &value);
    }
}

/**
 * @brief _dummy1_callback    - default debuging callback for evfunc's
 * @param *w                 - pointer to the Widget_t receive the event
 * @param user_data          - void pointer to attached user_data
 * @return void 
 */

void _dummy1_callback(void *w_, void* _data, void* user_data) {
    debug_print("Widget_t _dummy callback\n");
}

/**
 * @brief _dummy1_callback    - default debuging callback for xevfunc's
 * @param *w                 - pointer to the Widget_t receive the event
 * @param user_data          - void pointer to attached user_data
 * @return void 
 */

void _dummy_callback(void *w_, void* user_data) {
    debug_print("Widget_t _dummy callback\n");
}

/**
 * @brief _resize_surface    - intern check if surfaces needs resizing
 * @param *wid               - pointer to the Widget_t receive the event
 * @param width              - the new width
 * @param height             - the new height
 * @return void 
 */

void _resize_surface(Widget_t *wid, int width, int height) {
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

void _resize_childs(Widget_t *wid) {
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
                XMoveWindow(wid->dpy,w->widget,w->scale.init_x-wid->scale.scale_x,
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
              //  XMoveWindow(wid->dpy,w->widget,w->scale.init_x-wid->scale.scale_x,
              //                          w->scale.init_y-wid->scale.scale_y);
            break;
            default:
            break;
        }
        
    }
}
