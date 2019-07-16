# Xputty

![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/logo-220x1601.png)
Do What The Fuck You Want To Public License (WTFPL) 

A damn tiny abstraction Layer to create X11 window/widgets  with cairo surfaces 

Here is the usual hello world:

![simple-example](https://github.com/brummer10/Xputty/raw/master/examples/simple-example.png)

produced  by this code:

```C
#include "xputty.h"

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    cairo_push_group (w->cr);
    cairo_set_source_rgb (w->cr, 1, 1, 1);
    cairo_paint (w->cr);
    cairo_pop_group_to_source (w->cr);
    cairo_paint (w->cr);
}

int main (int argc, char ** argv)
{
    Display *dpy = XOpenDisplay(0);
    XContext context =  XUniqueContext();
    Widget_t *w;

    w = create_window(dpy, DefaultRootWindow(dpy), context, 0, 0, 300, 200);
    XStoreName(dpy, w->widget, "Xputty Hello world");
    w->label = "How are you?";
    w->func.expose_callback = draw_window;
    bool run = true;
    loop(w,context,&run);
    destroy_widget( w, context);
    XCloseDisplay(dpy);
    return 0;
}
```

check out the example folder for more examples.
