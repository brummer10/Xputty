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


#include "xmeter_private.h"

/**
 * @brief _create_vertical_meter_image      - internal draw the meter image
 * to the cairo image surface
 * @param *w                                - pointer to the Widget_t meter
 * @param width                             - widget width
 * @param height                            - widget height
 * @return void
 */

void _create_vertical_meter_image(Widget_t *w, int width, int height) {
    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width*2, height);
    cairo_t *cri = cairo_create (w->image);

    cairo_rectangle(cri,0.0, 0.0, width, height);
    use_base_color_scheme(w, NORMAL_);
    cairo_fill(cri);

    cairo_rectangle(cri,width, 0.0, width, height);
    use_base_color_scheme(w, NORMAL_);
    cairo_fill(cri);

    cairo_pattern_t *pat = cairo_pattern_create_linear (0, 0, 0.0, height);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.1, 0.5, 0.1, 0.4);
    cairo_pattern_add_color_stop_rgba(pat, 0.2, 0.4, 0.4, 0.1, 0.4);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.5, 0.0, 0.0, 0.4);
    cairo_set_source(cri, pat);

    int c = (width)/2 ;
    int ci = c-2;

    int i = 2;
    int j = 1;
    for(;i<height;) {
        for(;j<width;) {
            cairo_rectangle(cri,j,i,ci,4);
            cairo_fill(cri);
            j +=c;
        }
        i +=6;
        j = 1;
    }

    cairo_pattern_destroy (pat);
    pat = cairo_pattern_create_linear (0, 0, 0.0, height);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.1, 0.5, 0.1, 1);
    cairo_pattern_add_color_stop_rgba(pat, 0.2, 0.4, 0.4, 0.1, 1);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.5, 0.0, 0.0, 1);
    cairo_set_source(cri, pat);
    i = 2;
    j = 1;
    for(;i<height;) {
        for(;j<width;) {
            cairo_rectangle(cri,width+j,i,ci,4);
            cairo_fill(cri);
            j +=c;
        }
        i +=6;
        j = 1;
    }

    cairo_pattern_destroy (pat);
    cairo_destroy(cri);
}

/**
 * @brief _create_horizontal_meter_image    - internal draw the meter image
 * to the cairo image surface
 * @param *w                                - pointer to the Widget_t meter
 * @param width                             - widget width
 * @param height                            - widget height
 * @return void
 */

void _create_horizontal_meter_image(Widget_t *w, int width, int height) {
    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height*2);
    cairo_t *cri = cairo_create (w->image);

    cairo_rectangle(cri,0.0, 0.0, width, height);
    use_base_color_scheme(w, NORMAL_);
    cairo_fill(cri);

    cairo_rectangle(cri, 0.0, height, width, height);
    use_base_color_scheme(w, NORMAL_);
    cairo_fill(cri);

    cairo_pattern_t *pat = cairo_pattern_create_linear (0, 0, width, 0.0);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.1, 0.5, 0.1, 0.4);
    cairo_pattern_add_color_stop_rgba(pat, 0.8, 0.4, 0.4, 0.1, 0.4);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.5, 0.0, 0.0, 0.4);
    cairo_set_source(cri, pat);

    int c = (height)/2 ;
    int ci = c-2;

    int i = 2;
    int j = 1;
    for(;i<width;) {
        for(;j<height;) {
            cairo_rectangle(cri,i,j,4,ci);
            cairo_fill(cri);
            j +=c;
        }
        i +=6;
        j = 1;
    }

    cairo_pattern_destroy (pat);
    pat = cairo_pattern_create_linear (0, 0, width, 0.0);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.1, 0.5, 0.1, 1);
    cairo_pattern_add_color_stop_rgba(pat, 0.8, 0.4, 0.4, 0.1, 1);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.5, 0.0, 0.0, 1);
    cairo_set_source(cri, pat);
    i = 2;
    j = 1;
    for(;i<width;) {
        for(;j<height;) {
            cairo_rectangle(cri,i,height+j,4,ci);
            cairo_fill(cri);
            j +=c;
        }
        i +=6;
        j = 1;
    }

    cairo_pattern_destroy (pat);
    cairo_destroy(cri);
}

/**
 * @brief _draw_v_meter          - internal draw the meter to the buffer
 * @param *w_                    - void pointer to the Widget_t button
 * @param *user_data             - void pointer to attached user_data
 * @return void
 */

void _draw_v_meter(void *w_, void* user_data) {
     Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    int height_t = attrs.height;

    int width = cairo_xlib_surface_get_width(w->image);
    int height = cairo_xlib_surface_get_height(w->image);
    double x = (double)width_t/((double)width/2);
    double y = ((double)width/2)/(double)width_t;
    double meterstate = adj_get_state(w->adj_y);
    cairo_scale(w->crb, x,x);
    cairo_set_source_surface (w->crb, w->image, 0, 0);
    cairo_rectangle(w->crb,0, 0, width/2, height);
    cairo_fill(w->crb);
    cairo_set_source_surface (w->crb, w->image, -width/2, 0);
    cairo_rectangle(w->crb, 0, height, width/2, -height*meterstate);
    cairo_fill(w->crb);
    cairo_scale(w->crb, y,y);
}

/**
 * @brief _draw_h_meter          - internal draw the meter to the buffer
 * @param *w_                    - void pointer to the Widget_t button
 * @param *user_data             - void pointer to attached user_data
 * @return void
 */

void _draw_h_meter(void *w_, void* user_data) {
     Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    int height_t = attrs.height;

    int width = cairo_xlib_surface_get_width(w->image);
    int height = cairo_xlib_surface_get_height(w->image);
    double x = (double)width_t/((double)width);
    double y = ((double)width)/(double)width_t;
    double meterstate = adj_get_state(w->adj_x);
    cairo_scale(w->crb, x,x);
    cairo_set_source_surface (w->crb, w->image, 0, 0);
    cairo_rectangle(w->crb,0, 0, width, height/2);
    cairo_fill(w->crb);
    cairo_set_source_surface (w->crb, w->image, 0, -height/2);
    cairo_rectangle(w->crb, 0, 0, width*meterstate, height/2);
    cairo_fill(w->crb);
    cairo_scale(w->crb, y,y);
}
