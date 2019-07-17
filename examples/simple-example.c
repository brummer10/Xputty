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

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    cairo_push_group (w->cr);
    cairo_set_source_rgb (w->cr, 1, 1, 1);
    cairo_paint (w->cr);
    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
}

int main (int argc, char ** argv)
{
    Display *dpy = XOpenDisplay(0);
    XContext context =  XUniqueContext();
    Widget_t *w;

    w = create_window(dpy, DefaultRootWindow(dpy), context, 0, 0, 300, 200);
    XStoreName(dpy, w->widget, "Xputty Hello world");
    w->label = "How are you?";
    w->func.expose_callback = draw_window;
    bool run = true;
    loop(w,context,&run);
    destroy_widget( w, context);
    XCloseDisplay(dpy);
    return 0;
}
