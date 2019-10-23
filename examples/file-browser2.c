/*
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
*/

#include <dirent.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdarg.h> 
#include <string.h>
#include <unistd.h>

#include <libgen.h>

#include "xwidgets.h"
#include "xdgmime.h"
#include "xasprintf.h"
#include "xfilepicker.h"

typedef struct {
    Widget_t *w;
    Widget_t *ct;
    Widget_t *ft;
    Widget_t *w_quit;
    Widget_t *w_okay;
    Widget_t *w_hidden;
    Widget_t *sel_dir;
    Widget_t *set_filter;
    FilePicker *fp;
    char *command;
} FileBrowser;


static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_pattern_t *pat;
    pat = cairo_pattern_create_linear (0.0, 0.0, width , height);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0.5, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 0.5, 1);
    cairo_rectangle(w->crb,0,0,width,height);
    cairo_set_source (w->crb, pat);
    cairo_fill (w->crb);
    cairo_pattern_destroy (pat);

    widget_set_scale(w);
    use_bg_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->crb, 12.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);

    cairo_move_to (w->crb, 20, 35);
    cairo_show_text(w->crb, "Directory");
    cairo_move_to (w->crb, 20, 85);
    cairo_show_text(w->crb, "File");
    cairo_move_to (w->crb, 20, 340);
    cairo_show_text(w->crb, "Load: ");
    cairo_move_to (w->crb, 45, 380);
    cairo_show_text(w->crb, "Show hidden files"); 
    cairo_move_to (w->crb, 60, 340);
    cairo_show_text(w->crb, w->label);
    widget_reset_scale(w);
}

static void button_quit_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && !adj_get_value(w->adj)){
        fprintf(stderr, "quit callback\n");
        quit(get_toplevel_widget(w->app));
    }
}

static inline int clear(Widget_t *w) {
    destroy_widget(w,w->app);
    return 0;
}

static int set_files(FileBrowser *filebrowser) {
    int ret = 0;
    int i = 0;
    for (; i<filebrowser->fp->n; i++) {
        listbox_add_entry(filebrowser->ft,filebrowser->fp->file_names[i]);
        if(filebrowser->fp->selected_file && strcmp(filebrowser->fp->file_names[i],
          basename(filebrowser->fp->selected_file))==0 )  ret = i;
    }
    return ret;
}

static void set_dirs(FileBrowser *filebrowser) {
    int i = 0;
    for (; i<filebrowser->fp->m; i++) {
        combobox_add_entry(filebrowser->ct,filebrowser->fp->dir_names[i]);
    }
}

static void center_widget(Widget_t *wid, Widget_t *w) {
    XMoveWindow(wid->app->dpy,w->widget,w->scale.init_x /
        wid->scale.cscale_x,w->scale.init_y / wid->scale.cscale_y);
    XResizeWindow (wid->app->dpy, w->widget, max(1,
        w->scale.init_width / (wid->scale.cscale_x)), 
        max(1,w->scale.init_height / (wid->scale.cscale_y)));
}

static void set_selected_file(FileBrowser *filebrowser) {
    Widget_t* view_port =  filebrowser->ft->childlist->childs[0];
    if(!childlist_has_child(view_port->childlist)) return ;
    Widget_t *file = view_port->childlist->childs[(int)adj_get_value(filebrowser->ft->adj)];
    Widget_t* menu =  filebrowser->ct->childlist->childs[1];
    view_port =  menu->childlist->childs[0];
    if(!childlist_has_child(view_port->childlist)) return ;
    Widget_t *dir = view_port->childlist->childs[(int)adj_get_value(filebrowser->ct->adj)];
    free(filebrowser->fp->selected_file);
    filebrowser->fp->selected_file = NULL;
    asprintf(&filebrowser->fp->selected_file, "%s/%s",dir->label, file->label);
    assert(filebrowser->fp->selected_file != NULL);
}

static void file_released_callback(void *w_, void* button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    set_selected_file(filebrowser);
    if(filebrowser->fp->selected_file) {
        fprintf(stderr, "file = %s\n",filebrowser->fp->selected_file);
        filebrowser->w->label = filebrowser->fp->selected_file;
        expose_widget(filebrowser->w);
    }
}

