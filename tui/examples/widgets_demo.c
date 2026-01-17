/*
 * widgets_demo.c - Comprehensive TUI Widget Demo
 *
 * A complete demonstration of all widgets and features,
 * similar to Dear ImGui's demo window.
 */

#define TUI_IMPLEMENTATION
#include "../tui.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifdef _WIN32
    #include <windows.h>
    #define sleep_ms(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
#endif

/* ============================================================================
 * Global State
 * ============================================================================ */

static bool g_running = true;
static char g_status_text[256] = "Welcome! Use Tab to navigate, Arrow keys to adjust, Enter/Space to activate.";
static int g_current_tab = 0;

/* Tab content panels */
static tui_widget* g_tab_panels[4] = {NULL};

/* Demo state */
static char g_textbox_buf[128] = "Edit this text...";
static char g_multiline_buf[256] = "Line 1\nLine 2\nLine 3";
static bool g_checkbox_values[5] = {true, false, true, false, true};
static int g_radio_value = 0;
static int g_radio_value2 = 1;
static float g_slider_values[4] = {0.5f, 0.25f, 0.75f, 1.0f};
static float g_progress_value = 0.0f;
static int g_list_selected = 0;
static int g_spinner_value = 42;
static int g_dropdown_selected = 0;
static int g_counter = 0;

/* ============================================================================
 * Event Handlers
 * ============================================================================ */

static void on_escape(tui_widget* widget, tui_widget_event* event, void* userdata) {
    (void)widget; (void)userdata;
    if (event->base.type == TUI_EVENT_KEY && event->base.key == TUI_KEY_ESC) {
        g_running = false;
        tui_widget_event_consume(event);
    }
}

static void on_tab_change(tui_widget* widget, tui_widget_event* event, void* userdata) {
    (void)userdata;
    if (event->base.type == TUI_EVENT_KEY || event->base.type == TUI_EVENT_MOUSE) {
        int new_tab = widget->state.tabs.selected;
        if (new_tab != g_current_tab) {
            /* Hide old panel, show new panel */
            if (g_tab_panels[g_current_tab]) {
                g_tab_panels[g_current_tab]->visible = false;
            }
            if (g_tab_panels[new_tab]) {
                g_tab_panels[new_tab]->visible = true;
            }
            g_current_tab = new_tab;
            snprintf(g_status_text, sizeof(g_status_text), "Switched to tab: %s", 
                     widget->state.tabs.labels[new_tab]);
        }
    }
}

static void on_button_click(tui_widget* widget, tui_widget_event* event, void* userdata) {
    (void)userdata;
    bool activated = (event->base.type == TUI_EVENT_KEY && 
        (event->base.key == TUI_KEY_ENTER || event->base.key == TUI_KEY_SPACE)) ||
        (event->base.type == TUI_EVENT_MOUSE && event->base.mouse_button == TUI_MOUSE_LEFT);
    
    if (activated) {
        snprintf(g_status_text, sizeof(g_status_text), "Button clicked: %s", 
                 widget->state.button.text);
    }
}

static void on_increment(tui_widget* widget, tui_widget_event* event, void* userdata) {
    (void)widget; (void)userdata;
    bool activated = (event->base.type == TUI_EVENT_KEY && 
        (event->base.key == TUI_KEY_ENTER || event->base.key == TUI_KEY_SPACE)) ||
        (event->base.type == TUI_EVENT_MOUSE && event->base.mouse_button == TUI_MOUSE_LEFT);
    
    if (activated) {
        g_counter++;
        snprintf(g_status_text, sizeof(g_status_text), "Counter: %d", g_counter);
    }
}

static void on_decrement(tui_widget* widget, tui_widget_event* event, void* userdata) {
    (void)widget; (void)userdata;
    bool activated = (event->base.type == TUI_EVENT_KEY && 
        (event->base.key == TUI_KEY_ENTER || event->base.key == TUI_KEY_SPACE)) ||
        (event->base.type == TUI_EVENT_MOUSE && event->base.mouse_button == TUI_MOUSE_LEFT);
    
    if (activated) {
        g_counter--;
        snprintf(g_status_text, sizeof(g_status_text), "Counter: %d", g_counter);
    }
}

