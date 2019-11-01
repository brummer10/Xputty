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

#ifndef XMIDI_KEYBOARD_H_
#define XMIDI_KEYBOARD_H_

#include "xwidgets.h"

typedef void (*midikeyfunc)(int *key, bool on_off);
typedef void (*midiwheelfunc)(int *value);

typedef struct {
    int octave;
    int layout;
    int modwheel;
    int pitchwheel;
    int prelight_key;
    int active_key;
    int send_key;
    unsigned long key_matrix[4];

    midikeyfunc mk_send_note;
    midiwheelfunc mk_send_pitch;
    midiwheelfunc mk_send_mod;
    midiwheelfunc mk_send_all_sound_off;
} MidiKeyboard;

Widget_t *open_midi_keyboard(Widget_t *w);

#endif //XMIDI_KEYBOARD_H_
