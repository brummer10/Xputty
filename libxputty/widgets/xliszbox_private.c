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


#include "xlistbox_private.h"


/**
 * @brief _draw_listbox           - draw listbox on expose call
 * @param *w_                  - the listbox to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_listbox(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    use_bg_color_scheme(w, NORMAL_);
    cairo_paint (w->crb);
}

/**
 * @brief _draw_listbox_item   - draw item on expose call
 * @param *w_                  - the item to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_listbox_item(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;
    Widget_t* view_port = w->parent;
    Widget_t* listbox =  view_port->parent;
    int j = (int)listbox->adj->value;
    if(w == view_port->childlist->childs[j]) w->state=3;
    
    use_bg_color_scheme(w, get_color_state(w));
    cairo_rectangle(w->crb, 0, 0, width , height);
    if(w->state==0) {
        cairo_set_line_width(w->crb, 1.0);
        cairo_fill_preserve(w->crb);
        use_bg_color_scheme(w, PRELIGHT_);
    } else if(w->state==1) {
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.5);
        use_bg_color_scheme(w, PRELIGHT_);
    } else if(w->state==2) {
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_bg_color_scheme(w, PRELIGHT_);
    } else if(w->state==3) {
        use_bg_color_scheme(w, SELECTED_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_bg_color_scheme(w, PRELIGHT_);
    }
    cairo_stroke(w->crb); 
    cairo_text_extents_t extents;
    /** show label **/
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, height/2);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, (width-extents.width)/2., height - extents.height );
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);
    if (extents.width > (float)width)  w->flags |= HAS_TOOLTIP;
    else w->flags &= ~HAS_TOOLTIP;
}

/**
 * @brief _draw_listbox_viewslider - draw a slider on the viewport
 * to indicate the view point
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _draw_listbox_viewslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)w->adj->max_value;
    if (!v) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    float sliderstate = adj_get_state(w->adj);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, width-5,0,5,height);
    cairo_fill_preserve(w->crb);
    use_base_color_scheme(w, NORMAL_);
    cairo_fill(w->crb);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, width-5,(height-10)*sliderstate,5,10);
    cairo_fill_preserve(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_line_width(w->crb,1);
    cairo_stroke(w->crb);
}

/**
 * @brief _set_listbox_viewpoint  - move the view_port to position
 * @param *w_                     - void pointer to view_port
 * @param *user_data              - attached user_data
 * @return void
 */

void _set_listbox_viewpoint(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)adj_get_value(w->adj);
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->childlist->childs[0]->widget, &attrs);
    int height = attrs.height;
    XMoveWindow(w->app->dpy,w->widget,0, -height*v);
}

/**
 * @brief _listbox_entry_released  - redraw the slider when buttob released 
 * @param *w_                      - void pointer to the Widget_t entry
 * @param *button_                 - void pointer to the XButtonEvent
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void _listbox_entry_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t* view_port = w->parent;
    int direction = 0 ;
    if (w->flags & HAS_POINTER) {
        XButtonEvent *xbutton = (XButtonEvent*)button_;
        if(xbutton->button == Button1) {
            Widget_t* listbox =  view_port->parent;
            int i = view_port->childlist->elem-1;
            int old_value = (int) listbox->adj->value;
            for(;i>-1;i--) {
                Widget_t *wid = view_port->childlist->childs[i];
                if (xbutton->window == wid->widget) {
                    const char *l = view_port->childlist->childs[i]->label;
                    listbox->func.button_release_callback(listbox, &i, &l);
                    float value = (float)i;
                    check_value_changed(listbox->adj, &value);
                    wid->state= 3;
                }
                wid->state= 0;
            }
            expose_widget(view_port->childlist->childs[old_value]);
            expose_widget(w);
        } else if(xbutton->button == Button4) {
            direction = 1;
        } else if(xbutton->button == Button5) {
            direction = -1;
        }
        if (direction !=0) {
            float value = view_port->adj->value + (view_port->adj->step * -direction);
            if (value>view_port->adj->max_value) value = view_port->adj->max_value;
            if (value<view_port->adj->min_value) value = view_port->adj->min_value;
            check_value_changed(view_port->adj, &value);
        }
    }
}