static void on_reset(tui_widget* widget, tui_widget_event* event, void* userdata) {
    (void)widget; (void)userdata;
    bool activated = (event->base.type == TUI_EVENT_KEY && 
        (event->base.key == TUI_KEY_ENTER || event->base.key == TUI_KEY_SPACE)) ||
        (event->base.type == TUI_EVENT_MOUSE && event->base.mouse_button == TUI_MOUSE_LEFT);
    
    if (activated) {
        g_counter = 0;
        g_progress_value = 0.0f;
        for (int i = 0; i < 4; i++) g_slider_values[i] = 0.5f;
        snprintf(g_status_text, sizeof(g_status_text), "All values reset!");
    }
}

static void on_checkbox_change(tui_widget* widget, tui_widget_event* event, void* userdata) {
    int idx = (int)(intptr_t)userdata;
    bool activated = (event->base.type == TUI_EVENT_KEY && 
        (event->base.key == TUI_KEY_ENTER || event->base.key == TUI_KEY_SPACE)) ||
        (event->base.type == TUI_EVENT_MOUSE && event->base.mouse_button == TUI_MOUSE_LEFT);
    
    if (activated) {
        snprintf(g_status_text, sizeof(g_status_text), "Checkbox %d: %s -> %s", 
                 idx + 1, 
                 widget->state.checkbox.checked ? "ON" : "OFF",
                 widget->state.checkbox.checked ? "OFF" : "ON");
    }
}

static void on_slider_change(tui_widget* widget, tui_widget_event* event, void* userdata) {
    int idx = (int)(intptr_t)userdata;
    if (event->base.type == TUI_EVENT_KEY) {
        snprintf(g_status_text, sizeof(g_status_text), "Slider %d: %.0f%%", 
                 idx + 1, widget->state.slider.value * 100);
    }
}

static void on_list_select(tui_widget* widget, tui_widget_event* event, void* userdata) {
    (void)userdata;
    if (event->base.type == TUI_EVENT_KEY) {
        if (widget->state.list.items && widget->state.list.selected < widget->state.list.count) {
            snprintf(g_status_text, sizeof(g_status_text), "Selected: %s", 
                     widget->state.list.items[widget->state.list.selected]);
        }
    }
}

static void on_spinner_change(tui_widget* widget, tui_widget_event* event, void* userdata) {
    (void)userdata;
    if (event->base.type == TUI_EVENT_KEY) {
        snprintf(g_status_text, sizeof(g_status_text), "Spinner value: %d", 
                 widget->state.spinner.value);
    }
}

/* ============================================================================
 * Widget Creation Helpers (internal use only)
 * ============================================================================ */

static tui_widget* create_label(tui_widget* parent, int x, int y, const char* text, uint32_t color) {
    tui_widget* w = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(w, x, y, (int)strlen(text) + 1, 1);
    w->state.label.text = text;
    w->fg_color = color;
    tui_widget_add_child(parent, w);
    return w;
}

static tui_widget* create_button(tui_widget* parent, int x, int y, int width, const char* text, 
                                  tui_event_handler handler, void* data, int tab_idx) {
    tui_widget* w = tui_widget_create(TUI_WIDGET_BUTTON);
    tui_widget_set_bounds(w, x, y, width, 1);
    w->state.button.text = text;
    w->tab_index = tab_idx;
    if (handler) {
        tui_widget_on(w, TUI_EVENT_KEY, handler, data);
        tui_widget_on(w, TUI_EVENT_MOUSE, handler, data);
    }
    tui_widget_add_child(parent, w);
    return w;
}

static tui_widget* create_checkbox(tui_widget* parent, int x, int y, const char* text, 
                                    bool* value, tui_event_handler handler, void* data, int tab_idx) {
    tui_widget* w = tui_widget_create(TUI_WIDGET_CHECKBOX);
    tui_widget_set_bounds(w, x, y, (int)strlen(text) + 5, 1);
    w->state.checkbox.text = text;
    w->state.checkbox.checked = value ? *value : false;
    w->tab_index = tab_idx;
    if (handler) {
        tui_widget_on(w, TUI_EVENT_KEY, handler, data);
        tui_widget_on(w, TUI_EVENT_MOUSE, handler, data);
    }
    tui_widget_add_child(parent, w);
    return w;
}

