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

#include "xmessage-dialog.h"

void draw_message_label(Widget_t *w, int width, int height) {
    MessageBox *mb = w->parent_struct;
    cairo_text_extents_t extents;
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->crb, 12.0);
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    int i = 0;
    for(;i<mb->lin;i++) {
        cairo_text_extents(w->crb,mb->message[i] , &extents);
        cairo_move_to (w->crb, 100, ((40)+(extents.height * (2*i))));
        cairo_show_text(w->crb, mb->message[i]);
        cairo_new_path (w->crb);
    }    
}

void draw_message_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    int height_t = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_rectangle(w->crb,0,0,width_t,height_t);
    set_pattern(w,&w->app->color_scheme->selected,&w->app->color_scheme->normal,BACKGROUND_);
    cairo_fill (w->crb);

    widget_set_scale(w);
    int width = cairo_xlib_surface_get_width(w->image);
    int height = cairo_xlib_surface_get_height(w->image);
    double x = 64.0/(double)(width);
    double y = 64.0/(double)height;
    double x1 = (double)height/64.0;
    double y1 = (double)(width)/64.0;
    cairo_scale(w->crb, x,y);
    cairo_set_source_surface (w->crb, w->image, 50, 50);
    cairo_rectangle(w->crb,50, 50, width, height);
    cairo_fill(w->crb);
    cairo_scale(w->crb, x1,y1);

    draw_message_label(w,width_t,height_t);
    widget_reset_scale(w);
}

void draw_entry(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->cr,0,0,width,height);
    cairo_fill_preserve (w->cr);
    use_text_color_scheme(w, NORMAL_);
    cairo_set_line_width(w->cr, 2.0);
    cairo_stroke(w->cr);

    cairo_set_font_size (w->cr, 9.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);

    cairo_move_to (w->cr, 2, 9);
    cairo_show_text(w->cr, " ");
}

void entry_add_text(void  *w_, void *label_) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    char *label = (char*)label_;
    if (!label) {
        label = "";
    }
    draw_entry(w,NULL);
    cairo_text_extents_t extents;
    use_text_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->cr, 11.0);
    if (strlen( w->input_label))
         w->input_label[strlen( w->input_label)-1] = 0;
    if (strlen( w->input_label)<30) {
        if (strlen(label))
        strcat( w->input_label, label);
    }
    w->label = w->input_label;
    strcat( w->input_label, "|");
    cairo_set_font_size (w->cr, 12.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->cr, w->input_label , &extents);

    cairo_move_to (w->cr, 2, 12.0+extents.height);
    cairo_show_text(w->cr,  w->input_label);

}

void entry_clip(Widget_t *w) {
    draw_entry(w,NULL);
    cairo_text_extents_t extents;
    use_text_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->cr, 11.0);

    // check for UTF 8 char
    if (strlen( w->input_label)>=2) {
        int i = strlen( w->input_label)-1;
        int j = 0;
        int u = 0;
        for(;i>0;i--) {
            if(IS_UTF8(w->input_label[i])) {
                 u++;
            }
            j++;
            if (u == 1) break;
            if (j>2) break;
        }
        if (!u) j =2;

        memset(&w->input_label[strlen( w->input_label)-(sizeof(char)*(j))],0,sizeof(char)*(j));
        strcat( w->input_label, "|");
    }
    cairo_set_font_size (w->cr, 12.0);
    cairo_select_font_face (w->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents(w->cr, w->input_label , &extents);

    cairo_move_to (w->cr, 2, 12.0+extents.height);
    cairo_show_text(w->cr,  w->input_label);

}

void message_okay_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t *p = (Widget_t*)w->parent;
        MessageBox *mb = p->parent_struct;
        if(mb->message_type == QUESTION_BOX || mb->message_type == SELECTION_BOX) {
            Widget_t *pa = (Widget_t*)p->parent;
            pa->func.user_callback(pa,&mb->response);
        } else if(mb->message_type == ENTRY_BOX) {
            Widget_t *pa = (Widget_t*)p->parent;
            if (strlen( mb->text_entry->input_label))
                mb->text_entry->input_label[strlen( mb->text_entry->input_label)-1] = 0;
            mb->text_entry->label = mb->text_entry->input_label;
            pa->func.user_callback(pa,&mb->text_entry->label);
        }
        destroy_widget(p, p->app);
    }
}

