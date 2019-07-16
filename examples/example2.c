/*
 * This is free and unencumbered software released into the public domain.

 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.

 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
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

    cairo_push_group (w->cr);

    cairo_set_source_rgb (w->cr, 0.0, 0.1, 0.1);
    if(w->state==1)
        cairo_set_source_rgb (w->cr, 0.05, 0.15, 0.15);
    if(w->state==2)
        cairo_set_source_rgb (w->cr, 0.2, 0.15, 0.15);
    cairo_rectangle(w->cr,0,0,width,height);
    cairo_fill_preserve (w->cr);
    cairo_set_source_rgb (w->cr, 0.6, 0.6, 0.6);
    if(w->state==1)
        cairo_set_source_rgb (w->cr, 0.8, 0.8, 0.8);
    cairo_set_line_width(w->cr, 1.0);
    if(w->state==2) {
        cairo_set_source_rgb (w->cr, 0.2, 0.8, 0.8);
        cairo_set_line_width(w->cr, 2.0);
    }
    cairo_stroke(w->cr);

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

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_push_group (w->cr);
    
    cairo_pattern_t *pat;

    pat = cairo_pattern_create_linear (0.0, 0.0,  0.0, width);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_rectangle(w->cr,0,0,width,height);
    cairo_set_source (w->cr, pat);
    cairo_fill (w->cr);
    cairo_pattern_destroy (pat);

    pat = cairo_pattern_create_radial (width/2.5, height/2.5, 25.6,
                                       width/2.8, height/2.8, width/2);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_set_source (w->cr, pat);
    cairo_arc (w->cr, width/2, height/2, width/4, 0, 2 * M_PI);
    cairo_fill (w->cr);
    cairo_pattern_destroy (pat);

    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->cr, 0.6, 0.6, 0.6);
    cairo_set_font_size (w->cr, 10.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->cr,w->label , &extents);

    cairo_move_to (w->cr, (width-extents.width)*0.5, (height-extents.height));
    cairo_show_text(w->cr, w->label);

    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
}

static void draw_motion(void *w_, void *motion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;
    XMotionEvent *motion = (XMotionEvent*)motion_;
    int x = (motion->x) - w->pos_x;
    int y = (motion->y) - w->pos_y;

    cairo_push_group (w->cr);
    
    cairo_pattern_t *pat;

    pat = cairo_pattern_create_linear (0.0, 0.0,  0.0+y, width+x);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_rectangle(w->cr,0,0,width,height);
    cairo_set_source (w->cr, pat);
    cairo_fill (w->cr);
    cairo_pattern_destroy (pat);

    pat = cairo_pattern_create_radial (width/2.5+x, height/2.5+y, 25.6,
                                       width/2.8, height/2.8, width/2);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_set_source (w->cr, pat);
    cairo_arc (w->cr, width/2, height/2, width/4, 0, 2 * M_PI);
    cairo_fill (w->cr);
    cairo_pattern_destroy (pat);

    char s[64];
    const char* format[] = {"%.1f", "%.2f", "%.3f"};
    snprintf(s, 63," x = %d y = %d", x ,y );
    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->cr, 0.6, 0.6, 0.6);
    cairo_set_font_size (w->cr, 10.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->cr,s , &extents);

    cairo_move_to (w->cr, (width-extents.width)*0.5, (height-extents.height));
    cairo_show_text(w->cr, s);
    
    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
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

    mywindow.w_quit = create_widget(dpy, mywindow.w->widget, context, 260, 170, 60, 20);
    mywindow.w_quit->label = "Quit";
    mywindow.w_quit->func.expose_callback = draw_button;
    mywindow.w_quit->func.enter_callback = draw_button;
    mywindow.w_quit->func.button_press_callback = button_press;
    mywindow.w_quit->func.button_release_callback = button_release;
    mywindow.w_quit->func.leave_callback = draw_button;

    mywindow.run = true;
    
    loop(mywindow.w,context,&mywindow.run);

    destroy_widget( mywindow.w_quit, context);
    destroy_widget( mywindow.w, context);
    
    XCloseDisplay(dpy);

    return 0;
}
