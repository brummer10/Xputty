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

#ifndef XPUTTY1_H_
#define XPUTTY1_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#include <math.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>


/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
					define debug print
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#ifndef DEBUG
#define DEBUG 0
#endif
#define debug_print(...) \
            ((void)((DEBUG) ? fprintf(stderr, __VA_ARGS__) : 0))

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				define min/max if not defined already
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x, y) ((x) < (y) ? (y) : (x))
#endif

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
					define check if char holds UTF 8 string
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#define IS_UTF8(c) (((c)&0xc0)==0xc0)

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				forward declareted structs
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

typedef struct Childlist_t Childlist_t;
typedef struct Adjustment_t Adjustment_t ;
typedef struct Widget_t Widget_t;
typedef struct Xputty Xputty;

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				xputty library headers
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#include "xwidget.h"
#include "xadjustment.h"
#include "xchildlist.h"


/**
 * @brief Xputty             - the main struct. It should be declared
 * before any other call to a Xputty function.
 * Example: 
 * Xputty main;
 * @param *childlist         - pointer to the main childlist
 * @param *dpy               - pointer to the display in use
 * @param run                - bool to quit the main loop
 */

struct Xputty{
/** pointer to the main childlist */
    Childlist_t *childlist;
/** pointer to the display in use */
    Display *dpy;
/** bool to quit the main loop */
    bool run;
};

/**
 * @brief main_init         - open the Display and init the 
 * main->childlist. Also it set the bool run to true. 
 * This one will be used to terminate the main event loop.
 * main_init() should be called directly after declaration of Xputty.
 * Example:
 * main_init(&main);
 * Any Widget_t which would be created afterwards will be added to the list.
 * This list is used to check if a Widget_t is valid.
 * When a Widget_t call quit_widget() it will be removed from the list.
 * On main_quit() any remaining Widget_t from this list will be destroyed,
 * to ensure that we leave the memory clean.
 * This list is also used to check which Window receive a XEvent.
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */

void main_init(Xputty *main);

/**
 * @brief main_run          - the main event loop. I should be start after 
 * your Widget_s been created. You could create and destroy additional Widget_s
 * at any time later during run. 
 * Eample:
 * main_run(&main);
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */

void main_run(Xputty *main);

/**
 * @brief main_quit         - destroy all remaining Widget_t's from the
 * main->childlist. Free all resources which may be allocated between init
 * and quit. It should be called after main_run();
 * Example:
 * main_quit(&main);
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */

void main_quit(Xputty *main);

#endif //XPUTTY_H_