void message_no_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        Widget_t *p = (Widget_t*)w->parent;
        MessageBox *mb = p->parent_struct;
        if(mb->message_type == QUESTION_BOX) {
            Widget_t *pa = (Widget_t*)p->parent;
            mb->response = -1;
            pa->func.user_callback(pa,&mb->response);
        }
        destroy_widget(p, p->app);
    }
}

void radio_box_set_active(Widget_t *w) {
    Widget_t * p = w->parent;
    MessageBox *mb = p->parent_struct;
    int response = 0;
    int i = 0;
    for(;i<p->childlist->elem;i++) {
        Widget_t *wid = p->childlist->childs[i];
        if (wid->adj && wid->flags & IS_RADIO) {
            ++response;
            if (wid != w) adj_set_value(wid->adj_y, 0.0);
            else if (wid == w) mb->response = response;
        }
    }
}

void radio_box_button_pressed(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_FOCUS) {
        radio_box_set_active(w);
    }
}

void create_checkboxes(Widget_t *w) {
    MessageBox *mb = w->parent_struct;
    int y = (mb->lin + 1) * 24 +12;
    int i = 0;
    for(;i<mb->sel;i++) {
        Widget_t *wid = add_check_box(w,mb->choices[i] , 100, y + (24*i), 15, 15);
        wid->flags |= IS_RADIO;
        wid->func.button_press_callback = radio_box_button_pressed;
   }
}

void entry_get_text(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XKeyEvent *key = (XKeyEvent*)key_;
    if (!key) return;
    int nk = key_mapping(w->app->dpy, key);
    if (nk) {
        switch (nk) {
            case 10: 
                {
                Widget_t *p = (Widget_t*)w->parent;
                MessageBox *mb = p->parent_struct;
                Widget_t *pa = (Widget_t*)p->parent;
                if (strlen( mb->text_entry->input_label))
                    mb->text_entry->input_label[strlen( mb->text_entry->input_label)-1] = 0;
                mb->text_entry->label = mb->text_entry->input_label;
                pa->func.user_callback(pa,&mb->text_entry->label);

                destroy_widget(p, p->app);
                }
            break;
            case 11: entry_clip(w);
            break;
            default:
            break;
        }
    } else {
        Status status;
        KeySym keysym;
        char buf[32];
        Xutf8LookupString(w->xic, key, buf, sizeof(buf) - 1, &keysym, &status);
        if(status == XLookupChars || status == XLookupBoth){
            entry_add_text(w, buf);
        }
    }
}

void create_entry_box(Widget_t *w) {
    MessageBox *mb = w->parent_struct;

    mb->text_entry = create_widget(w->app, w, 20, mb->height-90, mb->width-40, 40);
    memset(mb->text_entry->input_label, 0, 32 * (sizeof mb->text_entry->input_label[0]) );
    mb->text_entry->func.expose_callback = entry_add_text;
    mb->text_entry->func.key_press_callback = entry_get_text;
    mb->text_entry->flags &= ~USE_TRANSPARENCY;
    mb->text_entry->scale.gravity = CENTER;
}

void check_for_message(MessageBox *mb, const char *message) {
    if(!message) return;
    if(!strlen(message)) return;
    int len = 0;
    char *ms =strdup(message);
    char * p = strtok (ms, "\n");
    while (p) {
        mb->message = realloc (mb->message, sizeof (char*) * ++mb->lin);
        mb->message[mb->lin-1] = strdup(p);
        len = max(len, strlen(mb->message[mb->lin-1]));
        p = strtok (NULL, "\n");
    }
    free(ms);
    mb->width = len*12 ;
    mb->height = mb->lin*12+100;
}

