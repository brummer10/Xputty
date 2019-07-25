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
 * @brief _draw_button        - internal draw the button to the buffer
 * @param *w_                 - void pointer to the Widget_t button
 * @param *user_data          - void pointer to attached user_data
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

    cairo_new_path (w->crb);
    cairo_move_to  (w->crb, 2, (2 + (height-4))/2);
    cairo_curve_to (w->crb, 2 ,2, 2, 2, (2 + width)/2, 2);
    cairo_curve_to (w->crb, width, 2, width, 2, width, (2 + height)/2);
    cairo_curve_to (w->crb, width, height, width, height, (width + 2)/2, height);
    cairo_curve_to (w->crb, 2, height, 2, height, 2, (2 + height)/2);
    cairo_close_path (w->crb);

    cairo_set_line_width(w->crb, 1.0);
    cairo_set_source_rgb (w->crb, 0., 0.1, 0.1);
    if(w->state==1) {
        cairo_set_source_rgba (w->crb, 0.2, 0.2, 0.2, 0.1);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.5);
        cairo_set_source_rgb (w->crb, 0.2, 0.2, 0.2);
    }
    if(w->state==2) {
        cairo_set_source_rgba (w->crb, 0.1, 0.1, 0.1, 0.1);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 2.0);
        cairo_set_source_rgb (w->crb, 0., 0.1, 0.1);
     }
    cairo_stroke(w->crb); 

    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->crb, 0.1, 0.1, 0.1);
    if(w->state==1)
        cairo_set_source_rgb (w->crb, 0.8, 0.8, 0.8);
    if(w->state==2)
        cairo_set_source_rgb (w->crb, 0.2, 0.2, 0.2);
    cairo_set_font_size (w->crb, 12.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, (width-extents.width)*0.5, (height+extents.height)*0.5);
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);
}

/**
 * @brief _button_pressed   - redraw the button and send state via user_callback
 * @param *w_               - void pointer to the Widget_t button
 * @param *button           - void pointer to XEvent.xbutton struct
 * @param *user_data        - void pointer to attached user_data
 * @return void
 */

void _button_pressed(void *w_, void* button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    expose_widget(w_);
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
    expose_widget(w_);
    w->data = 0;
    w->func.user_callback(w_, &w->data);
}