static tui_widget* create_slider(tui_widget* parent, int x, int y, int width, 
                                  float* value, float min, float max, float step,
                                  tui_event_handler handler, void* data, int tab_idx) {
    tui_widget* w = tui_widget_create(TUI_WIDGET_SLIDER);
    tui_widget_set_bounds(w, x, y, width, 1);
    w->state.slider.value = value ? *value : 0.5f;
    w->state.slider.min = min;
    w->state.slider.max = max;
    w->state.slider.step = step;
    w->tab_index = tab_idx;
    if (handler) tui_widget_on(w, TUI_EVENT_KEY, handler, data);
    tui_widget_add_child(parent, w);
    return w;
}

/* ============================================================================
 * Tab Panel Creation
 * ============================================================================ */

static tui_widget* create_basic_widgets_tab(tui_widget* parent, int w, int h, int* tab_idx) {
    tui_widget* panel = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(panel, 0, 3, w, h - 5);
    tui_widget_add_child(parent, panel);
    
    /* Section: Text Input */
    create_label(panel, 1, 0, "=== Text Input ===", TUI_COLOR_YELLOW);
    
    create_label(panel, 1, 2, "Single line:", TUI_RGB(150, 150, 150));
    tui_widget* textbox = tui_widget_create(TUI_WIDGET_TEXTBOX);
    tui_widget_set_bounds(textbox, 15, 2, 35, 1);
    textbox->state.textbox.buffer = g_textbox_buf;
    textbox->state.textbox.capacity = sizeof(g_textbox_buf);
    textbox->state.textbox.length = (int)strlen(g_textbox_buf);
    textbox->state.textbox.cursor = textbox->state.textbox.length;
    textbox->tab_index = (*tab_idx)++;
    tui_widget_add_child(panel, textbox);
    
    /* Section: Buttons */
    create_label(panel, 1, 5, "=== Buttons ===", TUI_COLOR_YELLOW);
    
    create_button(panel, 1, 7, 10, "Button 1", on_button_click, NULL, (*tab_idx)++);
    create_button(panel, 13, 7, 10, "Button 2", on_button_click, NULL, (*tab_idx)++);
    create_button(panel, 25, 7, 10, "Button 3", on_button_click, NULL, (*tab_idx)++);
    
    /* Counter demo */
    create_label(panel, 1, 9, "Counter:", TUI_RGB(150, 150, 150));
    create_button(panel, 11, 9, 5, " - ", on_decrement, NULL, (*tab_idx)++);
    create_button(panel, 18, 9, 5, " + ", on_increment, NULL, (*tab_idx)++);
    create_button(panel, 25, 9, 9, "Reset", on_reset, NULL, (*tab_idx)++);
    
    /* Section: Checkboxes */
    create_label(panel, 1, 12, "=== Checkboxes ===", TUI_COLOR_YELLOW);
    
    create_checkbox(panel, 1, 14, "Feature A", &g_checkbox_values[0], 
                    on_checkbox_change, (void*)(intptr_t)0, (*tab_idx)++);
    create_checkbox(panel, 1, 15, "Feature B", &g_checkbox_values[1], 
                    on_checkbox_change, (void*)(intptr_t)1, (*tab_idx)++);
    create_checkbox(panel, 1, 16, "Feature C", &g_checkbox_values[2], 
                    on_checkbox_change, (void*)(intptr_t)2, (*tab_idx)++);
    create_checkbox(panel, 20, 14, "Option X", &g_checkbox_values[3], 
                    on_checkbox_change, (void*)(intptr_t)3, (*tab_idx)++);
    create_checkbox(panel, 20, 15, "Option Y", &g_checkbox_values[4], 
                    on_checkbox_change, (void*)(intptr_t)4, (*tab_idx)++);
    
    return panel;
}

