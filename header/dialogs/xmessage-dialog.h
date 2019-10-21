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

#ifndef XMESSAGE_DIALOG_H_
#define XMESSAGE_DIALOG_H_

#include "xwidgets.h"

enum {
    INFO_BOX,
    WARNING_BOX,
    ERROR_BOX,
    QUESTION_BOX
};

typedef struct {
    int response;
    int message_type;
    unsigned int width;
    unsigned int height;
    unsigned int lin;
    char **message;
} MessageBox;

Widget_t *open_message_dialog(Widget_t *w, int style, const char *title, const char *message);

#endif //XMESSAGE_DIALOG_H_
