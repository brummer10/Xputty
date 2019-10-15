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


/* define _GNU_SOURCE to remove dependency of xasprintf.h */

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

#pragma once

#ifndef XFILEPICKER_H_
#define XFILEPICKER_H_

/* comment out xdgmine.h will disable the file filter, you may implement your own.*/
#include "xdgmime.h"
/* comment out xasprintf and define _GNU_SOURCE on top of this file will remove this dependency.*/
#include "xasprintf.h"

#if defined(WIN32) || defined(_WIN32) 
#define PATH_SEPARATOR "\\" 
#else 
#define PATH_SEPARATOR "/" 
#endif

typedef struct {
    int use_filter;
    bool show_hidden;
    unsigned int n;
    unsigned int m;
    char *filter;
    char *path;
    char *command;
    char *selected_file;
    char **file_names;
    char **dir_names;
} FilePicker;


static inline int fp_compare_fun (const void *p1, const void *p2) {
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline int fp_compare_hidden_dirs_fun (const void *p1, const void *p2) {
    if(strstr(*(const char**)p1, PATH_SEPARATOR".") && strstr(*(const char**)p2, PATH_SEPARATOR".")) return 0;
    if(strstr(*(const char**)p1, PATH_SEPARATOR".")) return 1;
    if(strstr(*(const char**)p2, PATH_SEPARATOR".")) return -1;
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline int fp_compare_hidden_files_fun (const void *p1, const void *p2) {
    if(strncmp(*(const char**)p1, ".",1)==0 && strncmp(*(const char**)p2, ".",1)==0) return 0;
    if(strncmp(*(const char**)p1, ".",1)==0 ) return 1;
    if(strncmp(*(const char**)p2, ".",1)==0 ) return -1;
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline bool fp_show_hidden_files(FilePicker *filepicker, char* file) {
    return (filepicker->show_hidden) ? strcmp(file,".")!=0 : (file[0] != '.');
}

static inline bool fp_show_filter_files(FilePicker *filepicker, char* file) {
    if (!filepicker->use_filter) {
        return true;
    } else {
#ifdef __XDG_MIME_H__
        return strstr(xdg_mime_get_mime_type_from_file_name(file), filepicker->filter);
#else
        return true;
#endif
    }
}

static inline void fp_sort_buffers(FilePicker *filepicker, int get_dirs) {
    if (filepicker->m>1 && get_dirs) {
        qsort (filepicker->dir_names, filepicker->m,
          sizeof filepicker->dir_names[0], (filepicker->show_hidden) ?
          fp_compare_hidden_dirs_fun : fp_compare_fun);
    }
    if (filepicker->n>1) {
        qsort (filepicker->file_names, filepicker->n,
          sizeof filepicker->file_names[0], (filepicker->show_hidden) ?
          fp_compare_hidden_files_fun : fp_compare_fun);
    }
}

static inline void fp_clear_filebuffer(FilePicker *filepicker) {
    int j = 0;
    for(; j<filepicker->n;j++) {
        free(filepicker->file_names[j]);
        filepicker->file_names[j] = NULL;
    }
    if(filepicker->n) {
        free(filepicker->file_names);
        filepicker->file_names = NULL;
        filepicker->n=0;
    }
}

static inline void fp_clear_dirbuffer(FilePicker *filepicker) {
    int j = 0;
    for(; j<filepicker->m;j++) {
        free(filepicker->dir_names[j]);
        filepicker->dir_names[j] = NULL;
    }
    if(filepicker->m) {
        free(filepicker->dir_names);
        filepicker->dir_names = NULL;
        filepicker->m=0;
    }
}

static inline int fp_prefill_dirbuffer(FilePicker *filepicker, char *path) {
    int ret = 0;
    if (strcmp (path, PATH_SEPARATOR) == 0) {
        filepicker->dir_names = (char **)realloc(filepicker->dir_names,
          (filepicker->m + 1) * sizeof(char *));
        assert(filepicker->dir_names != NULL);
        asprintf(&filepicker->dir_names[filepicker->m++], "%s",path);
        assert(&filepicker->dir_names[filepicker->m] != NULL);
    } else {
        char *ho;
        asprintf(&ho, "%s",path);
        assert(ho != NULL);
        while (strcmp (ho, PATH_SEPARATOR) != 0) {
            filepicker->dir_names = (char **)realloc(filepicker->dir_names,
              (filepicker->m + 1) * sizeof(char *));
            assert(filepicker->dir_names != NULL);
            asprintf(&filepicker->dir_names[filepicker->m++], "%s",dirname(ho));
            assert(&filepicker->dir_names[filepicker->m] != NULL);
            ret++;
        }
        if (strcmp (path, PATH_SEPARATOR) != 0) {
            filepicker->dir_names = (char **)realloc(filepicker->dir_names,
              (filepicker->m + 1) * sizeof(char *));
            assert(filepicker->dir_names != NULL);
            asprintf(&filepicker->dir_names[filepicker->m++], "%s",path);
            assert(&filepicker->dir_names[filepicker->m] != NULL);
        }
        free(ho);
    }
    return ret;   
}

static inline int fp_get_files(FilePicker *filepicker, char *path, int get_dirs) {
    int ret = 0;
    fp_clear_filebuffer(filepicker);
    if(get_dirs) {
        fp_clear_dirbuffer(filepicker);
        ret = fp_prefill_dirbuffer(filepicker, path);
    }

    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(path);
    while ((dp = readdir(dirp)) != NULL) {

        if(dp-> d_type != DT_DIR && strlen(dp->d_name)!=0 && dp->d_type != DT_UNKNOWN
          && strcmp(dp->d_name,"..")!=0 && fp_show_hidden_files(filepicker, dp->d_name) &&
          fp_show_filter_files(filepicker, dp->d_name)) {

            filepicker->file_names = (char **)realloc(filepicker->file_names,
              (filepicker->n + 1) * sizeof(char *));
            assert(filepicker->file_names != NULL);
            asprintf(&filepicker->file_names[filepicker->n++],"%s",dp->d_name);
            assert(&filepicker->file_names[filepicker->n] != NULL);

        } else if(get_dirs && dp -> d_type == DT_DIR && strlen(dp->d_name)!=0
          && strcmp(dp->d_name,"..")!=0 && fp_show_hidden_files(filepicker, dp->d_name)) {

            filepicker->dir_names = (char **)realloc(filepicker->dir_names,
              (filepicker->m + 1) * sizeof(char *));
            assert(filepicker->dir_names != NULL);
            asprintf(&filepicker->dir_names[filepicker->m++], (strcmp(path, PATH_SEPARATOR) != 0) ?
              "%s"PATH_SEPARATOR"%s" : "%s%s" , path,dp->d_name);
            assert(&filepicker->dir_names[filepicker->m] != NULL);
        }
    }
    closedir(dirp);
    fp_sort_buffers(filepicker, get_dirs);
    return ret;
}

static inline void fp_free(FilePicker *filepicker) {
    fp_clear_filebuffer(filepicker);
    fp_clear_dirbuffer(filepicker);
    free(filepicker->selected_file);
    free(filepicker->path);
    free(filepicker->command);
    free(filepicker->filter);
}

static inline void fp_init(FilePicker *filepicker, const char *path) {
    filepicker->n=0;
    filepicker->m=0;
    filepicker->use_filter = 0;
    filepicker->show_hidden = false;
    filepicker->file_names = NULL;
    filepicker->dir_names = NULL;
    filepicker->selected_file = NULL;
    filepicker->path = NULL;
    filepicker->command = NULL;
    filepicker->filter = NULL;
    asprintf(&filepicker->path, path);
    assert(filepicker->path != NULL);
}

#endif //XFILEPICKER_H_
