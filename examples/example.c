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

static void draw_label(Widget_t *w, int width, int height) {
    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->cr, 0.6, 0.6, 0.6);
    if(w->state==1)
        cairo_set_source_rgb (w->cr, 0.8, 0.8, 0.8);
    if(w->state==2)
        cairo_set_source_rgb (w->cr, 0.1, 0.8, 0.8);
    cairo_set_font_size (w->cr, 12.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->cr,w->label , &extents);

    cairo_move_to (w->cr, (width-extents.width)*0.5, (height+extents.height)*0.5);
    cairo_show_text(w->cr, w->label);
    cairo_new_path (w->cr);
}

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_push_group (w->cr);

    cairo_set_source_rgb (w->cr, 0.05, 0.15, 0.15);
    cairo_paint (w->cr);

    draw_label(w,width,height);

    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
}

static void draw_button(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_push_group (w->cr);
    cairo_set_source_rgb (w->cr, 0.5, 0.15, 0.15);
    cairo_paint (w->cr);

    draw_label(w,width,height);

    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
}

static void button_press(void *w_, void* button, void* user_data) {
    draw_button(w_, user_data);
}

static void button_release(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    draw_button(w_, NULL);
    if (w->has_pointer){
        quit(w);
    }

}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    Widget_t *w;
    Widget_t *w_quit;
    
    w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 200);
    XStoreName(app.dpy, w->widget, "Xputty Hello world");
    w->label = "How are you?";
    w->func.expose_callback = draw_window;

    w_quit = create_widget(&app, w, 230, 170, 60, 20);
    w_quit->label = "OK";
    w_quit->func.expose_callback = draw_button;
    w_quit->func.enter_callback = draw_button;
    w_quit->func.button_press_callback = button_press;
    w_quit->func.button_release_callback = button_release;
    w_quit->func.leave_callback = draw_button;

    bool run = true;
    
    main_run(&app);
   
    main_quit(&app);

    return 0;
    
    
}
