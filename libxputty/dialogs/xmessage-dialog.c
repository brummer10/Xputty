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

#include "xmessage-dialog.h"

void draw_message_label(Widget_t *w, int width, int height) {
    MessageBox *mb = w->parent_struct;
    cairo_text_extents_t extents;
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->crb, 12.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    int i = 0;
    for(;i<mb->lin;i++) {
        cairo_text_extents(w->crb,mb->message[i] , &extents);

        cairo_move_to (w->crb, 100, ((40)+(extents.height * (2*i))));
        cairo_show_text(w->crb, mb->message[i]);
        cairo_new_path (w->crb);
    }
}

void draw_message_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    int height_t = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_rectangle(w->crb,0,0,width_t,height_t);
    set_pattern(w,&w->app->color_scheme->selected,&w->app->color_scheme->normal,BACKGROUND_);
    cairo_fill (w->crb);

    widget_set_scale(w);
    int width = cairo_xlib_surface_get_width(w->image);
    int height = cairo_xlib_surface_get_height(w->image);
    double x = 64.0/(double)(width);
    double y = 64.0/(double)height;
    double x1 = (double)height/64.0;
    double y1 = (double)(width)/64.0;
    cairo_scale(w->crb, x,y);
    cairo_set_source_surface (w->crb, w->image, 50, 50);
    cairo_rectangle(w->crb,50, 50, width, height);
    cairo_fill(w->crb);
    cairo_scale(w->crb, x1,y1);

    draw_message_label(w,width_t,height_t);
    widget_reset_scale(w);
}

void message_okay_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t *p = (Widget_t*)w->parent;
        MessageBox *mb = p->parent_struct;
        if(mb->message_type == QUESTION_BOX) {
            Widget_t *pa = (Widget_t*)p->parent;
            pa->func.user_callback(pa,&mb->response);
        }
        destroy_widget(p, p->app);
    }
}

void message_no_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t *p = (Widget_t*)w->parent;
        MessageBox *mb = p->parent_struct;
        if(mb->message_type == QUESTION_BOX) {
            Widget_t *pa = (Widget_t*)p->parent;
            mb->response = 1;
            pa->func.user_callback(pa,&mb->response);
        }
        destroy_widget(p, p->app);
    }
}

void mg_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    MessageBox *mb = w->parent_struct;
    int i = 0;
    for(;i<mb->lin;i++) {
        free(mb->message[i]);
    }
    free(mb->message);
    free(mb);
}

void check_for_lines(MessageBox *mb, const char *message) {
    int len = 0;
    char *ms =strdup(message);
    char * p = strtok (ms, "\n");
    while (p) {
        mb->message = realloc (mb->message, sizeof (char*) * ++mb->lin);
        mb->message[mb->lin-1] = malloc(sizeof(char)*strlen(p));
        strcpy(mb->message[mb->lin-1],p);
        len = max(len, strlen(mb->message[mb->lin-1]));
        p = strtok (NULL, "\n");
    }
    free(ms);
    mb->width = len*12 ;
    mb->height = mb->lin*12+100;
}

Widget_t *open_message_dialog(Widget_t *w, int style, const char *title, const char *message) {
    MessageBox *mb = (MessageBox*)malloc(sizeof(MessageBox));
    mb->response = 0;
    mb->message_type = 0;
    mb->lin = 0;
    mb->width = 1;
    mb->height = 1;
    mb->message = NULL;
    check_for_lines(mb, message);
    Widget_t *wid = create_window(w->app, DefaultRootWindow(w->app->dpy), 0, 0, mb->width, mb->height);
    wid->label = message;
    wid->flags |= HAS_MEM;
    wid->scale.gravity = CENTER;
    wid->parent_struct = mb;
    wid->parent = w;
    wid->func.mem_free_callback = mg_mem_free;
    wid->func.expose_callback = draw_message_window;
    char *alternate_title = NULL;
    char *button_title = "OK";
    switch (style) {
        case INFO_BOX: 
            widget_get_png(wid, LDVAR(info_png));
            alternate_title = "INFO";
            mb->message_type = INFO_BOX;
        break;
        case WARNING_BOX: 
            widget_get_png(wid, LDVAR(warning_png));
            alternate_title = "WARNING";
            mb->message_type = WARNING_BOX;
        break;
        case ERROR_BOX: 
            widget_get_png(wid, LDVAR(error_png));
            alternate_title = "ERROR";
            mb->message_type = ERROR_BOX;
        break;
        case QUESTION_BOX: 
            widget_get_png(wid, LDVAR(question_png));
            alternate_title = "QUESTION";
            Widget_t *no = add_button(wid, "NO", 10, mb->height-40, 60, 30);
            no->scale.gravity = CENTER;
            no->func.value_changed_callback = message_no_callback;
            mb->message_type = QUESTION_BOX;
            button_title = "YES";
        break;
    }
    XStoreName(w->app->dpy, wid->widget, strlen(title)? title : alternate_title);

    Widget_t *okay = add_button(wid, button_title, mb->width-70, mb->height-40, 60, 30);
    okay->scale.gravity = CENTER;
    okay->func.value_changed_callback = message_okay_callback;
    
    widget_show_all(wid);
    return wid;
}
