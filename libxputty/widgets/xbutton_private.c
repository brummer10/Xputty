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


#include "xbutton_private.h"

/**
 * @brief _rounded_rectangle  - internal draw a rounded button
 * @param x                   - point on x axis
 * @param y                   - point on y axis
 * @param width               - the button width
 * @param height              - the button height
 * @return void
 */

void _rounded_rectangle(cairo_t *cr,float x, float y, float width, float height) {
	cairo_new_path (cr);
	cairo_move_to  (cr, x, (y + height)/2);
	cairo_curve_to (cr, x ,y, x, y, (x + width)/2, y);
	cairo_curve_to (cr, width, y, width, y, width, (y + height)/2);
	cairo_curve_to (cr, width, height, width, height, (width + x)/2, height);
	cairo_curve_to (cr, x, height, x, height, x, (y + height)/2);
	cairo_close_path (cr);
}

/**
 * @brief _pattern_in         - a little pattern to make press state more visible
 * @param *w_                 - void pointer to the Widget_t button
 * @param st                  - the color state to use
 * @param height              - the button height
 * @return void
 */

void _pattern_out(Widget_t *w, Color_state st, int height) {
    Colors *c = get_color_scheme(w->app,st);
    if (!c) return;
    cairo_pattern_t *pat = cairo_pattern_create_linear (2, 2, 2, height);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_pattern_add_color_stop_rgba(pat, 0.5, 0.0, 0.0, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_set_source(w->crb, pat);
    cairo_pattern_destroy (pat);
}

/**
 * @brief _pattern_in         - a little pattern to make press state more visible
 * @param *w_                 - void pointer to the Widget_t button
 * @param st                  - the color state to use
 * @param height              - the button height
 * @return void
 */

void _pattern_in(Widget_t *w, Color_state st, int height) {
    Colors *c = get_color_scheme(w->app,st);
    if (!c) return;
    cairo_pattern_t *pat = cairo_pattern_create_linear (2, 2, 2, height);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.0, 0.0, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(pat, 0.5, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.0, 0.0, 0.0, 0.0);
    cairo_set_source(w->crb, pat);
    cairo_pattern_destroy (pat);
}

/**
 * @brief _draw_button           - internal draw the button to the buffer
 * @param *w_                    - void pointer to the Widget_t button
 * @param *user_data             - void pointer to attached user_data
 * @return void
 */

void _draw_button(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-2;
    if (attrs.map_state != IsViewable) return;
    if (!w->state && (int)w->adj_y->value)
        w->state = 3;

    _rounded_rectangle(w->crb,2.0, 2.0, width, height);

    if(w->state==0) {
        cairo_set_line_width(w->crb, 1.0);
        _pattern_out(w, NORMAL_, height);
        cairo_fill_preserve(w->crb);
        use_bg_color_scheme(w, PRELIGHT_);
    } else if(w->state==1) {
        _pattern_out(w, PRELIGHT_, height);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.5);
        use_bg_color_scheme(w, PRELIGHT_);
    } else if(w->state==2) {
        _pattern_in(w, SELECTED_, height);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_bg_color_scheme(w, PRELIGHT_);
    } else if(w->state==3) {
        _pattern_in(w, ACTIVE_, height);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_bg_color_scheme(w, PRELIGHT_);
    }
    cairo_stroke(w->crb); 

    if(w->state==2) {
        _rounded_rectangle(w->crb,4.0, 4.0, width, height);
        cairo_stroke(w->crb);
        _rounded_rectangle(w->crb,3.0, 3.0, width, height);
        cairo_stroke(w->crb);
    } else if (w->state==3) {
        _rounded_rectangle(w->crb,3.0, 3.0, width, height);
        cairo_stroke(w->crb);
    }

    float offset = 0.0;
    cairo_text_extents_t extents;
    if(w->state==0) {
        use_fg_color_scheme(w, NORMAL_);
    } else if(w->state==1 && ! (int)w->adj_y->value) {
        use_fg_color_scheme(w, PRELIGHT_);
        offset = 1.0;
    } else if(w->state==1) {
        use_fg_color_scheme(w, ACTIVE_);
        offset = 0.5;
    } else if(w->state==2) {
        use_fg_color_scheme(w, SELECTED_);
        offset = 2.0;
    } else if(w->state==3) {
        use_fg_color_scheme(w, ACTIVE_);
        offset = 1.0;
    }
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, 12.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, (width-extents.width)*0.5 +offset, (height+extents.height)*0.5 +offset);
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
                            button
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief _button_pressed   - redraw the button and send state via user_callback
 * @param *w_               - void pointer to the Widget_t button
 * @param *button           - void pointer to XEvent.xbutton struct
 * @param *user_data        - void pointer to attached user_data
 * @return void
 */

void _button_pressed(void *w_, void* button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    adj_set_value(w->adj_y, 1.0);
   // expose_widget(w_);
    w->data = 1;
    w->func.user_callback(w_, &w->data);
}

/**
 * @brief _button_released  - redraw the button and send state via user_callback
 * @param *w_               - void pointer to the Widget_t button
 * @param *button           - void pointer to XEvent.xbutton struct
 * @param *user_data        - void pointer to attached user_data
 * @return void
 */

void _button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->has_pointer) w->state=1;
    adj_set_value(w->adj_y, 0.0);
   // expose_widget(w_);
    w->data = 0;
    w->func.user_callback(w_, &w->data);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
                        toggle button
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief _toggle_button_pressed  - redraw the button and send state via user_callback
 * @param *w_                     - void pointer to the Widget_t button
 * @param *button                 - void pointer to XEvent.xbutton struct
 * @param *user_data              - void pointer to attached user_data
 * @return void
 */

void _toggle_button_pressed(void *w_, void* button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    expose_widget(w_);
}

/**
 * @brief _toggle_button_released  - redraw the button and send state via user_callback
 * @param *w_                      - void pointer to the Widget_t button
 * @param *button                  - void pointer to XEvent.xbutton struct
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void _toggle_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->has_pointer) {
        float value = w->adj_y->value ? 0.0 : 1.0;
        adj_set_value(w->adj_y, value);
        w->state = (int) w->adj_y->value ? 3 : 1;
    } else {
        w->state = (int) w->adj_y->value ? 3 : 0;
        w->data = (int) w->adj_y->value;
    }
    expose_widget(w_);
    w->data = (int) w->adj_y->value;
    w->func.user_callback(w_, &w->data);
}
