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

typedef struct {
    Widget_t *w;
    Widget_t *w_quit;
    bool run;
} MyWindow;

// draw a button
static void draw_button(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_new_path (w->cr);
    cairo_move_to  (w->cr, 2, (2 + (height-4))/2);
    cairo_curve_to (w->cr, 2 ,2, 2, 2, (2 + width)/2, 2);
    cairo_curve_to (w->cr, width, 2, width, 2, width, (2 + height)/2);
    cairo_curve_to (w->cr, width, height, width, height, (width + 2)/2, height);
    cairo_curve_to (w->cr, 2, height, 2, height, 2, (2 + height)/2);
    cairo_close_path (w->cr);

    cairo_set_line_width(w->cr, 1.0);
    cairo_set_source_rgb (w->cr, 0., 0.1, 0.1);
    if(w->state==1) {
        cairo_set_line_width(w->cr, 2.0);
        cairo_set_source_rgb (w->cr, 0.2, 0.2, 0.2);
    }
    if(w->state==2) {
        cairo_set_line_width(w->cr, 2.5);
        cairo_set_source_rgb (w->cr, 0., 0.1, 0.1);
     }
    cairo_stroke(w->cr); 

    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->cr, 0.1, 0.2, 0.3);
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

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;
    double state_x = adj_get_state(w->adj_x);
    double state_y = adj_get_state(w->adj_y);
    
    double pos_x1 = 1 + (width * state_x);
    double pos_y1 = 1 + (height * state_y);
    double pos_x2 = width/2. + ((width/3.2) * (state_x-0.5));
    double pos_y2 = height/2 - ((width/3.2) * (state_y-0.5));

    cairo_push_group (w->cr);
    cairo_pattern_t *pat;

    pat = cairo_pattern_create_linear (0.0, 0.0,  0.0+pos_y1, width+pos_x1);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_rectangle(w->crb,0,0,width,height);
    cairo_set_source (w->crb, pat);
    cairo_fill (w->crb);
    cairo_pattern_destroy (pat);

    pat = cairo_pattern_create_radial (pos_x2, pos_y2, 25.6,
                                       width/2.8, height/2.8, width/2);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_set_source (w->crb, pat);
    cairo_arc (w->crb, width/2, height/2, width/4, 0, 2 * M_PI);
    cairo_fill (w->crb);
    cairo_pattern_destroy (pat);

    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->crb, 0.1, 0.1, 0.1);
    cairo_set_font_size (w->crb, 10.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, (width-extents.width)*0.5, (height-extents.height));
    cairo_show_text(w->crb, w->label);

    cairo_set_source_surface (w->cr, w->buffer,0,0);
    cairo_paint (w->cr);

    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
}

static void draw_motion(void *w_, void *motion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    float x = adj_get_value(w->adj_x);
    float y = adj_get_value(w->adj_y);
    char s[10];
    char sa[30];
    
    const char* format[] = {"%.1f", "%.2f", "%.3f"};
    snprintf(s, 30, "%s", "x = ");
    strcat(sa, s);
    snprintf(s, 30, format[2-1], x);
    strcat(sa, s);
    snprintf(s, 30, "%s", " y = ");
    strcat(sa, s);
    snprintf(s, 30, format[2-1], y);
    strcat(sa, s);
    
    w->label = sa;   
    transparent_draw(w_,NULL);
}

static void window_button_press(void *w_, void* button, void* user_data) {
     Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    w->label = "Move it, moce it";   
    draw_window(w_,NULL);
}

static void window_button_release(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    w->label = "Press mouse button and move:";
    draw_window(w_,NULL);
}

int main (int argc, char ** argv)
{
    Display *dpy = XOpenDisplay(0);
    XContext context =  XUniqueContext();
    MyWindow mywindow;
    
    mywindow.w = create_window(dpy, DefaultRootWindow(dpy), context, 0, 0, 330, 200);
    XStoreName(dpy, mywindow.w->widget, "Xputty Movement");
    mywindow.w->label = "Press mouse button and move:";
    mywindow.w->func.expose_callback = draw_window;
    mywindow.w->func.motion_callback = draw_motion;
    mywindow.w->adj_x = add_adjustment(mywindow.w,0.0, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    mywindow.w->adj_y = add_adjustment(mywindow.w,0.0, 0.5, 0.0, 1.0, 0.01, CL_CONTINUOS);
    mywindow.w->func.button_press_callback = window_button_press;
    mywindow.w->func.button_release_callback = window_button_release;

    mywindow.w_quit = create_widget(dpy, mywindow.w, context, 260, 170, 60, 20);
    mywindow.w_quit->label = "Quit";
    mywindow.w_quit->scale.gravity = NONE;
    mywindow.w_quit->func.expose_callback = draw_button;
    mywindow.w_quit->func.enter_callback = transparent_draw;
    mywindow.w_quit->func.button_press_callback = button_press;
    mywindow.w_quit->func.button_release_callback = button_release;
    mywindow.w_quit->func.leave_callback = transparent_draw;

    mywindow.run = true;
    
    loop(mywindow.w,context,&mywindow.run);

    destroy_widget( mywindow.w_quit, context);
    destroy_widget( mywindow.w, context);
    
    XCloseDisplay(dpy);

    return 0;
}
