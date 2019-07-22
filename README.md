# Xputty


A damn tiny abstraction Layer to create X11 window/widgets  with cairo surfaces 

Here is the usual hello world:

![simple-example](https://github.com/brummer10/Xputty/raw/master/examples/simple-example.png)

produced  by this code:

```C

#include "xputty.h"

static void draw_window(void *w_, void* buffer_context) {
    cairo_t * cr = (cairo_t*) buffer_context;
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);
}

int main (int argc, char ** argv)
{
    Xputty app;
    main_init(&app);
    Widget_t *w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 200);
    XStoreName(app.dpy, w->widget, "Hello world");
    w->func.expose_callback = draw_window;
    main_run(&app);
    main_quit(&app);
    return 0;
}

```

check out the example folder for more examples.

## Documentation:

[Documentation](https://brummer10.github.io/Xputty/html/index.html)

## License

         0BSD 
BSD Zero Clause License
