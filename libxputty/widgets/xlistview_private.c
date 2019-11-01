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


#include "xlistview_private.h"
#include "xtooltip.h"


/**
 * @brief _draw_listview           - draw listview on expose call
 * @param *w_                  - the listview to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_listview(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    set_pattern(w,&w->app->color_scheme->normal,&w->app->color_scheme->active,BACKGROUND_);
    cairo_paint (w->cr);
}

/**
 * @brief _draw_list           - draw list on expose call
 * @param *w_                  - the Widget_t to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_list(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    _draw_listview_viewslider(w, user_data);
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;

    int v = (int)w->adj->max_value;
    int sub = (v<=0) ? 0 : 5;
    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width-sub , height);
    cairo_fill (w->crb);

    int i = (int)max(0,adj_get_value(w->adj));
    int j = filelist->list_size<filelist->show_items+i+1 ? 
      filelist->list_size : filelist->show_items+i+1;
    for(;i<j;i++) {
        if(i == filelist->prelight_item && i == filelist->active_item)
            use_base_color_scheme(w, ACTIVE_);
        else if(i == filelist->prelight_item)
            use_base_color_scheme(w, PRELIGHT_);
        else if (i == filelist->active_item)
            use_base_color_scheme(w, SELECTED_);
        else
            use_base_color_scheme(w,NORMAL_ );
        cairo_rectangle(w->crb, 0, i*25, width-sub , 25);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, PRELIGHT_);
        cairo_stroke(w->crb); 
        cairo_text_extents_t extents;
        /** show label **/
        if(i == filelist->prelight_item && i == filelist->active_item)
            use_text_color_scheme(w, ACTIVE_);
        else if(i == filelist->prelight_item)
            use_text_color_scheme(w, PRELIGHT_);
        else if (i == filelist->active_item)
            use_text_color_scheme(w, SELECTED_);
        else
            use_text_color_scheme(w,NORMAL_ );
        cairo_set_font_size (w->crb, 12);
        cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                                   CAIRO_FONT_WEIGHT_BOLD);
        cairo_text_extents(w->crb,filelist->list_names[i] , &extents);

        cairo_move_to (w->crb, (width-extents.width)/2., (25*(i+1)) - extents.height );
        cairo_show_text(w->crb, filelist->list_names[i]);
        cairo_new_path (w->crb);
        if (i == filelist->prelight_item && extents.width > (float)width-20) {
            tooltip_set_text(w,filelist->list_names[i]);
            w->flags |= HAS_TOOLTIP;
            show_tooltip(w);
        } else {
            w->flags &= ~HAS_TOOLTIP;
            hide_tooltip(w);
        }
    }
}

/**
 * @brief _list_motion             - follow mouse pointer to set prelight item 
 * @param *w_                      - void pointer to the Widget_t listview
 * @param *xmotion_                - void pointer to the XMotionEvent
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void _list_motion(void *w_, void* xmotion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    XMotionEvent *xmotion = (XMotionEvent*)xmotion_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    int _items = height/(height/25);
    int prelight_item = xmotion->y/_items;
    if(prelight_item != filelist->prelight_item) {
        filelist->prelight_item = prelight_item;
        expose_widget(w);
    }
}

/**
 * @brief _list_key_pressed        - move listview or set active entry 
 * @param *w_                      - void pointer to the Widget_t listview
 * @param *button_                 - void pointer to the XButtonEvent
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void _list_key_pressed(void *w_, void* xkey_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XKeyEvent *xkey = (XKeyEvent*)xkey_;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    int _items = height/(height/25);
   // filelist->prelight_item = xkey->y/_items;
    int nk = key_mapping(w->app->dpy, xkey);
    if (nk) {
        switch (nk) {
            case 3: filelist->prelight_item = (xkey->y+24)/_items;
            break;
            case 4: filelist->prelight_item = (xkey->y+24)/_items;
            break;
            case 5: filelist->prelight_item = (xkey->y-24)/_items;
            break;
            case 6: filelist->prelight_item = (xkey->y-24)/_items;
            break;
            default:
            break;
        }
    }
}

/**
 * @brief _list_entry_released     - move listview or set active entry 
 * @param *w_                      - void pointer to the Widget_t listview
 * @param *button_                 - void pointer to the XButtonEvent
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void _list_entry_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER) {
        ViewList_t *filelist = (ViewList_t*)w->parent_struct;
        XButtonEvent *xbutton = (XButtonEvent*)button_;
        XWindowAttributes attrs;
        XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
        int height = attrs.height;
        int _items = height/(height/25);
        int prelight_item = xbutton->y/_items;
        if(xbutton->button == Button4) {
            prelight_item = (xbutton->y-24)/_items;
            if(prelight_item != filelist->prelight_item) {
                filelist->prelight_item = prelight_item;
            }
        } else if (xbutton->button == Button5) {
            prelight_item = (xbutton->y+24)/_items;
            if(prelight_item != filelist->prelight_item) {
                filelist->prelight_item = prelight_item;
            }
        } else if(xbutton->button == Button1) {
            Widget_t* listview = w->parent;
            filelist->active_item = filelist->prelight_item;
            adj_set_value(listview->adj,filelist->active_item);
        }
    }
}

/**
 * @brief _leave_list          - draw list on pointer leave
 * @param *w_                  - the Widget_t to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _leave_list(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    filelist->prelight_item = -1;
    expose_widget(w);
}

/**
 * @brief _reconfigure_listview_viewport - reconfigure the viewport adjustment
 * on size changes
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _reconfigure_listview_viewport(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    float st = adj_get_state(w->adj);
    Widget_t* listview = w->parent;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(listview->app->dpy, (Window)listview->widget, &attrs);
    int height = attrs.height;
    filelist->show_items = height/25;
    w->adj->max_value = filelist->list_size-filelist->show_items;
    adj_set_state(w->adj,st);
}

/**
 * @brief _configure_listview       - configure the viewport on mapping
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _configure_listview(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t* listview = w->parent;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(listview->app->dpy, (Window)listview->widget, &attrs);
    int width = attrs.width;
    XResizeWindow (w->app->dpy, w->widget, width, 25*(max(1,filelist->list_size)));
}

/**
 * @brief _draw_listview_viewslider - draw a slider on the viewport
 * to indicate the view point
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _draw_listview_viewslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)w->adj->max_value;
    if (v<=0) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    float sliderstate = adj_get_state(w->adj);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, width-5,0,5,height);
    cairo_fill_preserve(w->crb);
    use_shadow_color_scheme(w, NORMAL_);
    cairo_fill(w->crb);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, width-5,(height-10)*sliderstate,5,10);
    cairo_fill_preserve(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_line_width(w->crb,1);
    cairo_stroke(w->crb);
}

/**
 * @brief _set_listview_viewpoint  - move the view_port to position
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _set_listview_viewpoint(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)max(0,adj_get_value(w->adj));
    XMoveWindow(w->app->dpy,w->widget,0, -25*v);
}

