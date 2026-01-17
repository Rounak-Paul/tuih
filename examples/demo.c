/*
 * demo.c - TUI Library Complete Demo
 *
 * Demonstrates all features of the TUI library:
 * - All widget types (Label, Button, TextBox, TextArea, Checkbox, Radio,
 *   List, Progress, Slider, Spinner, Dropdown, Tabs, Scrollbar, Splitter)
 * - Modifier keys (Ctrl, Alt, Shift)
 * - Mouse support (clicks, scroll, drag)
 * - Tab navigation and focus management
 * - Event bubbling and hotkeys
 * - Theming and styling
 *
 * Controls:
 *   Tab/Shift+Tab  - Navigate between widgets
 *   Arrow keys     - Adjust values / navigate lists
 *   Enter/Space    - Activate buttons / toggle checkboxes
 *   Escape         - Exit
 *   Mouse          - Click, scroll, drag splitter
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
 * State
 * ============================================================================ */

static bool g_running = true;
static char g_status[256] = "Welcome! Tab to navigate, Arrow keys to adjust, Enter/Space to activate.";

/* Widget state */
static char g_textbox_buf[64] = "Edit me...";
static bool g_checkboxes[3] = {true, false, true};
static int g_radio_value = 0;
static float g_slider_val = 0.5f;
static int g_spinner_val = 42;
static int g_click_count = 0;

/* Editable textarea buffer */
#define MAX_EDITOR_LINES 100
#define MAX_LINE_LEN 256
static char* g_editor_lines[MAX_EDITOR_LINES];
static int g_editor_line_count = 0;

static void init_editor_lines(void) {
    static const char* initial[] = {
        "#include <stdio.h>",
        "",
        "int main(void) {",
        "    printf(\"Hello, TUI!\\n\");",
        "    return 0;",
        "}",
        "",
        "// This is an editable code editor!",
        "// Type to insert text",
        "// Backspace/Delete to remove",
        "// Enter to create new lines",
        "// Tab inserts 4 spaces"
    };
    g_editor_line_count = 12;
    for (int i = 0; i < g_editor_line_count; i++) {
        g_editor_lines[i] = (char*)malloc(MAX_LINE_LEN);
        strncpy(g_editor_lines[i], initial[i], MAX_LINE_LEN - 1);
        g_editor_lines[i][MAX_LINE_LEN - 1] = '\0';
    }
}

static void free_editor_lines(void) {
    for (int i = 0; i < g_editor_line_count; i++) {
        free(g_editor_lines[i]);
    }
}

/* List items */
static const char* g_list_items[] = {
    "Apple", "Banana", "Cherry", "Date", "Elderberry",
    "Fig", "Grape", "Honeydew", "Kiwi", "Lemon"
};

/* Tab labels */
static const char* g_tab_labels[] = {"Widgets", "Text", "About"};

/* Tab panels */
static tui_widget* g_panels[3] = {NULL};
static int g_current_tab = 0;

/* ============================================================================
 * Event Handlers
 * ============================================================================ */

static void on_escape(tui_widget* w, tui_widget_event* e, void* data) {
    (void)w; (void)data;
    if (e->base.type == TUI_EVENT_KEY && e->base.key == TUI_KEY_ESC) {
        g_running = false;
        tui_widget_event_consume(e);
    }
}

static void on_tab_change(tui_widget* w, tui_widget_event* e, void* data) {
    (void)data;
    if (e->base.type == TUI_EVENT_KEY || e->base.type == TUI_EVENT_MOUSE) {
        int new_tab = w->state.tabs.selected;
        if (new_tab != g_current_tab) {
            if (g_panels[g_current_tab]) g_panels[g_current_tab]->visible = false;
            if (g_panels[new_tab]) g_panels[new_tab]->visible = true;
            g_current_tab = new_tab;
            snprintf(g_status, sizeof(g_status), "Switched to: %s", g_tab_labels[new_tab]);
        }
    }
}

static void on_button_click(tui_widget* w, tui_widget_event* e, void* data) {
    (void)data;
    bool clicked = (e->base.type == TUI_EVENT_KEY && 
        (e->base.key == TUI_KEY_ENTER || e->base.key == TUI_KEY_SPACE)) ||
        (e->base.type == TUI_EVENT_MOUSE && e->base.mouse_button == TUI_MOUSE_LEFT);
    
    if (clicked) {
        g_click_count++;
        snprintf(g_status, sizeof(g_status), "Button '%s' clicked! (total: %d)", 
                 w->state.button.text, g_click_count);
    }
}

