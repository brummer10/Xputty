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
#include "xmessage-dialog.h"

typedef struct {
    Widget_t *w;
    Widget_t *w_quit;
    Widget_t *filebutton;
    Widget_t *messagebutton;
    Pixmap *icon;
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
        free(xde->filename);
        xde->filename = NULL;
        xde->filename = strdup(*(const char**)user_data);
        p->label = xde->filename;
        expose_widget(p);
    } else {
        fprintf(stderr, "no file selected\n");
    }
}

static void mbutton_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        open_message_dialog(w, INFO_BOX, "", "This is a longer info message\n"
        "now with double line test\nand now with 3 lines\nand one more", NULL);
    }
    adj_set_value(w->adj,0.0);
}

static void wbutton_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        open_message_dialog(w, WARNING_BOX, "", "This is a warning message", NULL);
    }
    adj_set_value(w->adj,0.0);
}

static void ebutton_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        open_message_dialog(w, ERROR_BOX, "", "This is a error message", NULL);
    }
    adj_set_value(w->adj,0.0);
}

static void qbutton_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        open_message_dialog(w, QUESTION_BOX, "", "This is a question message\n"
            "Do you like dog's?", NULL);
    }
    adj_set_value(w->adj,0.0);
}

static void qbutton_response(void *w_, void* user_data) {
    if(user_data !=NULL) {
        int response = *(int*)user_data;
        if(response <0)
            fprintf(stderr, "You select NO\n");
        else if(response == 0)
            fprintf(stderr, "You select Yes\n");
    }
}

static void cbutton_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        open_message_dialog(w, SELECTION_BOX, "", "This is a selection question\n"
            "Please make your choice\nWhat do you like most:", "Dogs\nCats\nFish\nBirds");
    }
    adj_set_value(w->adj,0.0);
}

static void cbutton_response(void *w_, void* user_data) {
    if(user_data !=NULL) {
        int response = *(int*)user_data;
        if(response>0)
            fprintf(stderr, "You select %i\n", response);
    }
}

static void tbutton_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        open_message_dialog(w, ENTRY_BOX, "", "This is a text entry\n"
            "Please enter a name:", NULL);
    }
    adj_set_value(w->adj,0.0);
}

static void tbutton_response(void *w_, void* user_data) {
    if(user_data !=NULL && strlen(*(const char**)user_data)) {
        fprintf(stderr, "You enter %s\n", *(const char**)user_data);
    } else {
        fprintf(stderr, "You enter nothing!\n");
    }
}


void mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XDialogExample *xde = w->parent_struct;
    if(xde->icon) {
        XFreePixmap(w->app->dpy, (*xde->icon));
        xde->icon = NULL;
    }
    free(xde->filename);
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    XDialogExample xde;

    xde.filename = NULL;
    xde.icon = NULL;
    xde.w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 660, 150);
    widget_set_title(xde.w, "Xputty Dialog examples");
    widget_get_png(xde.w, LDVAR(xputty_logo_png));
    widget_set_icon_from_surface(xde.w,xde.icon,xde.w->image);
    xde.w->label = "..";
    xde.w->parent_struct = &xde;
    xde.w->func.expose_callback = draw_window;
    xde.w->flags |= HAS_MEM;
    xde.w->func.mem_free_callback = mem_free;

    xde.w_quit = add_button(xde.w, "Quit", 580, 80, 60, 60);
    xde.w_quit->scale.gravity = CENTER;
    xde.w_quit->func.value_changed_callback = button_quit_callback;

    xde.filebutton = add_file_button(xde.w, 20, 80, 60, 60, getenv("HOME") ? getenv("HOME") : "/", "");
    xde.filebutton->func.user_callback = dialog_response;

    xde.messagebutton = add_image_toggle_button(xde.w, "", 100, 80, 60, 60);
    widget_get_png(xde.messagebutton, LDVAR(info_png));
    xde.messagebutton->func.value_changed_callback = mbutton_callback;

    xde.messagebutton = add_image_toggle_button(xde.w, "", 180, 80, 60, 60);
    widget_get_png(xde.messagebutton, LDVAR(warning_png));
    xde.messagebutton->func.value_changed_callback = wbutton_callback;

    xde.messagebutton = add_image_toggle_button(xde.w, "", 260, 80, 60, 60);
    widget_get_png(xde.messagebutton, LDVAR(error_png));
    xde.messagebutton->func.value_changed_callback = ebutton_callback;

    xde.messagebutton = add_image_toggle_button(xde.w, "", 340, 80, 60, 60);
    widget_get_png(xde.messagebutton, LDVAR(question_png));
    xde.messagebutton->func.value_changed_callback = qbutton_callback;
    xde.messagebutton->func.user_callback = qbutton_response;

    xde.messagebutton = add_image_toggle_button(xde.w, "", 420, 80, 60, 60);
    widget_get_png(xde.messagebutton, LDVAR(choice_png));
    xde.messagebutton->func.value_changed_callback = cbutton_callback;
    xde.messagebutton->func.user_callback = cbutton_response;

    xde.messagebutton = add_image_toggle_button(xde.w, "", 500, 80, 60, 60);
    widget_get_png(xde.messagebutton, LDVAR(message_png));
    xde.messagebutton->func.value_changed_callback = tbutton_callback;
    xde.messagebutton->func.user_callback = tbutton_response;

   widget_show_all(xde.w);

    main_run(&app);

    main_quit(&app);

    return 0;
}
