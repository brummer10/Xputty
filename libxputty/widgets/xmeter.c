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
 * @brief power2db             - power (db) input to db output consider falloff
 * @param power                - mesured power
 * @return float               - calculated db
 */

float power2db(Widget_t *w, float power) {
    const float falloff = 27 * 60 * 0.0005;
    const float fallsoft = 6 * 60 * 0.0005;
    //power = 20.*log10(power);
    if (power <=  20.*log10(0.00021)) { // -70db
        power = 20.*log10(0.00000000001); //-137db
        w->adj->start_value = min(0.0,w->adj->start_value - fallsoft);
    }
    // retrieve old meter value and consider falloff
    if (power < w->adj->std_value) {
        power = max(power, w->adj->std_value - falloff);
        w->adj->start_value = min(0.0,w->adj->start_value - fallsoft);
    }
    if (power > w->adj->start_value) {
        w->adj->start_value = power ;
    }
    
    w->adj->std_value = power;
    return  power;
}

/**
 * @brief add_vmeter          - add a vumeter to a Widget_t
 * connect to func.value_changed_callback to implement your actions
 * @param *parent             - pointer to the Widget_t request the vumeter
 * @param *label              - Label to show on the vumeter
 * @param x,y,width,height    - the position/geometry to create the button
 * @return Widget_t*          - pointer to the Widget_t meter struct
 */

Widget_t* add_vmeter(Widget_t *parent, const char * label, bool show_scale,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    _create_vertical_meter_image(wid, width, height);
    wid->label = label;
    wid->adj_y = add_adjustment(wid,-70.0, -70.0, -70.0, 6.0,0.001, CL_METER);
    wid->adj = wid->adj_y;
    wid->scale.gravity = ASPECT;
    wid->func.expose_callback = _draw_v_meter;
    if (show_scale) {
        Widget_t *wid2 = create_widget(parent->app, parent, x+width, y, width, height);
        wid2->scale.gravity = ASPECT;
        wid2->func.expose_callback =_draw_vmeter_scale;
    }
    return wid;
}

/**
 * @brief add_hmeter          - add a vumeter to a Widget_t
 * connect to func.value_changed_callback to implement your actions
 * @param *parent             - pointer to the Widget_t request the vumeter
 * @param *label              - Label to show on the vumeter
 * @param x,y,width,height    - the position/geometry to create the button
 * @return Widget_t*          - pointer to the Widget_t meter struct
 */

Widget_t* add_hmeter(Widget_t *parent, const char * label, bool show_scale,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    _create_horizontal_meter_image(wid, width, height);
    wid->label = label;
    wid->adj_x = add_adjustment(wid,-70.0, -70.0, -70.0, 6.0,0.001, CL_METER);
    wid->adj = wid->adj_x;
    wid->scale.gravity = ASPECT;
    wid->func.expose_callback = _draw_h_meter;
    if (show_scale) {
        Widget_t *wid2 = create_widget(parent->app, parent, x, y+height, width, height);
        wid2->scale.gravity = ASPECT;
        wid2->func.expose_callback =_draw_hmeter_scale;
    }
    return wid;
}
