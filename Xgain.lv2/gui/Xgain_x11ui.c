
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

// xwidgets.h includes xputty.h and all defined widgets from Xputty
#include "xwidgets.h"

#include "./Xgain.h"

// png's linked in as binarys
EXTLD(pedal_png)
EXTLD(pswitch_png)
EXTLD(knob_png)

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                define controller numbers
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#define CONTROLS 3

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                the main LV2 handle->XWindow
-----------------------------------------------------------------------
----------------------------------------------------------------------*/


// main window struct
typedef struct {
    void *parentXwindow;
    Xputty main;
    Widget_t *win;
    Widget_t *widget[CONTROLS];

    void *controller;
    LV2UI_Write_Function write_function;
    LV2UI_Resize* resize;
} X11_UI;

// draw the window
static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    widget_set_scale(w);
    cairo_set_source_surface (w->crb, w->image,0,0);
    cairo_paint (w->crb);
    widget_reset_scale(w);
}

// if controller value changed send notify to host
static void value_changed(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    X11_UI* ui = (X11_UI*)w->parent_struct;
    ui->write_function(ui->controller,w->data,sizeof(float),0,&w->adj->value);
}

// init the xwindow and return the LV2UI handle
static LV2UI_Handle instantiate(const struct _LV2UI_Descriptor * descriptor,
            const char * plugin_uri, const char * bundle_path,
            LV2UI_Write_Function write_function,
            LV2UI_Controller controller, LV2UI_Widget * widget,
            const LV2_Feature * const * features) {

    X11_UI* ui = (X11_UI*)malloc(sizeof(X11_UI));

    if (!ui) {
        fprintf(stderr,"ERROR: failed to instantiate plugin with URI %s\n", plugin_uri);
        return NULL;
    }

    ui->parentXwindow = 0;
    LV2UI_Resize* resize = NULL;

    int i = 0;
    for (; features[i]; ++i) {
        if (!strcmp(features[i]->URI, LV2_UI__parent)) {
            ui->parentXwindow = features[i]->data;
        } else if (!strcmp(features[i]->URI, LV2_UI__resize)) {
            resize = (LV2UI_Resize*)features[i]->data;
        }
    }

    if (ui->parentXwindow == NULL)  {
        fprintf(stderr, "ERROR: Failed to open parentXwindow for %s\n", plugin_uri);
        free(ui);
        return NULL;
    }
    // init Xputty
    main_init(&ui->main);
    // create the toplevel Window on the parentXwindow provided by the host
    ui->win = create_window(&ui->main, (Window)ui->parentXwindow, 0, 0, 230, 200);
    // setup a background image to use on the toplevel window
    widget_get_png(ui->win, LDVAR(pedal_png));
    // connect the expose func
    ui->win->func.expose_callback = draw_window;
    // create a toggle button
    ui->widget[0] = add_toggle_button(ui->win, "Power", 20, 60, 80, 90);
    // setup a image to use for the toggle button
    widget_get_png(ui->widget[0], LDVAR(pswitch_png));
    // set resize mode for the toggle button to Aspect ratio
    ui->widget[0]->scale.gravity = ASPECT;
    // store the Port Index in the Widget_t data field
    ui->widget[0]->data = BYPASS;
    // store a pointer to the X11_UI struct in the parent_struct Widget_t field
    ui->widget[0]->parent_struct = ui;
    // connect the value changed callback with the write_function
    ui->widget[0]->func.value_changed_callback = value_changed;
    // create a knob widget
    ui->widget[1] = add_knob(ui->win, "Gain", 100, 60, 80, 90);
    // setup a image to be used for the knob
    widget_get_png(ui->widget[1], LDVAR(knob_png));
    // store the port index in the Widget_t data field
    ui->widget[1]->data = GAIN;
    // store a pointer to the X11_UI struct in the parent_struct Widget_t field
    ui->widget[1]->parent_struct = ui;
    // set the knob adjustment to the needed range
    set_adjustment(ui->widget[1]->adj,0.0, 0.0, -40.0, 40.0, 0.1, CL_CONTINUOS);
    // connect the value changed callback with the write_function
    ui->widget[1]->func.value_changed_callback = value_changed;
    // create a meter widget
    ui->widget[2] = add_vmeter(ui->win, "Meter", 190, 20, 10, 160);
    // store the port index in the Widget_t data field
    ui->widget[2]->data = METER;
    // finally map all Widgets on screen
    widget_show_all(ui->win);
    // set the widget pointer to the X11 Window from the toplevel Widget_t
    *widget = (void*)ui->win->widget;
    // request to resize the parentXwindow to the size of the toplevel Widget_t
    if (resize){
        ui->resize = resize;
        resize->ui_resize(resize->handle, 230, 200);
    }
    // store pointer to the host controller
    ui->controller = controller;
    // store pointer to the host write function
    ui->write_function = write_function;
    
    return (LV2UI_Handle)ui;
}

// cleanup after usage
static void cleanup(LV2UI_Handle handle) {
    X11_UI* ui = (X11_UI*)handle;
    // Xputty free all memory used
    main_quit(&ui->main);
    free(ui);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                        LV2 interface
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

// port value change message from host
static void port_event(LV2UI_Handle handle, uint32_t port_index,
                        uint32_t buffer_size, uint32_t format,
                        const void * buffer) {
    X11_UI* ui = (X11_UI*)handle;
    float value = *(float*)buffer;
    int i=0;
    for (;i<CONTROLS;i++) {
        if (port_index == ui->widget[i]->data) {
            // case port is METER, convert value to meter deflection
            if (port_index == METER) value = power2db(ui->widget[i], value);
            // Xputty check if the new value differs from the old one
            // and set new one, when needed
            check_value_changed(ui->widget[i]->adj, &value);
        }
    }
}

// LV2 idle interface to host
static int ui_idle(LV2UI_Handle handle) {
    X11_UI* ui = (X11_UI*)handle;
    // Xputty event loop setup to run one cycle when called
    run_embedded(&ui->main);
    return 0;
}

// LV2 resize interface to host
static int ui_resize(LV2UI_Feature_Handle handle, int w, int h) {
    X11_UI* ui = (X11_UI*)handle;
    // Xputty sends configure event to the toplevel widget to resize itself
    if (ui) send_configure_event(ui->win,0, 0, w, h);
    return 0;
}

// connect idle and resize functions to host
static const void* extension_data(const char* uri) {
    static const LV2UI_Idle_Interface idle = { ui_idle };
    static const LV2UI_Resize resize = { 0 ,ui_resize };
    if (!strcmp(uri, LV2_UI__idleInterface)) {
        return &idle;
    }
    if (!strcmp(uri, LV2_UI__resize)) {
        return &resize;
    }
    return NULL;
}

static const LV2UI_Descriptor descriptor = {
    PLUGIN_UI_URI,
    instantiate,
    cleanup,
    port_event,
    extension_data
};


LV2_SYMBOL_EXPORT
const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index) {
    switch (index) {
        case 0:
            return &descriptor;
        default:
        return NULL;
    }
}

