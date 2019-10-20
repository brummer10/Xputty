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
#include "xfile-dialog.h"

typedef struct {
    Widget_t *w;
    Widget_t *w_quit;
    Widget_t *filebutton;
    char *filename;
} XDialogExample;

static void draw_label(Widget_t *w, int width, int height) {
    cairo_text_extents_t extents;
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->crb, 12.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, (width-extents.width)*0.5, (height+extents.height)*0.2);
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

    cairo_rectangle(w->crb,0,0,width,height);
    set_pattern(w,&w->app->color_scheme->selected,&w->app->color_scheme->normal,BACKGROUND_);
    cairo_fill (w->crb);
    draw_label(w,width,height);
}

static void button_quit_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t *p = (Widget_t*)w->parent;
        quit(p);
    }
}

static void dialog_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    XDialogExample *xde = p->parent_struct;
    if(user_data !=NULL) {
        fprintf(stderr, "selected file %s\n", *(const char**)user_data);
        xde->filename = strdup(*(const char**)user_data);
        p->label = xde->filename;
        expose_widget(p);
    } else {
        fprintf(stderr, "no file selected\n");
    }
}

void mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XDialogExample *xde = w->parent_struct;
    free(xde->filename);
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    XDialogExample xde;

    xde.filename = NULL;
    xde.w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 500, 150);
    XStoreName(app.dpy, xde.w->widget, "Xputty File Dialog example");
    xde.w->label = "..";
    xde.w->parent_struct = &xde;
    xde.w->func.expose_callback = draw_window;
    xde.w->flags |= HAS_MEM;
    xde.w->func.mem_free_callback = mem_free;

    xde.w_quit = add_button(xde.w, "Quit", 420, 80, 60, 60);
    xde.w_quit->scale.gravity = CENTER;
    xde.w_quit->func.value_changed_callback = button_quit_callback;

    xde.filebutton = open_file_button(xde.w, 20, 80, 60, 60, getenv("HOME"), "");
    xde.filebutton->func.user_callback = dialog_response;

    widget_show_all(xde.w);
   
    main_run(&app);
   
    main_quit(&app);

    return 0;
    
    
}
