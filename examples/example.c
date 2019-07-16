/*
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                   Version 2, December 2004
 *
 * Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 * 0. You just DO WHAT THE FUCK YOU WANT TO.
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
    Display *dpy = XOpenDisplay(0);
    XContext context =  XUniqueContext();
    Widget_t *w;
    Widget_t *w_quit;
    
    w = create_window(dpy, DefaultRootWindow(dpy), context, 0, 0, 300, 200);
    XStoreName(dpy, w->widget, "Xputty Hello world");
    w->label = "How are you?";
    w->func.expose_callback = draw_window;

    w_quit = create_widget(dpy, w->widget, context, 230, 170, 60, 20);
    w_quit->label = "OK";
    w_quit->func.expose_callback = draw_button;
    w_quit->func.enter_callback = draw_button;
    w_quit->func.button_press_callback = button_press;
    w_quit->func.button_release_callback = button_release;
    w_quit->func.leave_callback = draw_button;

    bool run = true;
    
    loop(w,context,&run);

    destroy_widget( w_quit, context);
    destroy_widget( w, context);
    
    XCloseDisplay(dpy);

    return 0;
    
    
}