static void reload_file_entrys(FileBrowser *filebrowser) {
    clear(filebrowser->ft);
    fp_get_files(filebrowser->fp,filebrowser->fp->path, 0);
    filebrowser->ft = add_listbox(filebrowser->w, "", 20, 90, 620, 225);
    filebrowser->ft->parent_struct = filebrowser;
    filebrowser->ft->func.button_release_callback = file_released_callback;
    int set_f = set_files(filebrowser);
    center_widget(filebrowser->w,filebrowser->ft);
    listbox_set_active_entry(filebrowser->ft, set_f);
    widget_show_all(filebrowser->w);
}

static void combo_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    Widget_t* menu =  w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    if(!childlist_has_child(view_port->childlist)) return ;
    Widget_t *entry = view_port->childlist->childs[(int)adj_get_value(w->adj)];
    free(filebrowser->fp->path);
    filebrowser->fp->path = NULL;
    asprintf(&filebrowser->fp->path, "%s",entry->label);
    assert(filebrowser->fp->path != NULL);
    reload_file_entrys(filebrowser);
}

static void button_ok_callback(void *w_, void* user_data) {
   Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        if(filebrowser->fp->selected_file) {
            fprintf(stderr, "selected = %s\n",filebrowser->fp->selected_file);
        } else {
            set_selected_file(filebrowser);
            if(filebrowser->fp->selected_file)
                fprintf(stderr, "selected = %s\n",filebrowser->fp->selected_file);
        }
        asprintf(&filebrowser->command, "xdg-open '%s'",filebrowser->fp->selected_file);
        if (system(NULL)) system(filebrowser->command);
        free(filebrowser->command);
        filebrowser->command = NULL;
        //quit(get_toplevel_widget(w->app));
   }
}

static void reload_all(FileBrowser *filebrowser) {
    Widget_t* menu =  filebrowser->ct->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    if(!childlist_has_child(view_port->childlist)) return ;
    Widget_t *entry = view_port->childlist->childs[(int)adj_get_value(filebrowser->ct->adj)];
    free(filebrowser->fp->path);
    filebrowser->fp->path = NULL;
    asprintf(&filebrowser->fp->path, "%s",entry->label);
    assert(filebrowser->fp->path != NULL);
    clear(filebrowser->ft);
    clear(filebrowser->ct);
    int ds = fp_get_files(filebrowser->fp,filebrowser->fp->path, 1);
    filebrowser->ct = add_combobox(filebrowser->w, "", 20, 40, 580, 30);
    center_widget(filebrowser->w,filebrowser->ct);
    filebrowser->ct->parent_struct = filebrowser;
    filebrowser->ct->func.value_changed_callback = combo_response;
    filebrowser->ft = add_listbox(filebrowser->w, "", 20, 90, 620, 225);
    filebrowser->ft->parent_struct = filebrowser;
    filebrowser->ft->func.button_release_callback = file_released_callback;
    int set_f = set_files(filebrowser);
    center_widget(filebrowser->w,filebrowser->ft);
    set_dirs(filebrowser);
    combobox_set_active_entry(filebrowser->ct, ds);
    listbox_set_active_entry(filebrowser->ft, set_f);
    widget_show_all(filebrowser->w);
}

static void open_dir_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        reload_all(filebrowser);
    }
}

static void button_hidden_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (w->flags & HAS_POINTER) {
        filebrowser->fp->show_hidden = adj_get_value(w->adj) ? true : false;
        reload_all(filebrowser);
    }
}

static void set_filter_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (filebrowser->fp->use_filter != (int)adj_get_value(w->adj)) {
        filebrowser->fp->use_filter = (int)adj_get_value(w->adj);
        Widget_t* menu =  w->childlist->childs[1];
        Widget_t* view_port =  menu->childlist->childs[0];
        if(!childlist_has_child(view_port->childlist)) return ;
        Widget_t *entry = view_port->childlist->childs[(int)adj_get_value(w->adj)];
        free(filebrowser->fp->filter);
        filebrowser->fp->filter = NULL;
        asprintf(&filebrowser->fp->filter, "%s",entry->label);
        assert(filebrowser->fp->filter != NULL);

        reload_file_entrys(filebrowser);
    }
}

