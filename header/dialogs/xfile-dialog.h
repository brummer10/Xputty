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


#pragma once

#ifndef XFILE_DIALOG_H_
#define XFILE_DIALOG_H_

#include "xwidgets.h"
#include "xdgmime.h"
#include "xasprintf.h"
#include "xfilepicker.h"

typedef struct {
    Widget_t *parent;
    Widget_t *w;
    Widget_t *ct;
    Widget_t *ft;
    Widget_t *w_quit;
    Widget_t *w_okay;
    Widget_t *w_hidden;
    Widget_t *sel_dir;
    Widget_t *set_filter;
    FilePicker *fp;
    bool send_clear_func;
} FileDialog;

typedef struct {
    Widget_t *w;
    char *last_path;
    const char *path;
    const char *filter;
    bool is_active;
} FileButton;

Widget_t *open_file_dialog(Widget_t *w, const char *path, const char *filter);

Widget_t *open_file_button(Widget_t *parent, int x, int y, int width, int height,
                           const char *path, const char *filter);

#endif //XFILE_DIALOG_H_

