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


#include "xtooltip.h"

/**
 * @brief _draw_tooltip        - draw tooltip on expose call
 * @param *w_                  - the tooltip to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_tooltip(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    use_bg_color_scheme(w, get_color_state(w));
    cairo_paint (w->crb);
    cairo_text_extents_t extents;
    /** show label **/
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, 12);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, (width-extents.width)/2., height - extents.height );
    cairo_show_text(w->crb, w->label);
    
}

/**
 * @brief add_tooltip          - add a tooltip to Widget_t
 * @param *w                   - pointer to the Widget_t request the tooltip
 * @param *label               - the tooltip text
 * @return void
 */

void add_tooltip(Widget_t *w, const char* label) {
    cairo_text_extents_t extents;
    cairo_set_font_size (w->crb, 12);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,label , &extents);
    
    int width = max(1, (int)extents.width+40);
    Widget_t *wid = create_tooltip(w, width, 25);
    wid->label = label;
}

/**
 * @brief create_tooltip      - create a tooltip for a Widget_t
 * @param *parent             - pointer to the Widget_t the tooltip should pop over
 * @param width               - define the width of the tooltip
 * @param height              - define the height of the tooltip
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* create_tooltip(Widget_t *parent, int width, int height) {

    int x1, y1;
    Window child;
    XTranslateCoordinates( parent->app->dpy, parent->widget, DefaultRootWindow(parent->app->dpy), 0, 0, &x1, &y1, &child );
    Widget_t *wid = create_window(parent->app, DefaultRootWindow(parent->app->dpy), x1+10, y1+10, width, height);
    Atom window_type = XInternAtom(wid->app->dpy, "_NET_WM_WINDOW_TYPE", False);
    long vale = XInternAtom(wid->app->dpy, "_NET_WM_WINDOW_TYPE_TOOLTIP", False);
    XChangeProperty(wid->app->dpy, wid->widget, window_type,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &vale,1 );
    XSetTransientForHint(parent->app->dpy,wid->widget,parent->widget);
    wid->func.expose_callback = _draw_tooltip;
    wid->flags |= IS_TOOLTIP;
    parent->flags |= HAS_TOOLTIP;
    wid->scale.gravity = NONE;
    childlist_add_child(parent->childlist, wid);
    return wid;
}
