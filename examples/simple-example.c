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
    cairo_set_source_rgb (w->crb, 1, 1, 1);
    cairo_paint (w->crb);
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    Widget_t *w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 200);
    XStoreName(app.dpy, w->widget, "Hello world");
    w->func.expose_callback = draw_window;
    widget_show_all(w);
    main_run(&app);
    main_quit(&app);
    return 0;
}
