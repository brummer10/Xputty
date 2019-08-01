# Xputty


A damn tiny abstraction Layer to create X11 window/widgets with cairo surfaces

## Features

- easy creation of widgets and windows within the xlib windows system
- easy handling of multiple windows including multiple widgets
- easy to use main struct to handle the lifetime of all widgets and windows
    - Xputty main;
    - main_init(&main);
    - create_windows();
    - main_run(&main);
    - main_quit(&main);
- easy connection to event handlers by simply overwriting the defaults with you own handlers
- double buffered cairo surfaces to enable transparent drawing on child widgets
- easy to use x/y adjustments to create your own controller widgets like sliders, knobs, buttons or a trackball
- full documented API [Documentation](https://brummer10.github.io/Xputty/html/index.html)
- static linking to create position independent applications


Here is the usual hello world:

![simple-example](https://github.com/brummer10/Xputty/raw/master/examples/simple-example.png)

produced  by this code:

```C

#include "xputty.h"

/** your own expose function */
static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    cairo_set_source_rgb (w->crb, 1, 1, 1);
    cairo_paint (w->crb);
}

int main (int argc, char ** argv)
{
    /** acces the main struct */
    Xputty app;
    /** init the main struct */
    main_init(&app);
    /** create a Window on default root window */
    Widget_t *w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 200);
    /** acces Xlib function */
    XStoreName(app.dpy, w->widget, "Hello world");
    /** overwrite event handler with your own */
    w->func.expose_callback = draw_window;
    /** map the Window to display */
    widget_show_all(w);
    /** run the event loop */
    main_run(&app);
    /** clean up after the event loop is finished */
    main_quit(&app);
    return 0;
}

```

check out the example folder for more examples.


## License

         0BSD 
BSD Zero Clause License
