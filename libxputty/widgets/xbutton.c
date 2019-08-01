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


#include "xbutton.h"
#include "xbutton_private.h"


/**
 * @brief add_button          - add a button to a Widget_t
 * connect to func.value_changed_callback to implement your actions
 * @param *parent             - pointer to the Widget_t request the button
 * @param *label              - Label to show on the button
 * @param x,y,width,height    - the position/geometry to create the button
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* add_button(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
  //  _set_button_colormap(wid);
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,1.0, CL_TOGGLE);
    wid->adj = wid->adj_y;
    wid->scale.gravity = CENTER;
    wid->func.expose_callback = _draw_button;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    wid->func.button_press_callback = _button_pressed;
    wid->func.button_release_callback = _button_released;
    return wid;
}

/**
 * @brief add_toggle_button          - add a button to a Widget_t
 * connect to func.value_changed_callback to implement your actions
 * @param *parent             - pointer to the Widget_t request the button
 * @param *label              - Label to show on the button
 * @param x,y,width,height    - the position/geometry to create the button
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* add_toggle_button(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
   // _set_button_colormap(wid);
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,1.0, CL_TOGGLE);
    wid->adj = wid->adj_y;
    wid->scale.gravity = CENTER;
    wid->func.expose_callback = _draw_button;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    wid->func.button_press_callback = _toggle_button_pressed;
    wid->func.button_release_callback = _toggle_button_released;
    return wid;
}
