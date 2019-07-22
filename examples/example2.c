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


#include "xputty.h"


// draw a button
static void draw_button(void *w_, void* buffer_context) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    cairo_t * cr = (cairo_t*) buffer_context;
    if (!cr) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-2;
    if (attrs.map_state != IsViewable) return;

    cairo_new_path (cr);
    cairo_move_to  (cr, 2, (2 + (height-4))/2);
    cairo_curve_to (cr, 2 ,2, 2, 2, (2 + width)/2, 2);
    cairo_curve_to (cr, width, 2, width, 2, width, (2 + height)/2);
    cairo_curve_to (cr, width, height, width, height, (width + 2)/2, height);
    cairo_curve_to (cr, 2, height, 2, height, 2, (2 + height)/2);
    cairo_close_path (cr);

    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgb (cr, 0., 0.1, 0.1);
    if(w->state==1) {
        cairo_set_line_width(cr, 1.5);
        cairo_set_source_rgb (cr, 0.2, 0.2, 0.2);
    }
    if(w->state==2) {
        cairo_set_source_rgba (cr, 0., 0.1, 0.1, 0.2);
        cairo_fill_preserve(cr);
        cairo_set_line_width(cr, 2.0);
        cairo_set_source_rgb (cr, 0., 0.1, 0.1);
     }
    cairo_stroke(cr); 

    cairo_text_extents_t extents;
    cairo_set_source_rgb (cr, 0.1, 0.2, 0.3);
    if(w->state==1)
        cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
    if(w->state==2)
        cairo_set_source_rgb (cr, 0.1, 0.8, 0.8);
    cairo_set_font_size (cr, 12.0);
    cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(cr,w->label , &extents);

    cairo_move_to (cr, (width-extents.width)*0.5, (height+extents.height)*0.5);
    cairo_show_text(cr, w->label);
    cairo_new_path (cr);
}

static void button_press(void *w_, void* button, void* user_data) {
    expose_widget(w_);
}

static void button_release(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    expose_widget(w_);
    if (w->has_pointer){
        quit(w);
    }

}

static void button_reset_release(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    expose_widget(w_);
    if (w->has_pointer){
        Widget_t *parent = w->parent;
        adj_set_value(parent->adj_x,parent->adj_x->std_value);
        adj_set_value(parent->adj_y,parent->adj_y->std_value);
    }

}

static void draw_window(void *w_, void* buffer_context) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    cairo_t * cr = (cairo_t*) buffer_context;
    if (!cr) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;
    double state_x = adj_get_state(w->adj_x);
    double state_y = adj_get_state(w->adj_y);
    
    double pos_x1 = 1 + (width * state_x);
    double pos_y1 = 1 + (height * state_y);
    double pos_x2 = width/2. + ((width/3.2) * (state_x-0.5));
    double pos_y2 = height/2 - ((width/3.2) * (state_y-0.5));

    cairo_pattern_t *pat;

    pat = cairo_pattern_create_linear (0.0, 0.0,  0.0+pos_y1, width+pos_x1);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_rectangle(cr,0,0,width,height);
    cairo_set_source (cr, pat);
    cairo_fill (cr);
    cairo_pattern_destroy (pat);

    pat = cairo_pattern_create_radial (pos_x2, pos_y2, 25.6,
                                       width/2.8, height/2.8, width/2);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_set_source (cr, pat);
    cairo_arc (cr, width/2, height/2, width/4, 0, 2 * M_PI);
    cairo_fill (cr);
    cairo_pattern_destroy (pat);

    cairo_text_extents_t extents;
    cairo_set_source_rgb (cr, 0.1, 0.1, 0.1);
    cairo_set_font_size (cr, 10.0);
    cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(cr,w->label , &extents);

    cairo_move_to (cr, (width-extents.width)*0.5, (height-extents.height));
    cairo_show_text(cr, w->label);
}

static void draw_motion(void *w_, void *motion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    float x = adj_get_value(w->adj_x);
    float y = adj_get_value(w->adj_y);
    char s[10];
    char sa[30];
    
    const char* format[] = {"%.1f", "%.2f", "%.3f"};
    snprintf(s, 30, "%s", "x = ");
    strcat(sa, s);
    snprintf(s, 30, format[2-1], x);
    strcat(sa, s);
    snprintf(s, 30, "%s", " y = ");
    strcat(sa, s);
    snprintf(s, 30, format[2-1], y);
    strcat(sa, s);
    
    w->label = sa;   
    transparent_draw(w_,NULL);
}

static void window_button_press(void *w_, void* button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    w->label = "Move it, move it";   
    transparent_draw(w_,NULL);
}

static void window_button_release(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    w->label = "Press mouse button and move:";
    transparent_draw(w_,NULL);
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    Widget_t *w;
    Widget_t *b;

    w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 330, 200);
    XStoreName(app.dpy, w->widget, "Xputty Movement");
    w->label = "Press mouse button and move:";
    w->func.expose_callback = draw_window;
    w->func.motion_callback = draw_motion;
    w->adj_x = add_adjustment(w,0.5, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    w->adj_y = add_adjustment(w,0.5, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    w->func.button_press_callback = window_button_press;
    w->func.button_release_callback = window_button_release;

    b = create_widget(&app, w, 260, 170, 60, 20);
    b->label = "Quit";
    b->scale.gravity = NONE;
    b->func.expose_callback = draw_button;
    b->func.enter_callback = transparent_draw;
    b->func.button_press_callback = button_press;
    b->func.button_release_callback = button_release;
    b->func.leave_callback = transparent_draw;

    b = create_widget(&app, w, 10, 170, 60, 20);
    b->label = "Reset";
    b->scale.gravity = SOUTHEAST;
    b->func.expose_callback = draw_button;
    b->func.enter_callback = transparent_draw;
    b->func.button_press_callback = button_press;
    b->func.button_release_callback = button_reset_release;
    b->func.leave_callback = transparent_draw;

    main_run(&app);

    main_quit(&app);
    return 0;
}
