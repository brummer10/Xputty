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

#include "xpngloader.h"

/* 
 * @brief      load png data from binary blob into cairo surface
*/

/**
 * @brief png_stream_reader          - read binary data bytes
 * into cairo surface from binary stream
 * @param *stream                    - the stream to read from
 * @param length                     - the length to be read
 * @return CAIRO_STATUS_SUCCESS      - the result
 */


cairo_status_t png_stream_reader (void *_stream, unsigned char *data, unsigned int length) {
    binary_stream * stream = (binary_stream *) _stream;
    memcpy(data, &stream->data[stream->position],length);
    stream->position += length;
    return CAIRO_STATUS_SUCCESS;
}

/**
 * @brief cairo_image_surface_create_from_stream  - read binary data 
 * into cairo surface from stream
 * @param *name                      - pointer to the binary image data
 * @return cairo_surface_t           - the cairo_image_surface
 */

cairo_surface_t *cairo_image_surface_create_from_stream ( const unsigned char* name) {
    binary_stream png_stream;
    png_stream.data = name;
    png_stream.position = 0;
    return cairo_image_surface_create_from_png_stream(&png_stream_reader, (void *)&png_stream);
}

/**
 * @brief widget_get_png             - read png into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the png
 * @param *name                      - pointer to the binary image data LDVAR(name)
 * @return void
 */

void widget_get_png(Widget_t *w, const unsigned char* name) {
    cairo_surface_t *getpng = cairo_image_surface_create_from_stream (name);
    int width = cairo_image_surface_get_width(getpng);
    int height = cairo_image_surface_get_height(getpng);
    
    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    cairo_t *cri = cairo_create (w->image);
    cairo_set_source_surface (cri, getpng,0,0);
    cairo_paint (cri);
    cairo_surface_destroy(getpng);
    cairo_destroy(cri);
}

/*
cairo_surface_t* iamge = cairo_image_surface_create_from_stream( LDVAR(image_name_png));
*/
