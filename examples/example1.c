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

typedef struct {
    Widget_t *w;
    Widget_t *w_ok;
    Widget_t *w_ti;
    bool run;
} MyWindow;

// draw a text input box
static void draw_text_input(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_push_group (w->cr);

    cairo_set_source_rgb (w->cr, 0.6, 0.6, 0.6);
    cairo_rectangle(w->cr,0,0,width,height);
    cairo_fill_preserve (w->cr);
    cairo_set_source_rgb (w->cr, 0.1, 0.1, 0.1);
    cairo_set_line_width(w->cr, 2.0);
    cairo_stroke(w->cr);

    cairo_set_font_size (w->cr, 9.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);

    cairo_move_to (w->cr, 2, 9);
    cairo_show_text(w->cr, " ");

    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
}

// draw add text to input box
static void text_input_add_text(void  *w_, void *label_) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    char *label = (char*)label_;
    if (!label) {
        label = "";
    }
    draw_text_input(w,NULL);
    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->cr, 0., 0.1, 0.1);
    cairo_set_font_size (w->cr, 11.0);
    if (strlen( w->input_label))
         w->input_label[strlen( w->input_label)-1] = 0;
    if (strlen( w->input_label)<30) {
        if (strlen(label))
        strcat( w->input_label, label);
    }
    strcat( w->input_label, "|");
    cairo_set_font_size (w->cr, 12.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->cr, w->input_label , &extents);

    cairo_move_to (w->cr, 2, 12.0+extents.height);
    cairo_show_text(w->cr,  w->input_label);

}

// draw remove last character from input box
static void text_input_clip(Widget_t *w) {
    draw_text_input(w,NULL);
    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->cr, 0., 0.1, 0.1);
    cairo_set_font_size (w->cr, 11.0);

    // check for UTF 8 char
    if (strlen( w->input_label)>=2) {
        int i = strlen( w->input_label)-1;
        int j = 0;
        int u = 0;
        for(;i>0;i--) {
            if(IS_UTF8(w->input_label[i])) {
                 u++;
            }
            j++;
            if (u == 1) break;
            if (j>2) break;
        }
        if (!u) j =2;

        memset(&w->input_label[strlen( w->input_label)-(sizeof(char)*(j))],0,sizeof(char)*(j));
        strcat( w->input_label, "|");
    }
    cairo_set_font_size (w->cr, 12.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->cr, w->input_label , &extents);

    cairo_move_to (w->cr, 2, 12.0+extents.height);
    cairo_show_text(w->cr,  w->input_label);

}

static void get_text(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XKeyEvent *key = (XKeyEvent*)key_;
    if (!key) return;
    int nk = key_mapping(w->dpy, key);
    if (nk) {
        switch (nk) {
            case 1: ;
            break;
            case 2: ;
            break;
            case 3: ;
            break;
            case 4: ;
            break;
            case 5: ;
            break;
            case 6: ;
            break;
            case 7: ;
            break;
            case 8: ;
            break;
            case 9: ;
            break;
            case 10: 
                {
                Widget_t *p = (Widget_t*)w->parent;
                quit(p);
                }
            break;
            case 11: text_input_clip(w);
            break;
            default:
            break;
        }
    } else {
        Status status;
        KeySym keysym;
        char buf[32];
        Xutf8LookupString(w->xic, key, buf, sizeof(buf) - 1, &keysym, &status);
        if(status == XLookupChars || status == XLookupBoth){
            text_input_add_text(w, buf);
        }
    }
}

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
        Widget_t *p = (Widget_t*)w->parent;
        quit(p);
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
    cairo_set_source_rgb (w->cr, 0.05, 0.15, 0.15);
    cairo_rectangle(w->cr,0,0,width,height);
    cairo_fill_preserve (w->cr);
    cairo_set_source_rgb (w->cr, 0.1, 0.1, 0.1);
    cairo_set_line_width(w->cr, 4.0);
    cairo_stroke(w->cr);

    cairo_text_extents_t extents;
    cairo_set_source_rgb (w->cr, 0.6, 0.6, 0.6);
    cairo_set_font_size (w->cr, 10.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->cr,w->label , &extents);

    cairo_move_to (w->cr, (width-extents.width)*0.5, (25-extents.height));
    cairo_show_text(w->cr, w->label);
   
    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    MyWindow mywindow;
    
    mywindow.w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 200);
    XStoreName(app.dpy, mywindow.w->widget, "Xputty Text Input");
    mywindow.w->label = "move pointer to text-input and press key:";
    mywindow.w->func.expose_callback = draw_window;

    mywindow.w_ti = create_widget(&app, mywindow.w, 20, 20, 260, 100);
    mywindow.w_ti->data = 1;
    memset(mywindow.w_ti->input_label, 0, 32 * (sizeof mywindow.w_ti->input_label[0]) );
    mywindow.w_ti->func.expose_callback = text_input_add_text;
   // mywindow.w_ti->func.enter_callback = draw_text_input;
    mywindow.w_ti->func.key_press_callback = get_text;
    mywindow.w_ti->scale.gravity = NORTHWEST;

    mywindow.w_ok = create_widget(&app, mywindow.w, 230, 170, 60, 20);
    mywindow.w_ok->label = "Ok";
    mywindow.w_ok->func.expose_callback = draw_button;
    mywindow.w_ok->func.enter_callback = draw_button;
    mywindow.w_ok->func.button_press_callback = button_press;
    mywindow.w_ok->func.button_release_callback = button_release;
    mywindow.w_ok->func.leave_callback = draw_button;
    mywindow.w_ok->scale.gravity = SOUTHWEST;

    widget_show_all(mywindow.w);
    mywindow.run = true;

    main_run(&app);

    main_quit(&app);

    return 0;
}
