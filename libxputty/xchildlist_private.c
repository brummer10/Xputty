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

#include "xchildlist_private.h"

/**
 * @brief _childlist_add_elem - reallocate the childlist array to new size
 * @param *childlist          - pointer to the Childlist_t
 * @return void 
 */

void _childlist_add_elem(Childlist_t *childlist) {
    childlist->childs = realloc(childlist->childs, sizeof(Widget_t*) * (4+childlist->cap));
    assert(childlist->childs != NULL);
    childlist->cap +=4;
    childlist->size = sizeof(childlist);
    int i = childlist->elem+1;
    for(;i<childlist->cap;i++) {
        childlist->childs[i] = NULL;
    }
}