static void on_checkbox_toggle(tui_widget* w, tui_widget_event* e, void* data) {
    int idx = (int)(intptr_t)data;
    bool toggle = (e->base.type == TUI_EVENT_KEY && 
        (e->base.key == TUI_KEY_ENTER || e->base.key == TUI_KEY_SPACE)) ||
        (e->base.type == TUI_EVENT_MOUSE && e->base.mouse_button == TUI_MOUSE_LEFT);
    
    if (toggle) {
        snprintf(g_status, sizeof(g_status), "Checkbox %d: %s", idx + 1,
                 w->state.checkbox.checked ? "OFF → ON" : "ON → OFF");
    }
}

static void on_slider_change(tui_widget* w, tui_widget_event* e, void* data) {
    (void)data;
    if (e->base.type == TUI_EVENT_KEY) {
        snprintf(g_status, sizeof(g_status), "Slider: %.0f%%", w->state.slider.value * 100);
    }
}

static void on_list_select(tui_widget* w, tui_widget_event* e, void* data) {
    (void)data;
    if (e->base.type == TUI_EVENT_KEY && w->state.list.items) {
        snprintf(g_status, sizeof(g_status), "Selected: %s", 
                 w->state.list.items[w->state.list.selected]);
    }
}

static void on_spinner_change(tui_widget* w, tui_widget_event* e, void* data) {
    (void)data;
    if (e->base.type == TUI_EVENT_KEY) {
        snprintf(g_status, sizeof(g_status), "Spinner: %d", w->state.spinner.value);
    }
}

/* ============================================================================
 * Widget Creation
 * ============================================================================ */

