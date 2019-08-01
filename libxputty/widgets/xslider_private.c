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


#include "xslider_private.h"


/**
 * @brief _pattern_vslider  - set pattern for the slider base
 * @param *w_               - void pointer to the Widget_t button
 * @param st                - the Widget_t Color_t mode to use
 * @param width             - the width of the base
 * @return void
 */

void _pattern_vslider(Widget_t *w, Color_state st, int width) {
    Colors *c = get_color_scheme(w->app,st);
    if (!c) return;
    cairo_pattern_t *pat = cairo_pattern_create_linear (2, 2, width, 2);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_pattern_add_color_stop_rgba(pat, 0.5, 0.0, 0.0, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_set_source(w->crb, pat);
    cairo_pattern_destroy (pat);
}


/**
 * @brief _pattern_hslider  - set pattern for the slider base
 * @param *w_               - void pointer to the Widget_t button
 * @param st                - the Widget_t Color_t mode to use
 * @param width             - the width of the base
 * @return void
 */

void _pattern_hslider(Widget_t *w, Color_state st, int height) {
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
 * @brief _draw_vslider          - internal draw the slider to the buffer
 * @param *w_                    - void pointer to the Widget_t button
 * @param *user_data             - void pointer to attached user_data
 * @return void
 */

void _draw_vslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-2;
    float center = (float)width/2;
    float upcenter = (float)width;
    
    if (attrs.map_state != IsViewable) return;

    float sliderstate = adj_get_state(w->adj_y);

    _pattern_vslider(w, get_color_state(w), width);
    cairo_move_to (w->crb, center, center);
    cairo_line_to(w->crb,center,height-center-10);
    cairo_set_line_cap (w->crb,CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(w->crb,center);
    cairo_stroke(w->crb);

    use_base_color_scheme(w, get_color_state(w));
    cairo_move_to (w->crb, center, center);
    cairo_line_to(w->crb,center,height-center-10);
    cairo_set_line_width(w->crb,center/10);
    cairo_stroke(w->crb);

    use_base_color_scheme(w, get_color_state(w));
    cairo_arc(w->crb,center, (height-center-10) -
        ((height-10-upcenter) * sliderstate), center/2, 0, 2 * M_PI );
    cairo_fill_preserve(w->crb);
    cairo_set_line_width(w->crb,1);
    cairo_stroke(w->crb);


    use_bg_color_scheme(w, get_color_state(w));
    cairo_arc(w->crb,center, (height-center-10) -
        ((height-10-upcenter) * sliderstate), center/3, 0, 2 * M_PI );
    cairo_fill_preserve(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_line_width(w->crb,center/15);
    cairo_stroke(w->crb);
    cairo_new_path (w->crb);

    cairo_text_extents_t extents;


    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, center/2);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, center-extents.width/2, height );
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);

}

/**
 * @brief _draw_hslider          - internal draw the slider to the buffer
 * @param *w_                    - void pointer to the Widget_t button
 * @param *user_data             - void pointer to attached user_data
 * @return void
 */

void _draw_hslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-2;
    float center = (float)height/2;
    float upcenter = (float)height;
    
    if (attrs.map_state != IsViewable) return;

    float sliderstate = adj_get_state(w->adj_x);

    _pattern_hslider(w, get_color_state(w), height);
    cairo_move_to (w->crb, center, center);
    cairo_line_to(w->crb,width-center-10,center);
    cairo_set_line_cap (w->crb,CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(w->crb,center);
    cairo_stroke(w->crb);

    use_base_color_scheme(w, get_color_state(w));
    cairo_move_to (w->crb, center, center);
    cairo_line_to(w->crb,width-center-10,center);
    cairo_set_line_width(w->crb,center/10);
    cairo_stroke(w->crb);

    use_base_color_scheme(w, get_color_state(w));
    cairo_arc(w->crb, (center) +
        ((width-10-upcenter) * sliderstate),center, center/2, 0, 2 * M_PI );
    cairo_fill_preserve(w->crb);
    cairo_set_line_width(w->crb,1);
    cairo_stroke(w->crb);

    use_bg_color_scheme(w, get_color_state(w));
    cairo_arc(w->crb, (center) +
        ((width-10-upcenter) * sliderstate),center, center/3, 0, 2 * M_PI );
    cairo_fill_preserve(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_line_width(w->crb,center/15);
    cairo_stroke(w->crb);
    cairo_new_path (w->crb);

    cairo_text_extents_t extents;


    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, center/2);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, width/2-extents.width/2, height );
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);

}

/**
 * @brief _slider_released  - redraw the slider when buttob released 
 * @param *w_               - void pointer to the Widget_t button
 * @param *user_data        - void pointer to attached user_data
 * @return void
 */

void _slider_released(void *w_, void* button_, void* user_data) {
    expose_widget(w_);
}