void check_for_choices(MessageBox *mb, const char *choices) {
    if(!choices) return;
    if(!strlen(choices)) return;
    int len = 0;
    char *ms =strdup(choices);
    char * p = strtok (ms, "\n");
    while (p) {
        mb->choices = realloc (mb->choices, sizeof (char*) * ++mb->sel);
        mb->choices[mb->sel-1] = strdup(p);
        len = max(len, strlen(mb->choices[mb->sel-1]));
        p = strtok (NULL, "\n");
    }
    free(ms);
    mb->width = max(len*12,mb->width);
    mb->height += mb->sel*12+50;
}

void check_for_style(MessageBox *mb, int style) {
    if(style == ENTRY_BOX) {
        mb->width = max(330,mb->width);
        mb->height = max(140,mb->height+60);
    }
}

void mg_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    MessageBox *mb = w->parent_struct;
    if(mb->icon) {
        XFreePixmap(w->app->dpy, (*mb->icon));
        mb->icon = NULL;
    }
    int i = 0;
    for(;i<mb->lin;i++) {
        free(mb->message[i]);
    }
    i = 0;
    for(;i<mb->sel;i++) {
        free(mb->choices[i]);
    }
    free(mb->choices);
    free(mb);
}

Widget_t *open_message_dialog(Widget_t *w, int style, const char *title,
                              const char *message, const char *choices) {

    MessageBox *mb = (MessageBox*)malloc(sizeof(MessageBox));
    mb->response = 0;
    mb->message_type = 0;
    mb->lin = 0;
    mb->width = 330;
    mb->height = 200;
    mb->message = NULL;
    mb->sel = 0;
    mb->choices = NULL;
    mb->icon = NULL;
    check_for_message(mb, message);
    check_for_choices(mb, choices);
    check_for_style(mb, style);
    Widget_t *wid = create_window(w->app, DefaultRootWindow(w->app->dpy), 0, 0, mb->width, mb->height);
    wid->label = message;
    wid->flags |= HAS_MEM;
    wid->scale.gravity = CENTER;
    wid->parent_struct = mb;
    wid->parent = w;
    wid->func.mem_free_callback = mg_mem_free;
    wid->func.expose_callback = draw_message_window;
    char *alternate_title = NULL;
    char *button_title = "OK";
    switch (style) {
        case INFO_BOX:
            widget_get_png(wid, LDVAR(info_png));
            alternate_title = "INFO";
            mb->message_type = INFO_BOX;
            widget_set_icon(wid,mb->icon,wid->image);
        break;
        case WARNING_BOX:
            widget_get_png(wid, LDVAR(warning_png));
            alternate_title = "WARNING";
            mb->message_type = WARNING_BOX;
            widget_set_icon(wid,mb->icon,wid->image);
        break;
        case ERROR_BOX:
            widget_get_png(wid, LDVAR(error_png));
            alternate_title = "ERROR";
            mb->message_type = ERROR_BOX;
            widget_set_icon(wid,mb->icon,wid->image);
        break;
        case QUESTION_BOX:
            widget_get_png(wid, LDVAR(question_png));
            alternate_title = "QUESTION";
            Widget_t *no = add_button(wid, "NO", 10, mb->height-40, 60, 30);
            no->scale.gravity = CENTER;
            no->func.value_changed_callback = message_no_callback;
            mb->message_type = QUESTION_BOX;
            button_title = "YES";
            widget_set_icon(wid,mb->icon,wid->image);
        break;
        case SELECTION_BOX:
            widget_get_png(wid, LDVAR(choice_png));
            alternate_title = "SELECTION";
            mb->message_type = SELECTION_BOX;
            create_checkboxes(wid);
            widget_set_icon(wid,mb->icon,wid->image);
        break;
        case ENTRY_BOX:
            widget_get_png(wid, LDVAR(message_png));
            alternate_title = "TEXT ENTRY";
            mb->message_type = ENTRY_BOX;
            create_entry_box(wid);
            widget_set_icon(wid,mb->icon,wid->image);
        break;
    }
    XStoreName(w->app->dpy, wid->widget, strlen(title)? title : alternate_title);

    Widget_t *okay = add_button(wid, button_title, mb->width-70, mb->height-40, 60, 30);
    okay->scale.gravity = CENTER;
    okay->func.value_changed_callback = message_okay_callback;

    widget_show_all(wid);
    return wid;
}