static tui_widget* create_widgets_panel(tui_widget* parent, int w, int h, int* tab_idx) {
    tui_widget* panel = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(panel, 0, 3, w, h - 5);
    tui_widget_add_child(parent, panel);
    
    int y = 0;
    
    /* Buttons section */
    tui_widget* lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, 1, y, 20, 1);
    lbl->state.label.text = "=== Buttons ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(panel, lbl);
    y += 2;
    
    for (int i = 0; i < 3; i++) {
        static const char* btn_texts[] = {"Click Me", "Button 2", "Button 3"};
        tui_widget* btn = tui_widget_create(TUI_WIDGET_BUTTON);
        tui_widget_set_bounds(btn, 1 + i * 12, y, 10, 1);
        btn->state.button.text = btn_texts[i];
        btn->tab_index = (*tab_idx)++;
        tui_widget_on(btn, TUI_EVENT_KEY, on_button_click, NULL);
        tui_widget_on(btn, TUI_EVENT_MOUSE, on_button_click, NULL);
        tui_widget_add_child(panel, btn);
    }
    y += 2;
    
    /* TextBox section */
    lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, 1, y, 20, 1);
    lbl->state.label.text = "=== TextBox ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(panel, lbl);
    y += 2;
    
    tui_widget* textbox = tui_widget_create(TUI_WIDGET_TEXTBOX);
    tui_widget_set_bounds(textbox, 1, y, 30, 1);
    textbox->state.textbox.buffer = g_textbox_buf;
    textbox->state.textbox.capacity = sizeof(g_textbox_buf);
    textbox->state.textbox.length = (int)strlen(g_textbox_buf);
    textbox->state.textbox.cursor = textbox->state.textbox.length;
    textbox->tab_index = (*tab_idx)++;
    tui_widget_add_child(panel, textbox);
    y += 2;
    
    /* Checkboxes */
    lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, 1, y, 20, 1);
    lbl->state.label.text = "=== Checkboxes ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(panel, lbl);
    y += 2;
    
    for (int i = 0; i < 3; i++) {
        static const char* cb_texts[] = {"Option A", "Option B", "Option C"};
        tui_widget* cb = tui_widget_create(TUI_WIDGET_CHECKBOX);
        tui_widget_set_bounds(cb, 1 + i * 14, y, 12, 1);
        cb->state.checkbox.text = cb_texts[i];
        cb->state.checkbox.checked = g_checkboxes[i];
        cb->tab_index = (*tab_idx)++;
        tui_widget_on(cb, TUI_EVENT_KEY, on_checkbox_toggle, (void*)(intptr_t)i);
        tui_widget_on(cb, TUI_EVENT_MOUSE, on_checkbox_toggle, (void*)(intptr_t)i);
        tui_widget_add_child(panel, cb);
    }
    y += 2;
    
    /* Radio buttons */
    lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, 1, y, 20, 1);
    lbl->state.label.text = "=== Radio ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(panel, lbl);
    y += 2;
    
    for (int i = 0; i < 3; i++) {
        static const char* radio_texts[] = {"Small", "Medium", "Large"};
        tui_widget* r = tui_widget_create(TUI_WIDGET_RADIO);
        tui_widget_set_bounds(r, 1, y + i, 12, 1);
        r->state.radio.text = radio_texts[i];
        r->state.radio.group_value = &g_radio_value;
        r->state.radio.value = i;
        r->tab_index = (*tab_idx)++;
        tui_widget_add_child(panel, r);
    }
    
    /* Right column: Slider, Spinner, List */
    int rx = 38;
    int ry = 0;
    
    lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, rx, ry, 20, 1);
    lbl->state.label.text = "=== Slider ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(panel, lbl);
    ry += 2;
    
    tui_widget* slider = tui_widget_create(TUI_WIDGET_SLIDER);
    tui_widget_set_bounds(slider, rx, ry, 25, 1);
    slider->state.slider.value = g_slider_val;
    slider->state.slider.min = 0.0f;
    slider->state.slider.max = 1.0f;
    slider->state.slider.step = 0.05f;
    slider->tab_index = (*tab_idx)++;
    tui_widget_on(slider, TUI_EVENT_KEY, on_slider_change, NULL);
    tui_widget_add_child(panel, slider);
    ry += 2;
    
    lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, rx, ry, 20, 1);
    lbl->state.label.text = "=== Spinner ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(panel, lbl);
    ry += 2;
    
    tui_widget* spinner = tui_widget_create(TUI_WIDGET_SPINNER);
    tui_widget_set_bounds(spinner, rx, ry, 12, 1);
    spinner->state.spinner.value = g_spinner_val;
    spinner->state.spinner.min = 0;
    spinner->state.spinner.max = 100;
    spinner->state.spinner.step = 1;
    spinner->tab_index = (*tab_idx)++;
    tui_widget_on(spinner, TUI_EVENT_KEY, on_spinner_change, NULL);
    tui_widget_add_child(panel, spinner);
    ry += 2;
    
    lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, rx, ry, 20, 1);
    lbl->state.label.text = "=== List ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(panel, lbl);
    ry += 2;
    
    tui_widget* list = tui_widget_create(TUI_WIDGET_LIST);
    tui_widget_set_bounds(list, rx, ry, 18, 5);
    list->state.list.items = g_list_items;
    list->state.list.count = 10;
    list->state.list.selected = 0;
    list->state.list.scroll = 0;
    list->state.list.visible = 5;
    list->tab_index = (*tab_idx)++;
    tui_widget_on(list, TUI_EVENT_KEY, on_list_select, NULL);
    tui_widget_add_child(panel, list);
    ry += 6;
    
    /* Progress bar */
    lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, rx, ry, 20, 1);
    lbl->state.label.text = "=== Progress ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(panel, lbl);
    ry += 2;
    
    tui_widget* progress = tui_widget_create(TUI_WIDGET_PROGRESS);
    tui_widget_set_bounds(progress, rx, ry, 25, 1);
    progress->state.progress.value = 0.0f;
    progress->state.progress.min = 0.0f;
    progress->state.progress.max = 1.0f;
    progress->name = "progress";
    tui_widget_add_child(panel, progress);
    
    return panel;
}

