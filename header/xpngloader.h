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

#ifndef XPNGLOADER_H_
#define XPNGLOADER_H_

#include "xputty.h"


/**  
        define some MACROS to read png data from binary stream 
        png's been converted to object files with
        ld -r -b binary name.png -o name.o
*/

#ifdef __APPLE__
#include <mach-o/getsect.h>

#define EXTLD(NAME) \
  extern const unsigned char _section$__DATA__ ## NAME [];
#define LDVAR(NAME) _section$__DATA__ ## NAME
#define LDLEN(NAME) (getsectbyname("__DATA", "__" #NAME)->size)

#elif (defined __WIN32__)  /* mingw */

#define EXTLD(NAME) \
  extern const unsigned char binary_ ## NAME ## _start[]; \
  extern const unsigned char binary_ ## NAME ## _end[];
#define LDVAR(NAME) \
  binary_ ## NAME ## _start
#define LDLEN(NAME) \
  ((binary_ ## NAME ## _end) - (binary_ ## NAME ## _start))

#else /* gnu/linux ld */

#define EXTLD(NAME) \
  extern const unsigned char _binary_ ## NAME ## _start[]; \
  extern const unsigned char _binary_ ## NAME ## _end[];
#define LDVAR(NAME) \
  _binary_ ## NAME ## _start
#define LDLEN(NAME) \
  ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))
#endif


/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                    define needed structs
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief binary_stream      - struct definition to read binary data 
 * into cairo surface
 * @param *data              - pointer to the binary image data
 * @param position           - pointer to the read position
 */

typedef struct  {
    const unsigned char * data;
    long int position;
} binary_stream;

#include "xresources.h"

/**
 * @brief cairo_image_surface_create_from_stream  - read binary data 
 * into cairo surface from stream
 * @param *name                      - pointer to the binary image data
 * @return cairo_surface_t           - the cairo_image_surface
 */

cairo_surface_t *cairo_image_surface_create_from_stream ( const unsigned char* name);

/**
 * @brief widget_get_png             - read png into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the png
 * @param *name                      - pointer to the binary image data LDVAR(name)
 * @return void
 */

void widget_get_png(Widget_t *w, const unsigned char* name);

#endif //XPNGLOADER_H_