static tui_widget* create_selection_widgets_tab(tui_widget* parent, int w, int h, int* tab_idx) {
    tui_widget* panel = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(panel, 0, 3, w, h - 5);
    panel->visible = false; /* Hidden by default */
    tui_widget_add_child(parent, panel);
    
    /* Section: Radio Buttons */
    create_label(panel, 1, 0, "=== Radio Buttons ===", TUI_COLOR_YELLOW);
    
    create_label(panel, 1, 2, "Group 1:", TUI_RGB(150, 150, 150));
    for (int i = 0; i < 3; i++) {
        static const char* labels[] = {"Radio Option A", "Radio Option B", "Radio Option C"};
        tui_widget* r = tui_widget_create(TUI_WIDGET_RADIO);
        tui_widget_set_bounds(r, 1, 3 + i, 20, 1);
        r->state.radio.text = labels[i];
        r->state.radio.group_value = &g_radio_value;
        r->state.radio.value = i;
        r->tab_index = (*tab_idx)++;
        tui_widget_add_child(panel, r);
    }
    
    create_label(panel, 25, 2, "Group 2:", TUI_RGB(150, 150, 150));
    for (int i = 0; i < 4; i++) {
        static const char* labels[] = {"Small", "Medium", "Large", "X-Large"};
        tui_widget* r = tui_widget_create(TUI_WIDGET_RADIO);
        tui_widget_set_bounds(r, 25, 3 + i, 15, 1);
        r->state.radio.text = labels[i];
        r->state.radio.group_value = &g_radio_value2;
        r->state.radio.value = i;
        r->tab_index = (*tab_idx)++;
        tui_widget_add_child(panel, r);
    }
    
    /* Section: List */
    create_label(panel, 1, 8, "=== List Selection ===", TUI_COLOR_YELLOW);
    
    static const char* fruits[] = {
        "Apple", "Banana", "Cherry", "Date", "Elderberry",
        "Fig", "Grape", "Honeydew", "Kiwi", "Lemon"
    };
    
    tui_widget* list = tui_widget_create(TUI_WIDGET_LIST);
    tui_widget_set_bounds(list, 1, 10, 20, 6);
    list->state.list.items = fruits;
    list->state.list.count = 10;
    list->state.list.selected = 0;
    list->state.list.scroll = 0;
    list->state.list.visible = 6;
    list->tab_index = (*tab_idx)++;
    tui_widget_on(list, TUI_EVENT_KEY, on_list_select, NULL);
    tui_widget_add_child(panel, list);
    
    create_label(panel, 1, 17, "Use Up/Down to navigate, Enter to select", TUI_RGB(100, 100, 100));
    
    /* Section: Spinner */
    create_label(panel, 25, 8, "=== Spinner ===", TUI_COLOR_YELLOW);
    
    tui_widget* spinner = tui_widget_create(TUI_WIDGET_SPINNER);
    tui_widget_set_bounds(spinner, 25, 10, 12, 1);
    spinner->state.spinner.value = g_spinner_value;
    spinner->state.spinner.min = 0;
    spinner->state.spinner.max = 100;
    spinner->state.spinner.step = 1;
    spinner->tab_index = (*tab_idx)++;
    tui_widget_on(spinner, TUI_EVENT_KEY, on_spinner_change, NULL);
    tui_widget_add_child(panel, spinner);
    
    create_label(panel, 25, 12, "Use Left/Right or", TUI_RGB(100, 100, 100));
    create_label(panel, 25, 13, "Up/Down to adjust", TUI_RGB(100, 100, 100));
    
    return panel;
}

static tui_widget* create_sliders_tab(tui_widget* parent, int w, int h, int* tab_idx) {
    tui_widget* panel = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(panel, 0, 3, w, h - 5);
    panel->visible = false;
    tui_widget_add_child(parent, panel);
    
    /* Section: Sliders */
    create_label(panel, 1, 0, "=== Sliders ===", TUI_COLOR_YELLOW);
    
    static const char* slider_labels[] = {"Volume", "Brightness", "Contrast", "Saturation"};
    for (int i = 0; i < 4; i++) {
        create_label(panel, 1, 2 + i * 2, slider_labels[i], TUI_RGB(150, 150, 150));
        create_slider(panel, 15, 2 + i * 2, 30, &g_slider_values[i], 
                      0.0f, 1.0f, 0.05f, on_slider_change, (void*)(intptr_t)i, (*tab_idx)++);
    }
    
    create_label(panel, 1, 11, "Use Left/Right arrows to adjust", TUI_RGB(100, 100, 100));
    
    /* Section: Progress Bars */
    create_label(panel, 1, 14, "=== Progress Bars ===", TUI_COLOR_YELLOW);
    
    create_label(panel, 1, 16, "Download:", TUI_RGB(150, 150, 150));
    tui_widget* progress1 = tui_widget_create(TUI_WIDGET_PROGRESS);
    tui_widget_set_bounds(progress1, 15, 16, 30, 1);
    progress1->state.progress.value = 0.0f;
    progress1->state.progress.min = 0.0f;
    progress1->state.progress.max = 1.0f;
    progress1->name = "progress1";
    tui_widget_add_child(panel, progress1);
    
    create_label(panel, 1, 18, "Upload:", TUI_RGB(150, 150, 150));
    tui_widget* progress2 = tui_widget_create(TUI_WIDGET_PROGRESS);
    tui_widget_set_bounds(progress2, 15, 18, 30, 1);
    progress2->state.progress.value = 0.65f;
    progress2->state.progress.min = 0.0f;
    progress2->state.progress.max = 1.0f;
    progress2->name = "progress2";
    tui_widget_add_child(panel, progress2);
    
    create_label(panel, 1, 20, "(Progress bars animate automatically)", TUI_RGB(100, 100, 100));
    
    return panel;
}

