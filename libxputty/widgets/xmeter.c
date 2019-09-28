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


#include "xmeter.h"
#include "xmeter_private.h"



/**
 * @brief add_vmeter          - add a vumeter to a Widget_t
 * connect to func.value_changed_callback to implement your actions
 * @param *parent             - pointer to the Widget_t request the button
 * @param *label              - Label to show on the button
 * @param x,y,width,height    - the position/geometry to create the button
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* add_vmeter(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    _create_vertical_meter_image(wid, width, height);
    wid->label = label;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,0.001, CL_METER);
    wid->adj = wid->adj_y;
    wid->scale.gravity = ASPECT;
    wid->func.expose_callback = _draw_v_meter;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    return wid;
}

/**
 * @brief add_hmeter          - add a vumeter to a Widget_t
 * connect to func.value_changed_callback to implement your actions
 * @param *parent             - pointer to the Widget_t request the button
 * @param *label              - Label to show on the button
 * @param x,y,width,height    - the position/geometry to create the button
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* add_hmeter(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    _create_horizontal_meter_image(wid, width, height);
    wid->label = label;
    wid->adj_x = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,0.001, CL_METER);
    wid->adj = wid->adj_x;
    wid->scale.gravity = ASPECT;
    wid->func.expose_callback = _draw_h_meter;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    return wid;
}
