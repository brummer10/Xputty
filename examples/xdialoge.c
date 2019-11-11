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

static void dialog_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if(user_data !=NULL && strlen(*(const char**)user_data)) {
        printf("FILE: %s\n", *(const char**)user_data);
    } else {
        printf("NOFILE:\n");
    }
    if(w->data && user_data !=NULL && strlen(*(const char**)user_data)) {
        char * command = NULL;
        asprintf(&command, "xdg-open '%s'",*(const char**)user_data);
        assert(command);
        if (system(NULL)) system(command);
        free(command);
        command = NULL;
    }
    destroy_widget(w,w->app);
}


int main (int argc, char ** argv)
{
    extern char *optarg;
    char *path = NULL;
    char *filter = NULL;
    int x = 0;
    int l = 0;
    int c = 0;

    static char usage[] = "usage: %s \n"
    "[-p path] optional set a path to open\n"
    "[-f filter] optional set a file filter\n"
    "[-x ] open response file with xdg-open\n"
    "[-l ] use light color theme\n";

    while ((c = getopt(argc, argv, "p:f:lx?")) != -1) {
        switch (c) {
            break;
            case 'p': path = optarg;
            break;
            case 'f': filter = optarg;
            break;
            case 'x': x = 1;
            break;
            case 'l': l = 1;
            break;
            case '?': fprintf(stderr, usage, argv[0]);
                exit(1);
            break;
        }
    }

    Xputty app;
    main_init(&app);
    Widget_t *w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 1, 1);
    if (x) w->data = 1;
    if (l) set_light_theme(&app);
    w->func.dialog_callback = dialog_response;
    open_file_dialog(w,path,filter);
    main_run(&app);
    main_quit(&app);
    return 0;
}
    
