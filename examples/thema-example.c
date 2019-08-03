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

static void set_green(Widget_t *w) {
    w->app->color_scheme->normal = (Colors){
     /* cairo/ r  / g  / b  / a  /  */
     /*fg*/{ 0.15, 0.15, 0.15, 1.0},
     /*bg*/{ 0.4, 0.5, 0.3, 1.0},
     /*base*/{ 0.1, 0.2, 0.3, 0.4},
     /*text*/{ 0.19, 0.19, 0.19, 1.0}};
    expose_widget(get_toplevel_widget(w->app));
}

static void set_blue(Widget_t *w) {
    w->app->color_scheme->normal = (Colors){
     /* cairo/ r / g / b / a /  */
     /*fg*/{ 0.85, 0.85, 0.85, 1.0},
     /*bg*/{ 0.3, 0.4, 0.5, 1.0},
     /*base*/{ 0.0, 0.0, 0.0, 0.2},
     /*text*/{ 0.9, 0.9, 0.9, 1.0}};
    expose_widget(get_toplevel_widget(w->app));
}

static void set_gray(Widget_t *w) {
    w->app->color_scheme->normal = (Colors){
     /* cairo/ r / g / b / a /  */
     /*fg*/{ 0.15, 0.15, 0.15, 1.0},
     /*bg*/{ 0.8, 0.8, 0.8, 1.0},
     /*base*/{ 0.0, 0.0, 0.0, 0.2},
     /*text*/{ 0.19, 0.19, 0.19, 1.0}};
    expose_widget(get_toplevel_widget(w->app));
}

static void set_black(Widget_t *w) {
    w->app->color_scheme->normal = (Colors){
     /* cairo/ r / g / b / a /  */
     /*fg*/{ 0.95, 0.95, 0.95, 1.0},
     /*bg*/{ 0.18, 0.18, 0.18, 1.0},
     /*base*/{ 0.0, 0.0, 0.0, 0.2},
     /*text*/{ 0.9, 0.9, 0.9, 1.0}};
    expose_widget(get_toplevel_widget(w->app));
}

static void set_red(Widget_t *w) {
    w->app->color_scheme->normal = (Colors){
     /* cairo/ r / g / b / a /  */
     /*fg*/{ 0.95, 0.95, 0.95, 1.0},
     /*bg*/{ 0.68, 0.18, 0.18, 1.0},
     /*base*/{ 0.0, 0.0, 0.0, 0.2},
     /*text*/{ 0.9, 0.9, 0.9, 1.0}};
    expose_widget(get_toplevel_widget(w->app));
}

static void set_yellow(Widget_t *w) {
    w->app->color_scheme->normal = (Colors){
     /* cairo/ r / g / b / a /  */
     /*fg*/{ 0.15, 0.15, 0.15, 1.0},
     /*bg*/{ 0.78, 0.78, 0.18, 1.0},
     /*base*/{ 0.0, 0.0, 0.0, 0.2},
     /*text*/{ 0.1, 0.1, 0.1, 1.0}};
    expose_widget(get_toplevel_widget(w->app));
}

static void _pattern(Widget_t *w) {
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    set_pattern(w,&w->app->color_scheme->normal,&w->app->color_scheme->active,BACKGROUND_,width,height);
    
}

static void pattern(Widget_t *w) {
    Widget_t *p = get_toplevel_widget(w->app);
    p->data = (int)w->adj->value;
    if ((int)w->adj->value) {
        _pattern(p);
    }
    expose_widget(get_toplevel_widget(w->app));
}

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w->data) use_bg_color_scheme(w, get_color_state(w));
    else _pattern(w);
    cairo_paint (w->crb);
}

static void button_quit_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    
    if (w->has_pointer && !adj_get_value(w->adj)){
        quit(get_toplevel_widget(w->app));
    }

}

static void menu_response(void *w_, void* item_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    // fprintf(stderr, "selected item is %i with label %s\n", *(int*)item_, *(const char**)user_data);
    switch (*(int*)item_) {
        case 0: set_green(w);
        break;
        case 1: set_blue(w);
        break;
        case 2: set_gray(w);
        break;
        case 3: set_black(w);
        break;
        case 4: set_red(w);
        break;
        case 5: set_yellow(w);
        break;
        case 6: pattern(w->childlist->childs[6]);
        break;
        default:
        break;
    }
}

static void create_mymenu(Widget_t *w) {
    Widget_t *m = create_menu(w,25);
    menu_add_item(m,"green");
    menu_add_item(m,"blue");
    menu_add_item(m,"gray");
    menu_add_item(m,"black");
    menu_add_item(m,"red");
    menu_add_item(m,"yellow");
    menu_add_check_item(m,"pattern");
    m->func.button_release_callback = menu_response;
}

static void button_thema_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    
    if (w->has_pointer && adj_get_value(w->adj)){
        pop_menu_show(w, w->childlist->childs[0]);
    }
    adj_set_value(w->adj,0.0);
}

static void hslider_callback(void *w_, void* user_data) {
   // Widget_t *w = (Widget_t*)w_;
   //  fprintf(stderr, "hslider value changed %f\n", adj_get_value(w->adj));
}

static void vslider_callback(void *w_, void* user_data) {
   //  Widget_t *w = (Widget_t*)w_;
   //  fprintf(stderr, "vslider value changed %f\n", adj_get_value(w->adj));
}

static void knob_callback(void *w_, void* user_data) {
   //  Widget_t *w = (Widget_t*)w_;
   //  fprintf(stderr, "knob value changed %f\n", adj_get_value(w->adj));
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    set_colors(&app);
    Widget_t *b;
    Widget_t *bt;
    Widget_t *w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 450);
    XStoreName(app.dpy, w->widget, "Color-scheme");
    w->func.expose_callback = draw_window;

    b = add_button(w, "Quit", 230, 410, 60, 30);
    b->scale.gravity = SOUTHWEST;
    b->func.value_changed_callback = button_quit_callback;

    bt = add_toggle_button(w, "Theme", 10, 410, 60, 30);
    bt->scale.gravity = SOUTHEAST;
    bt->func.value_changed_callback = button_thema_callback;

    b = add_hslider(w, "HSlider", 10, 10, 280, 40);
    b->func.value_changed_callback = hslider_callback;

    b = add_vslider(w, "Vslider", 120, 80, 40, 260);
    b->func.value_changed_callback = vslider_callback;

    b = add_vslider(w, "Slider1", 180, 80, 40, 260);
    b->func.value_changed_callback = vslider_callback;

    b = add_vslider(w, "Slider2", 240, 80, 40, 260);
    b->func.value_changed_callback = vslider_callback;

    b = add_knob(w, "Knob", 20, 60, 60, 80);
    b->func.value_changed_callback = knob_callback;

    b = add_knob(w, "Knob1", 20, 160, 60, 80);
    b->func.value_changed_callback = knob_callback;

    b = add_knob(w, "Knob2", 20, 260, 60, 80);
    b->func.value_changed_callback = knob_callback;
    widget_show_all(w);
    create_mymenu(bt);

    main_run(&app);
    main_quit(&app);
    return 0;
}
