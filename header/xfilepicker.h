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
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

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
    char *selected_file;
    char **file_names;
    char **dir_names;
} FilePicker;


int fp_compare_fun (const void *p1, const void *p2);

int fp_compare_hidden_dirs_fun (const void *p1, const void *p2);

int fp_compare_hidden_files_fun (const void *p1, const void *p2);

bool fp_show_hidden_files(FilePicker *filepicker, char* file);

bool fp_show_filter_files(FilePicker *filepicker, char* file);

void fp_sort_buffers(FilePicker *filepicker, int get_dirs);

void fp_clear_filebuffer(FilePicker *filepicker);

void fp_clear_dirbuffer(FilePicker *filepicker);

int fp_prefill_dirbuffer(FilePicker *filepicker, char *path);

int fp_get_files(FilePicker *filepicker, char *path, int get_dirs);

void fp_free(FilePicker *filepicker);

void fp_init(FilePicker *filepicker, const char *path);

#endif //XFILEPICKER_H_