static void mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    free(filebrowser->command);
    fp_free(filebrowser->fp);
    free(filebrowser->fp);
}


int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    FileBrowser filebrowser;
    filebrowser.fp = (FilePicker*)malloc(sizeof(FilePicker));
    fp_init(filebrowser.fp, getenv("HOME") ? getenv("HOME") : "/");
    filebrowser.command = NULL;

    filebrowser.w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 660, 420);
    filebrowser.w->flags |= HAS_MEM;
    filebrowser.w->parent_struct = &filebrowser;
    widget_set_title(filebrowser.w, "File Selector");
    filebrowser.w->func.expose_callback = draw_window;
    filebrowser.w->func.mem_free_callback = mem_free;

    filebrowser.ct = add_combobox(filebrowser.w, "", 20, 40, 580, 30);
    filebrowser.ct->parent_struct = &filebrowser;
    filebrowser.ct->func.value_changed_callback = combo_response;

    filebrowser.sel_dir = add_button(filebrowser.w, "\u23CE", 610, 40, 30, 30);
    filebrowser.sel_dir->parent_struct = &filebrowser;
    filebrowser.sel_dir->scale.gravity = CENTER;
    add_tooltip(filebrowser.sel_dir,"Open sub-directory's");
    filebrowser.sel_dir->func.value_changed_callback = open_dir_callback;

    filebrowser.ft = add_listbox(filebrowser.w, "", 20, 90, 620, 225);
    filebrowser.ft->parent_struct = &filebrowser;
    filebrowser.ft->func.button_release_callback = file_released_callback;

    int ds = fp_get_files(filebrowser.fp,filebrowser.fp->path, 1);   
    int set_f = set_files(&filebrowser); 
    set_dirs(&filebrowser);
    combobox_set_active_entry(filebrowser.ct, ds);
    listbox_set_active_entry(filebrowser.ft, set_f);

    filebrowser.w_quit = add_button(filebrowser.w, "\u2620", 580, 350, 60, 60);
    filebrowser.w_quit->parent_struct = &filebrowser;
    filebrowser.w_quit->scale.gravity = CENTER;
    add_tooltip(filebrowser.w_quit,"Exit file selector");
    filebrowser.w_quit->func.value_changed_callback = button_quit_callback;

    filebrowser.w_okay = add_button(filebrowser.w, "\u2713", 510, 350, 60, 60);
    filebrowser.w_okay->parent_struct = &filebrowser;
    filebrowser.w_okay->scale.gravity = CENTER;
    add_tooltip(filebrowser.w_okay,"Load selected file");
    filebrowser.w_okay->func.value_changed_callback = button_ok_callback;

    filebrowser.set_filter = add_combobox(filebrowser.w, "", 360, 355, 120, 30);
    filebrowser.set_filter->parent_struct = &filebrowser;
    combobox_add_entry(filebrowser.set_filter,"all");
    combobox_add_entry(filebrowser.set_filter,"application");
    combobox_add_entry(filebrowser.set_filter,"audio");
    combobox_add_entry(filebrowser.set_filter,"font");
    combobox_add_entry(filebrowser.set_filter,"image");
    combobox_add_entry(filebrowser.set_filter,"text");
    combobox_add_entry(filebrowser.set_filter,"video");
    combobox_add_entry(filebrowser.set_filter,"x-content");
    combobox_set_active_entry(filebrowser.set_filter, 0);
    filebrowser.set_filter->func.value_changed_callback = set_filter_callback;
    add_tooltip(filebrowser.set_filter->childlist->childs[0], "File filter type");

    filebrowser.w_hidden = add_check_button(filebrowser.w, "", 20, 365, 20, 20);
    filebrowser.w_hidden->parent_struct = &filebrowser;
    filebrowser.w_hidden->scale.gravity = CENTER;
    add_tooltip(filebrowser.w_hidden,"Show hidden files and folders");
    filebrowser.w_hidden->func.value_changed_callback = button_hidden_callback;

    widget_show_all(filebrowser.w);

    main_run(&app);

    main_quit(&app);

    return 0;
}
