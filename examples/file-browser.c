#include <dirent.h> 
#include <stdio.h> 
#include <string.h>
#include <unistd.h>

#include <libgen.h>

#include "xwidgets.h"

typedef struct {
    Widget_t *w;
    Widget_t *ct;
    Widget_t *ft;
    Widget_t *w_quit;
    Widget_t *w_okay;
    Widget_t *sel_dir;
    Widget_t *sel_file;
    unsigned n;
    unsigned m;
    char path[4096];
    char selected_file[4096];
    char filenames[255][255]; //for simplicity in this example
    char dirnames[255][255]; //for simplicity in this example
    
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
    use_bg_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, 12.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);

    cairo_move_to (w->crb, 20, 140);
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

static int clear(Widget_t *w) {
    destroy_widget(w,w->app);
    return 0;
}

static int compare_fun (const void *p1, const void *p2) {
    return strcmp((const char*) p1, (const char*) p2);
}

static void get_files(FileBrowser *filebrowser, char *path, int get_dirs) {
    int i = 0;
    for (; i<255;i++) {
        sprintf(filebrowser->filenames[i], '\0');
        if(get_dirs) sprintf(filebrowser->dirnames[i], '\0');
    }
    filebrowser->n=0;
    filebrowser->m=0;
    DIR *dirp;
    struct dirent *dp;
    if(get_dirs) {
        char ho[4096];
        sprintf(ho, "%s",path);

        sprintf(filebrowser->dirnames[filebrowser->m++], "%s",dirname(ho));
        if (strcmp (path, "/") != 0)
            sprintf(filebrowser->dirnames[filebrowser->m++], "%s",path);
    }

    dirp = opendir(path);
    while ((dp = readdir(dirp)) != NULL && 
      filebrowser->n < sizeof filebrowser->filenames / sizeof filebrowser->filenames[0] &&
      filebrowser->m < sizeof filebrowser->dirnames / sizeof filebrowser->dirnames[0]) {
        if(dp-> d_type != DT_DIR && strlen(dp->d_name)!=0 &&
          (dp->d_name[0] != '.')  && strcmp(dp->d_name,"..")!=0) {
            sprintf(filebrowser->filenames[filebrowser->n++], "%s",dp->d_name);
        } else if(get_dirs && dp -> d_type == DT_DIR && (dp->d_name[0] != '.')
          && strcmp(dp->d_name,"..")!=0 && strlen(dp->d_name)!=0 ) {
            if (strcmp (path, "/") != 0) {
                sprintf(filebrowser->dirnames[filebrowser->m++], "%s/%s",path,dp->d_name);
            } else {
                sprintf(filebrowser->dirnames[filebrowser->m++], "%s%s",path,dp->d_name);
            }
        }
    }
    closedir(dirp);
   
    if (filebrowser->m>1 && get_dirs)
        qsort (filebrowser->dirnames, filebrowser->m, sizeof filebrowser->dirnames[0], compare_fun);
    if (filebrowser->n>1)
        qsort (filebrowser->filenames, filebrowser->n, sizeof filebrowser->filenames[0], compare_fun);
}

static void set_files(FileBrowser *filebrowser) {
    set_adjustment(filebrowser->ft->adj_y,0.0, 0.0, 0.0, -1.0,1.0, CL_ENUM);
    filebrowser->ft->adj = filebrowser->ft->adj_y;
    adj_set_value(filebrowser->ft->adj, 0.0);
    int i = 0;
    for (; i<filebrowser->n; i++) {
        combobox_add_entry(filebrowser->ft,filebrowser->filenames[i]);
    }

}

static void set_dirs(FileBrowser *filebrowser) {
    set_adjustment(filebrowser->ct->adj_y,0.0, 0.0, 0.0, -1.0,1.0, CL_ENUM);
    filebrowser->ct->adj = filebrowser->ct->adj_y;
    adj_set_value(filebrowser->ct->adj, 0.0);
    int i = 0;
    for (; i<filebrowser->m; i++) {
        combobox_add_entry(filebrowser->ct,filebrowser->dirnames[i]);
    }
}

static void center_widget(Widget_t *wid, Widget_t *w) {
    XMoveWindow(wid->app->dpy,w->widget,w->scale.init_x /
        wid->scale.cscale_x,w->scale.init_y / wid->scale.cscale_y);
    XResizeWindow (wid->app->dpy, w->widget, max(1,
        w->scale.init_width / (wid->scale.cscale_x)), 
        max(1,w->scale.init_height / (wid->scale.cscale_y)));
}

static void combo_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    Widget_t* menu =  w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    if(!childlist_has_child(view_port->childlist)) return ;
    Widget_t *entry = view_port->childlist->childs[(int)adj_get_value(w->adj)];
    sprintf(filebrowser->path, "%s",entry->label);
    clear(filebrowser->ft);
    get_files(filebrowser,filebrowser->path, 0);
    filebrowser->ft = add_combobox(filebrowser->w, "", 20, 90, 560, 30);
    center_widget(filebrowser->w,filebrowser->ft);
    filebrowser->ft->parent_struct = filebrowser;
    set_files(filebrowser);
    combobox_set_active_entry(filebrowser->ft, 0);
    widget_show_all(filebrowser->w);
}

