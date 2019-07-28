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

static void set_colors(Xputty *app) {
    app->color_scheme->normal = (Colors){
         /* cairo/ r / g / b / a /  */
         /*fg*/{ 0.85, 0.85, 0.85, 1.0},
         /*bg*/{ 0.3, 0.4, 0.5, 1.0},
         /*base*/{ 0.0, 0.0, 0.0, 0.2},
         /*text*/{ 0.9, 0.9, 0.9, 1.0}};

    app->color_scheme->prelight = (Colors){
        /*fg*/{ 1.0, 1.0, 1.0, 1.0},
        /*bg*/{ 0.25, 0.25, 0.25, 1.0},
        /*base*/{ 0.1, 0.1, 0.1, 0.4},
        /*text*/{ 1.0, 1.0, 1.0, 1.0}};

    app->color_scheme->selected = (Colors){
        /*fg*/{ 0.9, 0.9, 0.9, 1.0},
        /*bg*/{ 0.2, 0.2, 0.2, 1.0},
        /*base*/{ 0.8, 0.18, 0.18, 0.2},
         /*text*/{ 1.0, 1.0, 1.0, 1.0}};

    app->color_scheme->active = (Colors){
        /*fg*/{ 1.0, 1.0, 1.0, 1.0},
        /*bg*/{ 0.0, 0.0, 0.0, 1.0},
        /*base*/{ 0.68, 0.28, 0.28, 0.5},
        /*text*/{ 0.75, 0.75, 0.75, 1.0}};
}

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    use_bg_color_scheme(w, get_color_state(w));
    cairo_paint (w->crb);
}

static void button_quit_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    
    if (w->has_pointer && !adj_get_value(w->adj_y)){
        Widget_t *p = (Widget_t*)w->parent;
        quit(p);
    }

}

static void button_thema_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    
    if (w->has_pointer && !adj_get_value(w->adj_y)){
        w->app->color_scheme->normal = (Colors){
         /* cairo/ r  / g  / b  / a  /  */
         /*fg*/{ 0.85, 0.85, 0.85, 1.0},
         /*bg*/{ 0.3, 0.4, 0.5, 1.0},
         /*base*/{ 0.0, 0.0, 0.0, 0.2},
         /*text*/{ 0.9, 0.9, 0.9, 1.0}};
    } else if (adj_get_value(w->adj_y)){
        w->app->color_scheme->normal = (Colors){
         /* cairo/ r  / g  / b  / a  /  */
         /*fg*/{ 0.15, 0.15, 0.15, 1.0},
         /*bg*/{ 0.4, 0.5, 0.3, 1.0},
         /*base*/{ 0.1, 0.2, 0.3, 0.4},
         /*text*/{ 0.19, 0.19, 0.19, 1.0}};
    }
    Widget_t *p = (Widget_t*)w->parent;
    expose_widget(p);
}

static void hslider_callback(void *w_, void* user_data) {
   // Widget_t *w = (Widget_t*)w_;
   //  fprintf(stderr, "hslider value changed %f\n", adj_get_value(w->adj_x));
}

static void vslider_callback(void *w_, void* user_data) {
   //  Widget_t *w = (Widget_t*)w_;
   //  fprintf(stderr, "vslider value changed %f\n", adj_get_value(w->adj_y));
}

static void knob_callback(void *w_, void* user_data) {
   //  Widget_t *w = (Widget_t*)w_;
   //  fprintf(stderr, "knob value changed %f\n", adj_get_value(w->adj_y));
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    set_colors(&app);
    Widget_t *b;
    Widget_t *w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 450);
    XStoreName(app.dpy, w->widget, "Color-scheme");
    w->func.expose_callback = draw_window;

    b = add_button(w, "Quit", 230, 410, 60, 30);
    b->scale.gravity = NONE;
    b->func.value_changed_callback = button_quit_callback;

    b = add_toggle_button(w, "Thema", 10, 410, 60, 30);
    b->scale.gravity = SOUTHEAST;
    b->func.value_changed_callback = button_thema_callback;

    b = add_hslider(w, "HSlider", 10, 10, 280, 40);
    b->func.value_changed_callback = hslider_callback;

    b = add_vslider(w, "Vslider", 120, 80, 40, 260);
    b->func.value_changed_callback = vslider_callback;

    b = add_vslider(w, "Slider1", 180, 80, 40, 260);
    b->func.value_changed_callback = vslider_callback;

    b = add_vslider(w, "Slider2", 240, 80, 40, 260);
    b->func.value_changed_callback = vslider_callback;

    b = add_knob(w, "Knob", 10, 60, 60, 80);
    b->func.value_changed_callback = knob_callback;

    b = add_knob(w, "Knob1", 10, 160, 60, 80);
    b->func.value_changed_callback = knob_callback;

    b = add_knob(w, "Knob2", 10, 260, 60, 80);
    b->func.value_changed_callback = knob_callback;

    main_run(&app);
    main_quit(&app);
    return 0;
}
