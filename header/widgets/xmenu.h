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

#ifndef XMENU_H_
#define XMENU_H_

#include "xputty.h"

/**
 * @brief item_height       - define the height of a menu item
 */

#define item_height 30

/**
 * @brief pop_menu_show       - pop up a menu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param *menu               - the menu to show
 * @return void
 */

void pop_menu_show(Widget_t *parent, Widget_t *menu);

/**
 * @brief create_menu         - create a menu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param items               - define how many entrys the menu should have
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* create_menu(Widget_t *parent, int items);

/**
 * @brief menu_add_item       - add a item to menu
 * @param *menu               - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t* menu_add_item(Widget_t *menu,const char * label);

#endif //XMENU_H_