static void button_ok_callback(void *w_, void* user_data) {
   Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t* menu =  filebrowser->ft->childlist->childs[1];
        Widget_t* view_port =  menu->childlist->childs[0];
        if(!childlist_has_child(view_port->childlist)) return ;
        Widget_t *file = view_port->childlist->childs[(int)adj_get_value(filebrowser->ft->adj)];
        menu =  filebrowser->ct->childlist->childs[1];
        view_port =  menu->childlist->childs[0];
        if(!childlist_has_child(view_port->childlist)) return ;
        Widget_t *dir = view_port->childlist->childs[(int)adj_get_value(filebrowser->ct->adj)];
        fprintf(stderr, "selected = %s\n",filebrowser->selected_file);
        quit(get_toplevel_widget(w->app));
   }
}

static void open_dir_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t* menu =  filebrowser->ct->childlist->childs[1];
        Widget_t* view_port =  menu->childlist->childs[0];
        if(!childlist_has_child(view_port->childlist)) return ;
        Widget_t *entry = view_port->childlist->childs[(int)adj_get_value(filebrowser->ct->adj)];
        sprintf(filebrowser->path, "%s",entry->label);
        fprintf(stderr, "path = %s\n",filebrowser->path);
        clear(filebrowser->ft);
        clear(filebrowser->ct);
        get_files(filebrowser,filebrowser->path, 1);
        filebrowser->ct = add_combobox(filebrowser->w, "", 20, 40, 560, 30);
        center_widget(filebrowser->w,filebrowser->ct);
        filebrowser->ct->parent_struct = filebrowser;
        filebrowser->ct->func.value_changed_callback = combo_response;
        filebrowser->ft = add_combobox(filebrowser->w, "", 20, 90, 560, 30);
        center_widget(filebrowser->w,filebrowser->ft);
        filebrowser->ft->parent_struct = filebrowser;
        set_files(filebrowser);
        set_dirs(filebrowser);
        widget_show_all(filebrowser->w);
        if (strcmp (filebrowser->path, "/") != 0)
            combobox_set_active_entry(filebrowser->ct, 1);
        else
            combobox_set_active_entry(filebrowser->ct, 0);
        combobox_set_active_entry(filebrowser->ft, 0);
    }

}

static void open_file_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t* menu =  filebrowser->ft->childlist->childs[1];
        Widget_t* view_port =  menu->childlist->childs[0];
        if(!childlist_has_child(view_port->childlist)) return ;
        Widget_t *file = view_port->childlist->childs[(int)adj_get_value(filebrowser->ft->adj)];
        menu =  filebrowser->ct->childlist->childs[1];
        view_port =  menu->childlist->childs[0];
        if(!childlist_has_child(view_port->childlist)) return ;
        Widget_t *dir = view_port->childlist->childs[(int)adj_get_value(filebrowser->ct->adj)];
        sprintf(filebrowser->selected_file, "%s/%s",dir->label, file->label);
        fprintf(stderr, "file = %s\n",filebrowser->selected_file);
        filebrowser->w->label = filebrowser->selected_file;
        expose_widget(filebrowser->w);
    }
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    FileBrowser filebrowser;
    filebrowser.n=0;
    filebrowser.m=0;
    sprintf(filebrowser.path, "%s",getenv("HOME"));
    
    filebrowser.w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 660, 200);
    filebrowser.w->parent_struct = &filebrowser;
    XStoreName(app.dpy, filebrowser.w->widget, "File Selector");
    filebrowser.w->func.expose_callback = draw_window;
    filebrowser.ct = add_combobox(filebrowser.w, "", 20, 40, 560, 30);
    filebrowser.ct->parent_struct = &filebrowser;
    filebrowser.ct->func.value_changed_callback = combo_response;

    filebrowser.sel_dir = add_button(filebrowser.w, "Open", 590, 40, 60, 30);
    filebrowser.sel_dir->parent_struct = &filebrowser;
    filebrowser.sel_dir->scale.gravity = CENTER;
    add_tooltip(filebrowser.sel_dir,"Open selected directory");
    filebrowser.sel_dir->func.value_changed_callback = open_dir_callback;

    filebrowser.ft = add_combobox(filebrowser.w, "", 20, 90, 560, 30);
    filebrowser.ft->parent_struct = &filebrowser;

    filebrowser.sel_file = add_button(filebrowser.w, "Set", 590, 90, 60, 30);
    filebrowser.sel_file->parent_struct = &filebrowser;
    filebrowser.sel_file->scale.gravity = CENTER;
    add_tooltip(filebrowser.sel_file,"Set select file");
    filebrowser.sel_file->func.value_changed_callback = open_file_callback;

    get_files(&filebrowser,filebrowser.path, 1);   
    set_files(&filebrowser); 
    set_dirs(&filebrowser);
    combobox_set_active_entry(filebrowser.ct, 1);
    combobox_set_active_entry(filebrowser.ft, 0);

    filebrowser.w_quit = add_button(filebrowser.w, "Quit", 590, 160, 60, 30);
    filebrowser.w_quit->parent_struct = &filebrowser;
    filebrowser.w_quit->scale.gravity = CENTER;
    add_tooltip(filebrowser.w_quit,"Exit file selector");
    filebrowser.w_quit->func.value_changed_callback = button_quit_callback;

    filebrowser.w_okay = add_button(filebrowser.w, "OK", 520, 160, 60, 30);
    filebrowser.w_okay->parent_struct = &filebrowser;
    filebrowser.w_okay->scale.gravity = CENTER;
    add_tooltip(filebrowser.w_okay,"Print selected file and exit");
    filebrowser.w_okay->func.value_changed_callback = button_ok_callback;
    widget_show_all(filebrowser.w);
   
    main_run(&app);
   
    main_quit(&app);

    return 0;
    
    
}
