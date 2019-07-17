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

#ifndef XADJUSTMENT_H_
#define XADJUSTMENT_H_


/**
 * 
 * @brief Adjustment_t     - struct to hold a controller adjustment
 * 
 */

typedef struct {
	float std_value;
	float value;
	float min_value;
	float max_value;
	float step;
    float start_value;
} Adjustment_t;

/**
 * @brief *add_adjustment    - adding a adjustment to the widget
 * @param *w                 - pointer to the Widget_t request a adjustment
 * @param std_value          - standard value of the adjustment
 * @param value              - current value of the adjustment
 * @param min_value          - minimum value of the adjustment
 * @param max_value          - maximal value of the adjustment
 * @param step               - step to increase/decrease the adjustment
 * @return *adj              - pointer to adjustment
 */

Adjustment_t *add_adjustment(void *w_, float std_value, float value,
                     float min_value,float max_value, float step);


/**
 * @brief delete_adjustment  - freeing the memory of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @return *void             - return NULL
 */

void *delete_adjustment(Adjustment_t *adj);

/**
 * @brief adj_get_state      - freeing the memory of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @return float             - return the adjustment state (0<->1)
 */

float adj_get_state(Adjustment_t *adj);

/**
 * @brief adj_get_value      - get the current value of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @return float             - return the adjustment value
 */

float adj_get_value(Adjustment_t *adj);

/**
 * @brief adj_set_value      - set the current value of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @return void
 */

void adj_set_value(Adjustment_t *adj, float v);

/**
 * @brief adj_set_start_value- set start value of the adjustment
 * @param *w               - pointer to Widget_t containing the adjustment 
 * @return void
 */

void adj_set_start_value(void *w);

/**
 * @brief adj_set_state      - set value of the adjustment
 * @param *adj               - pointer to Widget_t containing the adjustment
 * @return void
 */

void adj_set_state(void *w, float x, float y);

#endif //XADJUSTMENT_H_