static tui_widget* create_text_panel(tui_widget* parent, int w, int h, int* tab_idx) {
    tui_widget* panel = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(panel, 0, 3, w, h - 5);
    panel->visible = false;
    tui_widget_add_child(parent, panel);
    
    /* Splitter with textarea and info panel */
    tui_widget* splitter = tui_widget_create(TUI_WIDGET_SPLITTER);
    tui_widget_set_bounds(splitter, 0, 0, w, h - 6);
    splitter->state.splitter.vertical = false;
    splitter->state.splitter.ratio = 0.65f;
    splitter->state.splitter.min_size = 15;
    tui_widget_add_child(panel, splitter);
    
    /* Left: Textarea (editable code editor) */
    tui_widget* textarea = tui_widget_create(TUI_WIDGET_TEXTAREA);
    tui_widget_set_bounds(textarea, 0, 0, (int)(w * 0.65f), h - 6);
    textarea->state.textarea.lines = g_editor_lines;
    textarea->state.textarea.line_count = g_editor_line_count;
    textarea->state.textarea.line_capacity = MAX_EDITOR_LINES;
    textarea->state.textarea.max_line_len = MAX_LINE_LEN;
    textarea->state.textarea.cursor_row = 0;
    textarea->state.textarea.cursor_col = 0;
    textarea->state.textarea.line_numbers = true;
    textarea->state.textarea.editable = true;  /* Enable editing! */
    textarea->tab_index = (*tab_idx)++;
    textarea->name = "editor";
    tui_widget_add_child(splitter, textarea);
    
    /* Right: Info panel */
    tui_widget* info = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(info, (int)(w * 0.65f) + 1, 0, (int)(w * 0.35f) - 1, h - 6);
    tui_widget_add_child(splitter, info);
    
    tui_widget* lbl = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(lbl, 1, 0, 22, 1);
    lbl->state.label.text = "=== Code Editor ===";
    lbl->fg_color = TUI_COLOR_YELLOW;
    tui_widget_add_child(info, lbl);
    
    static const char* help_lines[] = {
        "Type to insert text",
        "Backspace: Delete left",
        "Delete: Delete right",
        "Enter: New line",
        "Tab: Insert 4 spaces",
        "",
        "Arrows: Move cursor",
        "Ctrl+Home: Top",
        "Ctrl+End: Bottom",
        "PageUp/Down: Scroll",
        "",
        "Drag splitter to resize"
    };
    
    for (int i = 0; i < 12; i++) {
        lbl = tui_widget_create(TUI_WIDGET_LABEL);
        tui_widget_set_bounds(lbl, 1, 2 + i, 24, 1);
        lbl->state.label.text = help_lines[i];
        lbl->fg_color = TUI_RGB(150, 150, 150);
        tui_widget_add_child(info, lbl);
    }
    
    return panel;
}

static tui_widget* create_about_panel(tui_widget* parent, int w, int h, int* tab_idx) {
    (void)tab_idx;
    tui_widget* panel = tui_widget_create(TUI_WIDGET_CONTAINER);
    tui_widget_set_bounds(panel, 0, 3, w, h - 5);
    panel->visible = false;
    tui_widget_add_child(parent, panel);
    
    static const char* lines[] = {
        "=== TUI Library ===",
        "",
        "A single-header terminal UI library for C11.",
        "",
        "Features:",
        "  * Hierarchical widget system",
        "  * Event bubbling (Capture → Target → Bubble)",
        "  * Tab navigation & focus management",
        "  * Mouse support (click, scroll, drag)",
        "  * Modifier keys (Ctrl, Alt, Shift)",
        "  * True color (24-bit RGB)",
        "  * Cross-platform (Windows, macOS, Linux)",
        "",
        "Widgets:",
        "  Container, Panel, Label, Button, TextBox,",
        "  TextArea, Checkbox, Radio, List, Progress,",
        "  Slider, Spinner, Dropdown, Tabs, Scrollbar,",
        "  Splitter",
        "",
        "License: Public Domain / MIT",
        "",
        "Press ESC to exit."
    };
    
    for (int i = 0; i < 22; i++) {
        tui_widget* lbl = tui_widget_create(TUI_WIDGET_LABEL);
        tui_widget_set_bounds(lbl, 2, i, 60, 1);
        lbl->state.label.text = lines[i];
        if (i == 0) {
            lbl->fg_color = TUI_COLOR_CYAN;
        } else if (strncmp(lines[i], "  *", 3) == 0 || strncmp(lines[i], "  C", 3) == 0) {
            lbl->fg_color = TUI_RGB(200, 200, 200);
        } else if (strcmp(lines[i], "Features:") == 0 || strcmp(lines[i], "Widgets:") == 0) {
            lbl->fg_color = TUI_COLOR_YELLOW;
        } else if (strncmp(lines[i], "Press", 5) == 0) {
            lbl->fg_color = TUI_COLOR_RED;
        } else {
            lbl->fg_color = TUI_COLOR_WHITE;
        }
        tui_widget_add_child(panel, lbl);
    }
    
    return panel;
}

