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

#include "xadjustment.h"


/**
 * @brief *add_adjustment    - adding a adjustment to the widget
 * @param *w                 - pointer to the Widget_t request a adjustment
 * @param std_value          - standard value of the adjustment
 * @param value              - current value of the adjustment
 * @param min_value          - minimum value of the adjustment
 * @param max_value          - maximal value of the adjustment
 * @param step               - step to increase/decrease the adjustment
 * @param type               - set CL_type of adjustment
 * @return *adj              - pointer to adjustment
 */

Adjustment_t *add_adjustment(Widget_t *w, float std_value, float value,
                float min_value,float max_value, float step, CL_type type) {
    Adjustment_t *adj = (Adjustment_t*)malloc(sizeof(Adjustment_t));
    assert(adj);
    *(adj) = (Adjustment_t){w, std_value, value, min_value, max_value, step, value, type};
    
    
    debug_print("Widget_t add adjustment\n");
    return adj;
}

/**
 * @brief delete_adjustment  - freeing the memory of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @return *void             - return NULL
 */

void *delete_adjustment(Adjustment_t *adj) {
    if(adj) {
        free(adj);
        debug_print("Widget_t delete adjustment\n");
    }
    return NULL;
}

/**
 * @brief adj_get_state      - get the current state of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @return float             - return the adjustment state (0<->1)
 */

float adj_get_state(Adjustment_t *adj) {
    if (!adj) return 0;
    return (adj->value - adj->min_value) /
       (adj->max_value - adj->min_value);
}

/**
 * @brief adj_get_value      - get the current value of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @return float             - return the adjustment value
 */

float adj_get_value(Adjustment_t *adj) {
    if (!adj) return 0;
    return (adj->value);
}

/**
 * @brief adj_set_value      - set the current value of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @param v                  - value set the Adjustment to 
 * @return void
 */

void adj_set_value(Adjustment_t *adj, float v) {
    if (!adj) return;
    adj->value = v;
    expose_widget(adj->w);
}

/**
 * @brief adj_set_start_value- set start value of the adjustment
 * @param *w               - pointer to Widget_t containing the adjustment 
 * @return void
 */

void adj_set_start_value(void *w) {
    Widget_t * wid = (Widget_t*)w;
    if(wid->adj_x)wid->adj_x->start_value = wid->adj_x->value;
    if(wid->adj_y)wid->adj_y->start_value = wid->adj_y->value;
}

/**
 * @brief adj_set_state      - set value/state of the adjustment
 * @param *adj               - pointer to Widget_t containing the adjustment
 * @param x                  - value for the xaxis
 * @param y                  - value for the yaxis
 * @return void
 */

void adj_set_state(void *w, float x, float y) {
    Widget_t * wid = (Widget_t*)w;
    const float scaling = 0.5;
    if(wid->adj_x) {
        switch(wid->adj_x->type) {
            case (CL_CONTINUOS):
            {
                float state = (wid->adj_x->start_value - wid->adj_x->min_value) / 
                    (wid->adj_x->max_value - wid->adj_x->min_value);
                float nsteps = wid->adj_x->step / (wid->adj_x->max_value - wid->adj_x->min_value);
                float nvalue = min(1.0,max(0.0,state + ((float)(x - wid->pos_x)*scaling *nsteps)));
                wid->adj_x->value = nvalue * (wid->adj_x->max_value - wid->adj_x->min_value) + wid->adj_x->min_value;
            }
            break;
            case (CL_TOGGLE):
                wid->adj_x->value = wid->adj_x->value ? 0.0 : 1.0;
            break;
            default:
            break;
        }
    }
    if(wid->adj_y) {
        switch(wid->adj_y->type) {
            case (CL_CONTINUOS):
            {
                float state = (wid->adj_y->start_value - wid->adj_y->min_value) / 
                    (wid->adj_y->max_value - wid->adj_y->min_value);
                float nsteps = wid->adj_y->step / (wid->adj_y->max_value - wid->adj_y->min_value);
                float nvalue = min(1.0,max(0.0,state + ((float)(wid->pos_y - y)*scaling *nsteps)));
                wid->adj_y->value = nvalue * (wid->adj_y->max_value - wid->adj_y->min_value) + wid->adj_y->min_value;
            }
            break;
            case (CL_TOGGLE):
                wid->adj_y->value = wid->adj_y->value ? 0.0 : 1.0;
            break;
            default:
            break;
        }
    }
   // if(wid->adj_x || wid->adj_y) expose_widget(wid);
}