static tui_widget* create_about_tab(tui_widget* parent, int w, int h, int* tab_idx) {
    (void)tab_idx;
    tui_widget* panel = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(panel, 0, 3, w, h - 5);
    panel->visible = false;
    tui_widget_add_child(parent, panel);
    
    create_label(panel, 1, 1, "=== TUI Library ===", TUI_COLOR_YELLOW);
    create_label(panel, 1, 3, "A single-header terminal UI library for C11", TUI_COLOR_WHITE);
    create_label(panel, 1, 5, "Features:", TUI_COLOR_CYAN);
    create_label(panel, 3, 7, "* Hierarchical widget system", TUI_RGB(200, 200, 200));
    create_label(panel, 3, 8, "* Event bubbling & capture phases", TUI_RGB(200, 200, 200));
    create_label(panel, 3, 9, "* Tab navigation & focus management", TUI_RGB(200, 200, 200));
    create_label(panel, 3, 10, "* Mouse support", TUI_RGB(200, 200, 200));
    create_label(panel, 3, 11, "* Cross-platform (Windows/macOS/Linux)", TUI_RGB(200, 200, 200));
    create_label(panel, 3, 12, "* True color support (24-bit RGB)", TUI_RGB(200, 200, 200));
    
    create_label(panel, 1, 14, "Widgets:", TUI_COLOR_CYAN);
    create_label(panel, 3, 16, "Container, Panel, Label, Button, TextBox, CheckBox,", TUI_RGB(200, 200, 200));
    create_label(panel, 3, 17, "Radio, List, Progress, Slider, Spinner, Tabs, ScrollBar", TUI_RGB(200, 200, 200));
    
    create_label(panel, 1, 19, "Press ESC to exit", TUI_COLOR_RED);
    
    return panel;
}

/* ============================================================================
 * Main
 * ============================================================================ */

