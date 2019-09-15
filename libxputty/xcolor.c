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

#include "xcolor.h"


/**
 * @brief set_color_scheme  - init color shema to default values 
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */

void set_color_scheme(Xputty *main) {
    main->color_scheme->normal = (Colors){
         /* cairo/ r / g / b / a /  */
         /*fg*/{ 0.85, 0.85, 0.85, 1.0},
         /*bg*/{ 0.1, 0.1, 0.1, 1.0},
         /*base*/{ 0.0, 0.0, 0.0, 0.2},
         /*text*/{ 0.9, 0.9, 0.9, 1.0}};

    main->color_scheme->prelight = (Colors){
        /*fg*/{ 1.0, 1.0, 1.0, 1.0},
        /*bg*/{ 0.25, 0.25, 0.25, 1.0},
        /*base*/{ 0.1, 0.1, 0.1, 0.4},
        /*text*/{ 1.0, 1.0, 1.0, 1.0}};

    main->color_scheme->selected = (Colors){
        /*fg*/{ 0.9, 0.9, 0.9, 1.0},
        /*bg*/{ 0.2, 0.2, 0.2, 1.0},
        /*base*/{ 0.8, 0.18, 0.18, 0.2},
         /*text*/{ 1.0, 1.0, 1.0, 1.0}};

    main->color_scheme->active = (Colors){
        /*fg*/{ 1.0, 1.0, 1.0, 1.0},
        /*bg*/{ 0.0, 0.0, 0.0, 1.0},
        /*base*/{ 0.18, 0.38, 0.38, 0.5},
        /*text*/{ 0.75, 0.75, 0.75, 1.0}};
}

/**
 * @brief get_color_scheme  - init color shema to default values 
 * @param *main             - pointer to the main Xputty struct
 * @param st               - the Color_state to use
 * @return Colors           - the Colors scheme asosiated with the Color_state 
 */

Colors *get_color_scheme(Xputty *main, Color_state st) {
    switch(st) {
        case NORMAL_:
            return &main->color_scheme->normal;
        break;
        case PRELIGHT_:
            return &main->color_scheme->prelight;
        break;
        case SELECTED_:
            return &main->color_scheme->selected;
        break;
        case ACTIVE_:
            return &main->color_scheme->active;
        break;
        default:
        break;
    }
    return NULL;
   
}

/**
 * @brief get_color_state   - get the Color_state to use
 * @param *wid              - pointer to the Widget_t
 * @return Color_state      - the Color_state related to the Widget_t state
 */

Color_state get_color_state(Widget_t *wid) {
    switch(wid->state) {
        case 0:
            return NORMAL_;
        break;
        case 1:
            return PRELIGHT_;
        break;
        case 2:
            return SELECTED_;
        break;
        case 3:
            return ACTIVE_;
        break;
        default:
            return NORMAL_;
        break;        
    }
}

/**
 * @brief use_fg_color_scheme  - set normal forground color for Widget_t
 * @param w                    - the Widget_t to send the event to
 * @param st                   - the Color_state to use
 * @return void 
 */

void use_fg_color_scheme(Widget_t *w, Color_state st) {
    Colors *c = get_color_scheme(w->app, st);
    if (!c) return;
    cairo_set_source_rgba(w->cr, c->fg[0],  c->fg[1], c->fg[2],  c->fg[3]);
    cairo_set_source_rgba(w->crb, c->fg[0],  c->fg[1], c->fg[2],  c->fg[3]);
}

/**
 * @brief use_bg_color_scheme  - set normal background color for Widget_t
 * @param w                    - the Widget_t to send the event to
 * @param st                   - the Color_state to use
 * @return void 
 */

void use_bg_color_scheme(Widget_t *w, Color_state st) {
    Colors *c = get_color_scheme(w->app, st);
    if (!c) return;
    cairo_set_source_rgba(w->cr, c->bg[0],  c->bg[1], c->bg[2],  c->bg[3]);
    cairo_set_source_rgba(w->crb, c->bg[0],  c->bg[1], c->bg[2],  c->bg[3]);
}

/**
 * @brief use_text_color_scheme  - set text color for Widget_t
 * @param w                      - the Widget_t to send the event to
 * @param st                   - the Color_state to use
 * @return void 
 */

void use_base_color_scheme(Widget_t *w, Color_state st) {
    Colors *c = get_color_scheme(w->app, st);
    if (!c) return;
    cairo_set_source_rgba(w->cr, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_set_source_rgba(w->crb, c->base[0],  c->base[1], c->base[2],  c->base[3]);
}

/**
 * @brief use_text_color_scheme  - set text color for Widget_t
 * @param w                      - the Widget_t to send the event to
 * @param st                   - the Color_state to use
 * @return void 
 */

void use_text_color_scheme(Widget_t *w, Color_state st) {
    Colors *c = get_color_scheme(w->app, st);
    if (!c) return;
    cairo_set_source_rgba(w->cr, c->text[0],  c->text[1], c->text[2],  c->text[3]);
    cairo_set_source_rgba(w->crb, c->text[0],  c->text[1], c->text[2],  c->text[3]);
}

/**
 * @brief set_pattern       - set pattern for the col_fromted colors
 * @param *w_               - void pointer to the Widget_t button
 * @param *from             - the Colors set to use from
 * @param *to               - the Colors set to use to
 * @param mod               - the Color_mod to use
 * @param width             - the width of the base
 * @return void
 */

void set_pattern(Widget_t *w, Colors *from, Colors *to, Color_mod mod) {
    float *col_from;
    float *col_to;
    switch (mod) {
        case FORGROUND_:
            col_from = from->fg;
            col_to = to->fg;
        break;
        case BACKGROUND_:
            col_from = from->bg;
            col_to = to->bg;
        break;
        case BASE_:
            col_from = from->base;
            col_to = to->base;
        break;
        case TEXT_:
            col_from = from->text;
            col_to = to->text;
        break;
    }
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    cairo_pattern_t *pat = cairo_pattern_create_linear (0, 0, width, height);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, col_from[0], col_from[1], col_from[2], col_from[3]);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, col_to[0], col_to[1], col_to[2], col_to[3]);
    cairo_set_source(w->crb, pat);
    cairo_pattern_destroy (pat);
}
