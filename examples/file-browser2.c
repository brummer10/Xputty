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

/* asprintf implement from:
    https://stackoverflow.com/questions/40159892/using-asprintf-on-windows
    define _GNU_SOURCE to use the GNU asprintf extension instead this one.
*/
#ifndef _GNU_SOURCE
#ifndef _vscprintf 
int _vscprintf_so(const char * format, va_list pargs) {
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}
#endif // _vscprintf

#ifndef vasprintf
int vasprintf(char **strp, const char *fmt, va_list ap) {
    int len = _vscprintf_so(fmt, ap);
    if (len == -1) return -1;
    char *str = malloc((size_t) len + 1);
    if (!str) return -1;
    int r = vsnprintf(str, len + 1, fmt, ap);
    if (r == -1) return free(str), -1;
    *strp = str;
    return r;
}
#endif // vasprintf

#ifndef asprintf
int asprintf(char *strp[], const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vasprintf(strp, fmt, ap);
    va_end(ap);
    return r;
}
#endif // asprintf
#endif // _GNU_SOURCE

typedef struct {
    Widget_t *w;
    Widget_t *ct;
    Widget_t *ft;
    Widget_t *w_quit;
    Widget_t *w_okay;
    Widget_t *w_hidden;
    Widget_t *sel_dir;
    Widget_t *set_filter;
    int use_filter;
    bool show_hidden;
    unsigned int n;
    unsigned int m;
    char *path;
    char *command;
    char *selected_file;
    char **file_names;
    char **dir_names;
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

static inline int compare_fun (const void *p1, const void *p2) {
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline int compare_hidden_dirs_fun (const void *p1, const void *p2) {
    if(strstr(*(const char**)p1, "/.") && strstr(*(const char**)p2, "/.")) return 0;
    if(strstr(*(const char**)p1, "/.")) return 1;
    if(strstr(*(const char**)p2, "/.")) return -1;
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline int compare_hidden_files_fun (const void *p1, const void *p2) {
    if(strncmp(*(const char**)p1, ".",1)==0 && strncmp(*(const char**)p2, ".",1)==0) return 0;
    if(strncmp(*(const char**)p1, ".",1)==0 ) return 1;
    if(strncmp(*(const char**)p2, ".",1)==0 ) return -1;
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline bool show_hidden_files(FileBrowser *filebrowser, char* file) {
    return (filebrowser->show_hidden) ? strcmp(file,".")!=0 : (file[0] != '.');
}

static inline bool show_filter_files(FileBrowser *filebrowser, char* file) {
    if (!filebrowser->use_filter) {
        return true;
    } else {
        Widget_t* menu =  filebrowser->set_filter->childlist->childs[1];
        Widget_t* view_port =  menu->childlist->childs[0];

        return strstr(xdg_mime_get_mime_type_from_file_name(file),
          view_port->childlist->childs[filebrowser->use_filter]->label);
    }
}

static inline void sort_buffers(FileBrowser *filebrowser, int get_dirs) {
    if (filebrowser->m>1 && get_dirs) {
        qsort (filebrowser->dir_names, filebrowser->m,
          sizeof filebrowser->dir_names[0], (filebrowser->show_hidden) ?
          compare_hidden_dirs_fun : compare_fun);
    }
    if (filebrowser->n>1) {
        qsort (filebrowser->file_names, filebrowser->n,
          sizeof filebrowser->file_names[0], (filebrowser->show_hidden) ?
          compare_hidden_files_fun : compare_fun);
    }
}

static inline void clear_filebuffer(FileBrowser *filebrowser) {
    int j = 0;
    for(; j<filebrowser->n;j++) {
        free(filebrowser->file_names[j]);
        filebrowser->file_names[j] = NULL;
    }
    if(filebrowser->n) {
        free(filebrowser->file_names);
        filebrowser->file_names = NULL;
        filebrowser->n=0;
    }
}

static inline void clear_dirbuffer(FileBrowser *filebrowser) {
    int j = 0;
    for(; j<filebrowser->m;j++) {
        free(filebrowser->dir_names[j]);
        filebrowser->dir_names[j] = NULL;
    }
    if(filebrowser->m) {
        free(filebrowser->dir_names);
        filebrowser->dir_names = NULL;
        filebrowser->m=0;
    }
}

static inline int prefill_dirbuffer(FileBrowser *filebrowser, char *path) {
    int ret = 0;
    if (strcmp (path, "/") == 0) {
        filebrowser->dir_names = (char **)realloc(filebrowser->dir_names,
          (filebrowser->m + 1) * sizeof(char *));
        assert(filebrowser->dir_names != NULL);
        asprintf(&filebrowser->dir_names[filebrowser->m++], "%s",path);
        assert(&filebrowser->dir_names[filebrowser->m] != NULL);
    } else {
        char *ho;
        asprintf(&ho, "%s",path);
        assert(ho != NULL);
        while (strcmp (ho, "/") != 0) {
            filebrowser->dir_names = (char **)realloc(filebrowser->dir_names,
              (filebrowser->m + 1) * sizeof(char *));
            assert(filebrowser->dir_names != NULL);
            asprintf(&filebrowser->dir_names[filebrowser->m++], "%s",dirname(ho));
            assert(&filebrowser->dir_names[filebrowser->m] != NULL);
            ret++;
        }
        if (strcmp (path, "/") != 0) {
            filebrowser->dir_names = (char **)realloc(filebrowser->dir_names,
              (filebrowser->m + 1) * sizeof(char *));
            assert(filebrowser->dir_names != NULL);
            asprintf(&filebrowser->dir_names[filebrowser->m++], "%s",path);
            assert(&filebrowser->dir_names[filebrowser->m] != NULL);
        }
        free(ho);
    }
    return ret;   
}

static int get_files(FileBrowser *filebrowser, char *path, int get_dirs) {
    int ret = 0;
    clear_filebuffer(filebrowser);
    if(get_dirs) {
        clear_dirbuffer(filebrowser);
        ret = prefill_dirbuffer(filebrowser, path);
    }

    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(path);
    while ((dp = readdir(dirp)) != NULL) {

        if(dp-> d_type != DT_DIR && strlen(dp->d_name)!=0 && dp->d_type != DT_UNKNOWN
          && strcmp(dp->d_name,"..")!=0 && show_hidden_files(filebrowser, dp->d_name) &&
          show_filter_files(filebrowser, dp->d_name)) {

            filebrowser->file_names = (char **)realloc(filebrowser->file_names,
              (filebrowser->n + 1) * sizeof(char *));
            assert(filebrowser->file_names != NULL);
            asprintf(&filebrowser->file_names[filebrowser->n++],"%s",dp->d_name);
            assert(&filebrowser->file_names[filebrowser->n] != NULL);

        } else if(get_dirs && dp -> d_type == DT_DIR && strlen(dp->d_name)!=0
          && strcmp(dp->d_name,"..")!=0 && show_hidden_files(filebrowser, dp->d_name)) {

            filebrowser->dir_names = (char **)realloc(filebrowser->dir_names,
              (filebrowser->m + 1) * sizeof(char *));
            assert(filebrowser->dir_names != NULL);
            asprintf(&filebrowser->dir_names[filebrowser->m++], (strcmp(path, "/") != 0) ?
              "%s/%s" : "%s%s" , path,dp->d_name);
            assert(&filebrowser->dir_names[filebrowser->m] != NULL);
        }
    }
    closedir(dirp);
    sort_buffers(filebrowser, get_dirs);
    return ret;
}

static void set_files(FileBrowser *filebrowser) {
    set_adjustment(filebrowser->ft->adj_y,0.0, 0.0, 0.0, -1.0,1.0, CL_ENUM);
    filebrowser->ft->adj = filebrowser->ft->adj_y;
    adj_set_value(filebrowser->ft->adj, 0.0);
    int i = 0;
    for (; i<filebrowser->n; i++) {
        listbox_add_entry(filebrowser->ft,filebrowser->file_names[i]);
    }
}

static void set_dirs(FileBrowser *filebrowser) {
    set_adjustment(filebrowser->ct->adj_y,0.0, 0.0, 0.0, -1.0,1.0, CL_ENUM);
    filebrowser->ct->adj = filebrowser->ct->adj_y;
    adj_set_value(filebrowser->ct->adj, 0.0);
    int i = 0;
    for (; i<filebrowser->m; i++) {
        combobox_add_entry(filebrowser->ct,filebrowser->dir_names[i]);
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
    free(filebrowser->selected_file);
    filebrowser->selected_file = NULL;
    asprintf(&filebrowser->selected_file, "%s/%s",dir->label, file->label);
    assert(filebrowser->selected_file != NULL);
}

static void file_released_callback(void *w_, void* button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    set_selected_file(filebrowser);
    if(filebrowser->selected_file) {
        fprintf(stderr, "file = %s\n",filebrowser->selected_file);
        filebrowser->w->label = filebrowser->selected_file;
        expose_widget(filebrowser->w);
    }
}

static void reload_file_entrys(FileBrowser *filebrowser) {
    clear(filebrowser->ft);
    get_files(filebrowser,filebrowser->path, 0);
    filebrowser->ft = add_listbox(filebrowser->w, "", 20, 90, 620, 225);
    filebrowser->ft->parent_struct = filebrowser;
    filebrowser->ft->func.button_release_callback = file_released_callback;
    set_files(filebrowser);
    center_widget(filebrowser->w,filebrowser->ft);
    widget_show_all(filebrowser->w);
    listbox_set_active_entry(filebrowser->ft, 0);
}

static void combo_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    Widget_t* menu =  w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    if(!childlist_has_child(view_port->childlist)) return ;
    Widget_t *entry = view_port->childlist->childs[(int)adj_get_value(w->adj)];
    free(filebrowser->path);
    filebrowser->path = NULL;
    asprintf(&filebrowser->path, "%s",entry->label);
    assert(filebrowser->path != NULL);
    reload_file_entrys(filebrowser);
}

static void button_ok_callback(void *w_, void* user_data) {
   Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        if(filebrowser->selected_file) {
            fprintf(stderr, "selected = %s\n",filebrowser->selected_file);
        } else {
            set_selected_file(filebrowser);
            if(filebrowser->selected_file)
                fprintf(stderr, "selected = %s\n",filebrowser->selected_file);
        }
        asprintf(&filebrowser->command, "xdg-open '%s'",filebrowser->selected_file);
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
    free(filebrowser->path);
    filebrowser->path = NULL;
    asprintf(&filebrowser->path, "%s",entry->label);
    assert(filebrowser->path != NULL);
    clear(filebrowser->ft);
    clear(filebrowser->ct);
    int ds = get_files(filebrowser,filebrowser->path, 1);
    filebrowser->ct = add_combobox(filebrowser->w, "", 20, 40, 580, 30);
    center_widget(filebrowser->w,filebrowser->ct);
    filebrowser->ct->parent_struct = filebrowser;
    filebrowser->ct->func.value_changed_callback = combo_response;
    filebrowser->ft = add_listbox(filebrowser->w, "", 20, 90, 620, 225);
    filebrowser->ft->parent_struct = filebrowser;
    filebrowser->ft->func.button_release_callback = file_released_callback;
    set_files(filebrowser);
    center_widget(filebrowser->w,filebrowser->ft);
    set_dirs(filebrowser);
    widget_show_all(filebrowser->w);
    combobox_set_active_entry(filebrowser->ct, ds);
    listbox_set_active_entry(filebrowser->ft, 0);
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
        filebrowser->show_hidden = adj_get_value(w->adj) ? true : false;
        reload_all(filebrowser);
    }
}

static void set_filter_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    if (filebrowser->use_filter != (int)adj_get_value(w->adj)) {
        filebrowser->use_filter = adj_get_value(w->adj);
        reload_file_entrys(filebrowser);
    }
}

static void mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileBrowser *filebrowser = w->parent_struct;
    clear_filebuffer(filebrowser);
    clear_dirbuffer(filebrowser);
    free(filebrowser->selected_file);
    free(filebrowser->path);
    free(filebrowser->command);
}


int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    FileBrowser filebrowser;
    filebrowser.n=0;
    filebrowser.m=0;
    filebrowser.use_filter = 0;
    filebrowser.show_hidden = false;
    filebrowser.file_names = NULL;
    filebrowser.dir_names = NULL;
    filebrowser.selected_file = NULL;
    filebrowser.path = NULL;
    filebrowser.command = NULL;
    asprintf(&filebrowser.path, "%s",getenv("HOME") ? getenv("HOME") : "/");
    assert(filebrowser.path != NULL);

    filebrowser.w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 660, 420);
    filebrowser.w->flags |= HAS_MEM;
    filebrowser.w->parent_struct = &filebrowser;
    XStoreName(app.dpy, filebrowser.w->widget, "File Selector");
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

    int ds = get_files(&filebrowser,filebrowser.path, 1);   
    set_files(&filebrowser); 
    set_dirs(&filebrowser);
    combobox_set_active_entry(filebrowser.ct, ds);
    listbox_set_active_entry(filebrowser.ft, 0);

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