/* ============================================================================
 * Main
 * ============================================================================ */

int main(void) {
    /* Initialize editable textarea buffer */
    init_editor_lines();
    
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
    
    /* Main panel */
    tui_widget* main_panel = tui_widget_create(TUI_WIDGET_PANEL);
    tui_widget_set_bounds(main_panel, 1, 0, width - 2, height - 2);
    main_panel->has_border = true;
    main_panel->border_style = TUI_BORDER_DOUBLE;
    main_panel->fg_color = TUI_RGB(80, 120, 180);
    tui_widget_add_child(root, main_panel);
    
    /* Title */
    tui_widget* title = tui_widget_create(TUI_WIDGET_LABEL);
    tui_widget_set_bounds(title, 2, 0, 25, 1);
    title->state.label.text = " TUI Library Demo ";
    title->fg_color = TUI_COLOR_CYAN;
    tui_widget_add_child(main_panel, title);
    
    /* Tab bar */
    tui_widget* tabs = tui_widget_create(TUI_WIDGET_TABS);
    tui_widget_set_bounds(tabs, 1, 1, width - 6, 1);
    tabs->state.tabs.labels = g_tab_labels;
    tabs->state.tabs.count = 3;
    tabs->state.tabs.selected = 0;
    tabs->tab_index = 0;
    tui_widget_on(tabs, TUI_EVENT_KEY, on_tab_change, NULL);
    tui_widget_on(tabs, TUI_EVENT_MOUSE, on_tab_change, NULL);
    tui_widget_add_child(main_panel, tabs);
    
    /* Create tab panels */
    int tab_idx = 1;
    g_panels[0] = create_widgets_panel(main_panel, width - 4, height - 4, &tab_idx);
    g_panels[1] = create_text_panel(main_panel, width - 4, height - 4, &tab_idx);
    g_panels[2] = create_about_panel(main_panel, width - 4, height - 4, &tab_idx);
    
    /* Status bar */
    tui_widget* status_bar = tui_widget_create(TUI_WIDGET_PANEL);
    tui_widget_set_bounds(status_bar, 1, height - 3, width - 2, 1);
    status_bar->bg_color = TUI_RGB(40, 40, 60);
    tui_widget_add_child(root, status_bar);
    
    /* Setup */
    tui_wm_set_root(&wm, root);
    tui_wm_focus(&wm, tabs);
    tui_wm_register_hotkey(&wm, TUI_KEY_ESC, 0, false, false, false, on_escape, NULL);
    
    /* Find progress widget for animation */
    tui_widget* progress = tui_widget_find_by_name(root, "progress");
    float anim_time = 0.0f;
    
    while (g_running) {
        tui_event event;
        while (tui_poll_event(ctx, &event)) {
            /* Show modifier keys in status */
            if (event.type == TUI_EVENT_KEY && (event.ctrl || event.alt || event.shift)) {
                char mod_str[64] = "";
                if (event.ctrl) strcat(mod_str, "Ctrl+");
                if (event.alt) strcat(mod_str, "Alt+");
                if (event.shift) strcat(mod_str, "Shift+");
                snprintf(g_status, sizeof(g_status), "Key: %s (modifiers: %s)", 
                         event.key == TUI_KEY_CHAR ? "CHAR" : "Special", mod_str);
            }
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
        
        /* Animate progress */
        anim_time += 0.016f;
        if (progress) {
            progress->state.progress.value = (float)(((int)(anim_time * 30) % 100) / 100.0f);
        }
        
        /* Render */
        tui_begin_frame(ctx);
        tui_clear(ctx);
        tui_wm_draw(&wm, ctx);
        
        /* Status bar text */
        tui_set_fg(ctx, TUI_COLOR_WHITE);
        tui_set_bg(ctx, TUI_RGB(40, 40, 60));
        tui_label(ctx, 3, height - 2, g_status);
        
        tui_end_frame(ctx);
        sleep_ms(16);
    }
    
    tui_widget_destroy_recursive(root);
    free_editor_lines();
    tui_disable_mouse(ctx);
    tui_destroy(ctx);
    
    return 0;
}
