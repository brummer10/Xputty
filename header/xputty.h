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
 * @brief Xputty             - the main struct
 * @param *childlist         - pointer to the main childlist
 * @param *dpy               - pointer to the display in use
 * @param *context           - pointer to the main context
 * @param run                - bool to quit the main loop
 */

struct Xputty{
/** pointer to the main childlist */
    Childlist_t *childlist;
/** pointer to the display in use */
    Display *dpy;
/** the main context */
    XContext context;
/** bool to quit the main loop */
    bool run;
};

/**
 * @brief main_init         - init the Xputty struct
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */

void main_init(Xputty *main);

/**
 * @brief main_run         - the main event loop
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */

void main_run(Xputty *main);

/**
 * @brief main_quit         - clean up afterr quiting the main loop
 * @param *main             - pointer to the main Xputty struct
 * @return void 
 */

void main_quit(Xputty *main);

#endif //XPUTTY_H_