int main(void) {
    tui_context* ctx = tui_create();
    if (!ctx) return 1;
    
    tui_enable_mouse(ctx);
    
    tui_widget_manager wm;
    tui_wm_init(&wm);
    
    int width = tui_get_width(ctx);
    int height = tui_get_height(ctx);
    
    /* Root container */
    tui_widget* root = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(root, 0, 0, width, height);
    
    /* Main panel with border */
    tui_widget* main_panel = tui_widget_create(TUI_WIDGET_PANEL);
    tui_widget_set_bounds(main_panel, 1, 0, width - 2, height - 2);
    main_panel->has_border = true;
    main_panel->border_style = TUI_BORDER_DOUBLE;
    main_panel->fg_color = TUI_RGB(80, 120, 180);
    tui_widget_add_child(root, main_panel);
    
    /* Title */
    create_label(main_panel, 2, 0, " TUI Widget Demo ", TUI_COLOR_CYAN);
    
    /* Tab bar */
    static const char* tab_labels[] = {"Basic Widgets", "Selection", "Sliders & Progress", "About"};
    tui_widget* tabs = tui_widget_create(TUI_WIDGET_TABS);
    tui_widget_set_bounds(tabs, 1, 1, width - 6, 1);
    tabs->state.tabs.labels = tab_labels;
    tabs->state.tabs.count = 4;
    tabs->state.tabs.selected = 0;
    tabs->tab_index = 0;
    tui_widget_on(tabs, TUI_EVENT_KEY, on_tab_change, NULL);
    tui_widget_on(tabs, TUI_EVENT_MOUSE, on_tab_change, NULL);
    tui_widget_add_child(main_panel, tabs);
    
    /* Create tab panels */
    int tab_idx = 1;
    g_tab_panels[0] = create_basic_widgets_tab(main_panel, width - 4, height - 4, &tab_idx);
    g_tab_panels[1] = create_selection_widgets_tab(main_panel, width - 4, height - 4, &tab_idx);
    g_tab_panels[2] = create_sliders_tab(main_panel, width - 4, height - 4, &tab_idx);
    g_tab_panels[3] = create_about_tab(main_panel, width - 4, height - 4, &tab_idx);
    
    /* Status bar */
    tui_widget* status_bar = tui_widget_create(TUI_WIDGET_PANEL);
    tui_widget_set_bounds(status_bar, 1, height - 3, width - 2, 1);
    status_bar->has_border = false;
    status_bar->bg_color = TUI_RGB(40, 40, 60);
    tui_widget_add_child(root, status_bar);
    
    /* Setup */
    tui_wm_set_root(&wm, root);
    tui_wm_focus(&wm, tabs);
    tui_wm_register_hotkey(&wm, TUI_KEY_ESC, 0, false, false, false, on_escape, NULL);
    
    /* Find progress widgets for animation */
    tui_widget* progress1 = NULL;
    tui_widget* progress2 = NULL;
    for (int i = 0; i < g_tab_panels[2]->child_count; i++) {
        tui_widget* c = g_tab_panels[2]->children[i];
        if (c->name && strcmp(c->name, "progress1") == 0) progress1 = c;
        if (c->name && strcmp(c->name, "progress2") == 0) progress2 = c;
    }
    
    float anim_time = 0.0f;
    
    while (g_running) {
        tui_event event;
        while (tui_poll_event(ctx, &event)) {
            tui_wm_route_event(&wm, &event);
        }
        
        if (tui_resized(ctx)) {
            width = tui_get_width(ctx);
            height = tui_get_height(ctx);
            tui_widget_set_bounds(root, 0, 0, width, height);
            tui_widget_set_bounds(main_panel, 1, 0, width - 2, height - 2);
            tui_widget_set_bounds(status_bar, 1, height - 3, width - 2, 1);
            tui_widget_set_bounds(tabs, 1, 1, width - 6, 1);
        }
        
        /* Animate progress bars */
        anim_time += 0.016f;
        if (progress1) {
            progress1->state.progress.value = (float)(((int)(anim_time * 20) % 100) / 100.0f);
        }
        if (progress2) {
            float v = 0.5f + 0.5f * sinf(anim_time * 2.0f);
            progress2->state.progress.value = v;
        }
        
        /* Render */
        tui_begin_frame(ctx);
        tui_clear(ctx);
        
        tui_wm_draw(&wm, ctx);
        
        /* Draw counter value in basic widgets tab */
        if (g_current_tab == 0) {
            char counter_str[32];
            snprintf(counter_str, sizeof(counter_str), "[%d]", g_counter);
            tui_set_fg(ctx, TUI_COLOR_WHITE);
            tui_set_bg(ctx, TUI_COLOR_DEFAULT);
            tui_label(ctx, 18, 12, counter_str);
        }
        
        /* Status bar text */
        tui_set_fg(ctx, TUI_COLOR_WHITE);
        tui_set_bg(ctx, TUI_RGB(40, 40, 60));
        tui_label(ctx, 3, height - 2, g_status_text);
        
        /* Focus indicator */
        tui_widget* focused = tui_wm_get_focus(&wm);
        if (focused && focused->name) {
            char focus_str[64];
            snprintf(focus_str, sizeof(focus_str), "Focus: %s", focused->name);
            tui_set_fg(ctx, TUI_COLOR_YELLOW);
            tui_label(ctx, width - 25, height - 2, focus_str);
        }
        
        tui_end_frame(ctx);
        
        sleep_ms(16);
    }
    
    tui_widget_destroy_recursive(root);
    tui_disable_mouse(ctx);
    tui_destroy(ctx);
    
    return 0;
}
