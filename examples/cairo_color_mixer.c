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


#include "xwidgets.h"

float r,g,b,a;

static void set_color(Widget_t *w) {
    w->app->color_scheme->normal = (Colors){
        /* cairo    /  r   /  g   /  b   /  a   /  */
        .fg =       { 0.15, 0.15, 0.15, 1.0},
        .bg =       { r, g, b, a},
        .base =     { 0.1, 0.2, 0.3, 0.4},
        .text =     { 1.0-r, 1.0-g, 1.0-b, 1.0},
        .shadow =   { 0.0, 0.0, 0.0, 0.2},
        .frame =    { 0.0, 0.0, 0.0, 1.0},
        .light =    { 0.1, 0.1, 0.1, 1.0}};
    expose_widget(get_toplevel_widget(w->app));
}

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    cairo_set_source_rgb (w->crb, 0.95, 0.95, 0.95);
    cairo_paint (w->crb);
    cairo_rectangle(w->crb,0,300,150,150);
    cairo_set_source_rgb (w->crb, 0.0, 0.0, 0.0);
    cairo_fill (w->crb);
    cairo_rectangle(w->crb,150,300,150,150);
    cairo_set_source_rgb (w->crb, 1.0, 1.0, 1.0);
    cairo_fill (w->crb);
    
    use_bg_color_scheme(w, get_color_state(w));
    cairo_paint (w->crb);
}

void draw_my_slider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-2;
    float center = (float)width/2;
    float upcenter = (float)width;
    
    if (attrs.map_state != IsViewable) return;
    cairo_set_source_rgb (w->crb, 1.0, 1.0, 1.0);
    cairo_move_to (w->crb, center, center);
    cairo_line_to(w->crb,center,height-center-10);
    cairo_set_line_cap (w->crb,CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(w->crb,center);
    cairo_stroke(w->crb);

    float sliderstate = adj_get_state(w->adj_y);
    cairo_pattern_t *pat = cairo_pattern_create_linear (0, 0, 0, height);
    
    if (w->data == 1) {
        cairo_pattern_add_color_stop_rgba(pat, 1.0, 0, 0, 0, 0);
        cairo_pattern_add_color_stop_rgba(pat, 0.0, 1, 0, 0, 1);
    } else if (w->data == 2) {
        cairo_pattern_add_color_stop_rgba(pat, 1.0, 0, 0, 0, 0);
        cairo_pattern_add_color_stop_rgba(pat, 0.0, 0, 1, 0, 1);
    } else if (w->data == 3) {
        cairo_pattern_add_color_stop_rgba(pat, 1.0, 0, 0, 0, 0);
        cairo_pattern_add_color_stop_rgba(pat, 0.0, 0, 0, 1, 1);
    } else if (w->data == 4) {
        cairo_pattern_add_color_stop_rgba(pat, 1.0, 0, 0, 0, 0);
        cairo_pattern_add_color_stop_rgba(pat, 0.0, 0, 0, 0, 1);
    }
    cairo_set_source(w->crb, pat);
    
    cairo_move_to (w->crb, center, center);
    cairo_line_to(w->crb,center,height-center-10);
    cairo_set_line_cap (w->crb,CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(w->crb,center);
    cairo_stroke(w->crb);

    use_shadow_color_scheme(w, get_color_state(w));
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


    use_text_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->crb, center/2.1);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, center-extents.width/2, height-center/2.1);
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);

    char s[64];
    const char* format[] = {"%.1f", "%.2f", "%.3f"};
    if (fabs(w->adj_y->value)>9.99) {
        snprintf(s, 63,"%d",  (int) w->adj->value);
    } else if (fabs(w->adj_y->value)>0.99) {
        snprintf(s, 63, format[1-1], w->adj->value);
    } else {
        snprintf(s, 63, format[2-1], w->adj->value);
    }
    cairo_text_extents(w->crb,s , &extents);
    cairo_move_to (w->crb, center-extents.width/2, extents.height );
    cairo_show_text(w->crb, s);
    cairo_new_path (w->crb);
}

static void r_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    r = adj_get_value(w->adj);
    set_color(w);
}

static void g_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    g = adj_get_value(w->adj);
    set_color(w);
}

static void b_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    b = adj_get_value(w->adj);
    set_color(w);
}

static void a_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    a = adj_get_value(w->adj);
    set_color(w);
}

static void button_ok_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        fprintf(stderr, "( %.2f, %.2f, %.2f, %.2f );\n",r,g,b,a);
    }
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    Widget_t *sl;
    Widget_t *win = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 450);
    widget_set_title(win, "cairo-color-mixer");
    win->func.expose_callback = draw_window;

    sl = add_vslider(win, "R", 20, 40, 40, 200);
    sl->data = 1;
    sl->func.expose_callback = draw_my_slider;
    sl->func.value_changed_callback = r_callback;

    sl = add_vslider(win, "G", 80, 40, 40, 200);
    sl->data = 2;
    sl->func.expose_callback = draw_my_slider;
    sl->func.value_changed_callback = g_callback;

    sl = add_vslider(win, "B", 140, 40, 40, 200);
    sl->data = 3;
    sl->func.expose_callback = draw_my_slider;
    sl->func.value_changed_callback = b_callback;

    sl = add_vslider(win, "A", 200, 40, 40, 200);
    sl->data = 4;
    sl->func.expose_callback = draw_my_slider;
    sl->func.value_changed_callback = a_callback;

    sl = add_button(win, "Print", 220, 260, 60, 30);
    signal_connect_func(sl,VALUE_CHANGED,button_ok_callback);

    widget_show_all(win);
    set_color(win);
    main_run(&app);
    main_quit(&app);
    return 0;
}
