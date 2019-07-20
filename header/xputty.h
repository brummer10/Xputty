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

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				xputty library headers
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#include "xwidget.h"
#include "xadjustment.h"
#include "xchildlist.h"


#endif //XPUTTY_H_

