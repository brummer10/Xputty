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
#include "xmidi_keyboard.h"

static void draw_label(Widget_t *w, int width, int height) {
    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->crb, 0.0, 0.1, 0.1);
    cairo_set_font_size (w->crb, 12.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, (width-extents.width)*0.5, (height+extents.height)*0.5);
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);
}

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_pattern_t *pat;
    pat = cairo_pattern_create_linear (0.0, 0.0, width , height);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0.5, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 0.5, 1);
    cairo_rectangle(w->crb,0,0,width,height);
    cairo_set_source (w->crb, pat);
    cairo_fill (w->crb);
    cairo_pattern_destroy (pat);

    draw_label(w,width,height);
}

static void button_quit_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t *p = (Widget_t*)w->parent;
        quit(p);
    }

}

static void button_ok_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t *p = (Widget_t*)w->parent;
        Widget_t *m = open_midi_keyboard(p);
        widget_show_all(m);
        //quit_widget(w);
        //quit_widget(p);
    }

}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    Widget_t *w;
    Widget_t *w_quit;
    
    w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 200);
    widget_set_title(w, "Xputty Hello world");
    w->label = "How are you?";
    w->func.expose_callback = draw_window;

    Screen *screen = ScreenOfDisplay(app.dpy,0);
    int center_x = screen->width/2 - 150;
    int center_y = screen->height/2 - 100; 
    XMoveWindow(w->app->dpy,w->widget,center_x, center_y);

    w_quit = add_button(w, "Quit", 230, 170, 60, 20);
    w_quit->scale.gravity = SOUTHWEST;
    connect_func(&w_quit->func.value_changed_callback, &button_quit_callback);
    widget_show_all(w);

    w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 200);
    widget_set_title(w, "Xputty Message Box");
    w->label = "This is a message";
    connect_func(&w->func.expose_callback,&draw_window);
    double alpha = 0.5;
    unsigned long opacity = (unsigned long)(0xFFFFFFFFul * alpha);
    Atom XA_NET_WM_WINDOW_OPACITY = XInternAtom(app.dpy, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(app.dpy, w->widget, XA_NET_WM_WINDOW_OPACITY, XA_CARDINAL, 32,
                    PropModeReplace, (unsigned char *)&opacity, 1L);   

    w_quit = add_button(w, "OK", 230, 170, 60, 20);
    w_quit->scale.gravity = SOUTHWEST;
    connect_func(&w_quit->func.value_changed_callback,button_ok_callback);
    //w_quit->func.value_changed_callback = button_ok_callback;
    widget_show_all(w);
    XMoveWindow(w->app->dpy,w->widget,center_x+60, center_y+60);

    main_run(&app);
   
    main_quit(&app);

    return 0;
    
    
}
