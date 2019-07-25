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

/**
 * here are the private functions from xadjustment
 */


#pragma once

#ifndef XADJUSTMENT_PRIVATE_H_
#define XADJUSTMENT_PRIVATE_H_

#include "xputty.h"

/**
 * @brief _check_value_changed  - check if value has changed and send
 * adj_callback if so
 * @param *adj                  - pointer to the Adjustment 
 * @param v                     - value to check 
 * @return void
 */

void _check_value_changed(Adjustment_t *adj, float *value);

#endif // XADJUSTMENT_PRIVATE_H_
