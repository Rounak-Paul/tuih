/*
 * tui.h - Single-header Terminal UI Library
 *
 * A zero-dependency, cross-platform TUI library in pure C11.
 * Supports Linux, macOS, and Windows 10+.
 *
 * Usage:
 *   #define TUI_IMPLEMENTATION
 *   #include "tui.h"
 *
 * License: Public Domain / MIT (choose one)
 */

#ifndef TUI_H
#define TUI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * PUBLIC API - Types
 * ============================================================================ */

typedef struct {
    uint32_t codepoint;
    uint32_t fg;
    uint32_t bg;
    uint32_t underline_color;  /* For colored underlines/undercurl */
    uint8_t  style;
} tui_cell;

typedef enum {
    TUI_KEY_NONE,
    TUI_KEY_UP,
    TUI_KEY_DOWN,
    TUI_KEY_LEFT,
    TUI_KEY_RIGHT,
    TUI_KEY_ENTER,
    TUI_KEY_ESC,
    TUI_KEY_BACKSPACE,
    TUI_KEY_TAB,
    TUI_KEY_SPACE,
    TUI_KEY_CHAR,
    TUI_KEY_HOME,
    TUI_KEY_END,
    TUI_KEY_PAGEUP,
    TUI_KEY_PAGEDOWN,
    TUI_KEY_INSERT,
    TUI_KEY_DELETE,
    TUI_KEY_F1,
    TUI_KEY_F2,
    TUI_KEY_F3,
    TUI_KEY_F4,
    TUI_KEY_F5,
    TUI_KEY_F6,
    TUI_KEY_F7,
    TUI_KEY_F8,
    TUI_KEY_F9,
    TUI_KEY_F10,
    TUI_KEY_F11,
    TUI_KEY_F12
} tui_key;

typedef enum {
    TUI_EVENT_NONE,
    TUI_EVENT_KEY,
    TUI_EVENT_MOUSE,
    TUI_EVENT_RESIZE,
    TUI_EVENT_PASTE_START,   /* Bracketed paste begin */
    TUI_EVENT_PASTE_END,     /* Bracketed paste end */
    TUI_EVENT_FOCUS_IN,      /* Terminal gained focus */
    TUI_EVENT_FOCUS_OUT      /* Terminal lost focus */
} tui_event_type;

typedef enum {
    TUI_MOUSE_NONE,
    TUI_MOUSE_LEFT,
    TUI_MOUSE_MIDDLE,
    TUI_MOUSE_RIGHT,
    TUI_MOUSE_RELEASE,
    TUI_MOUSE_WHEEL_UP,
    TUI_MOUSE_WHEEL_DOWN,
    TUI_MOUSE_MOVE
} tui_mouse_button;

typedef struct {
    tui_event_type type;
    tui_key key;
    uint32_t ch;
    tui_mouse_button mouse_button;
    int mouse_x;
    int mouse_y;
    int new_width;
    int new_height;
    bool ctrl;      /* Ctrl modifier held */
    bool alt;       /* Alt modifier held */
    bool shift;     /* Shift modifier held */
} tui_event;

/* Style flags */
#define TUI_STYLE_NONE        0x00
#define TUI_STYLE_BOLD        0x01
#define TUI_STYLE_DIM         0x02
#define TUI_STYLE_ITALIC      0x04
#define TUI_STYLE_UNDERLINE   0x08
#define TUI_STYLE_BLINK       0x10
#define TUI_STYLE_REVERSE     0x20
#define TUI_STYLE_STRIKETHROUGH 0x40
#define TUI_STYLE_UNDERCURL   0x80

/* Cursor shapes */
typedef enum {
    TUI_CURSOR_DEFAULT,      /* Terminal default */
    TUI_CURSOR_BLOCK_BLINK,  /* Blinking block */
    TUI_CURSOR_BLOCK,        /* Steady block */
    TUI_CURSOR_UNDERLINE_BLINK,  /* Blinking underline */
    TUI_CURSOR_UNDERLINE,    /* Steady underline */
    TUI_CURSOR_BAR_BLINK,    /* Blinking bar (beam) */
    TUI_CURSOR_BAR           /* Steady bar (beam) */
} tui_cursor_shape;

/* Default colors (RGB packed as 0x00RRGGBB, high bit set means use default) */
#define TUI_COLOR_DEFAULT   0x80000000
#define TUI_COLOR_BLACK     0x00000000
#define TUI_COLOR_RED       0x00CC0000
#define TUI_COLOR_GREEN     0x0000CC00
#define TUI_COLOR_YELLOW    0x00CCCC00
#define TUI_COLOR_BLUE      0x000000CC
#define TUI_COLOR_MAGENTA   0x00CC00CC
#define TUI_COLOR_CYAN      0x0000CCCC
#define TUI_COLOR_WHITE     0x00CCCCCC

/* Make RGB color */
#define TUI_RGB(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

/* Forward declaration */
typedef struct tui_context tui_context;

/* ============================================================================
 * PUBLIC API - Functions
 * ============================================================================ */

tui_context* tui_create(void);
void tui_destroy(tui_context* ctx);

void tui_begin_frame(tui_context* ctx);
void tui_end_frame(tui_context* ctx);

int tui_get_width(tui_context* ctx);
int tui_get_height(tui_context* ctx);

void tui_set_fg(tui_context* ctx, uint32_t color);
void tui_set_bg(tui_context* ctx, uint32_t color);
void tui_set_style(tui_context* ctx, uint8_t style);

void tui_set_cell(tui_context* ctx, int x, int y, uint32_t codepoint);
void tui_label(tui_context* ctx, int x, int y, const char* text);
int  tui_button(tui_context* ctx, int x, int y, const char* text);

int tui_poll_event(tui_context* ctx, tui_event* event);

void tui_set_cursor(tui_context* ctx, int x, int y);
void tui_show_cursor(tui_context* ctx, bool show);

void tui_enable_mouse(tui_context* ctx);
void tui_disable_mouse(tui_context* ctx);

bool tui_resized(tui_context* ctx);
void tui_clear(tui_context* ctx);

/* Cursor shape */
void tui_set_cursor_shape(tui_context* ctx, tui_cursor_shape shape);

/* Underline color (for colored underlines/undercurl) */
void tui_set_underline_color(tui_context* ctx, uint32_t color);
void tui_reset_underline_color(tui_context* ctx);

/* Bracketed paste mode */
void tui_enable_bracketed_paste(tui_context* ctx);
void tui_disable_bracketed_paste(tui_context* ctx);
bool tui_is_pasting(tui_context* ctx);

/* Terminal focus events */
void tui_enable_focus_events(tui_context* ctx);
void tui_disable_focus_events(tui_context* ctx);

/* Synchronized output (prevents tearing) */
void tui_begin_sync(tui_context* ctx);
void tui_end_sync(tui_context* ctx);

/* Clipboard (OSC 52) */
void tui_clipboard_set(tui_context* ctx, const char* text);

/* Hyperlinks (OSC 8) */
void tui_hyperlink_start(tui_context* ctx, const char* url);
void tui_hyperlink_end(tui_context* ctx);

/* Wide character support */
int tui_char_width(uint32_t codepoint);  /* Returns 0, 1, or 2 */
void tui_set_cell_wide(tui_context* ctx, int x, int y, uint32_t codepoint);  /* For double-width chars */

/* Box drawing styles */
typedef enum {
    TUI_BORDER_NONE,
    TUI_BORDER_SINGLE,
    TUI_BORDER_DOUBLE,
    TUI_BORDER_ROUNDED,
    TUI_BORDER_BOLD,
    TUI_BORDER_ASCII
} tui_border_style;

/* Box/Rectangle drawing */
void tui_box(tui_context* ctx, int x, int y, int w, int h, tui_border_style style);
void tui_fill(tui_context* ctx, int x, int y, int w, int h, uint32_t ch);
void tui_hline(tui_context* ctx, int x, int y, int len, uint32_t ch);
void tui_vline(tui_context* ctx, int x, int y, int len, uint32_t ch);

/* Text alignment */
typedef enum {
    TUI_ALIGN_LEFT,
    TUI_ALIGN_CENTER,
    TUI_ALIGN_RIGHT
} tui_align;

void tui_label_aligned(tui_context* ctx, int x, int y, int width, const char* text, tui_align align);
int tui_text_width(const char* text);

/* Text wrapping */
int tui_wrap_text(tui_context* ctx, int x, int y, int width, int max_lines, const char* text);

/* Modal/Popup helpers */
void tui_popup_box(tui_context* ctx, int x, int y, int w, int h, const char* title, tui_border_style style);

/* ============================================================================
 * Theming System
 * ============================================================================ */

typedef struct {
    /* General colors */
    uint32_t bg;
    uint32_t fg;
    uint32_t fg_dim;
    
    /* Widget colors */
    uint32_t widget_bg;
    uint32_t widget_fg;
    uint32_t widget_border;
    
    /* Focus/Selection colors */
    uint32_t focus_bg;
    uint32_t focus_fg;
    uint32_t select_bg;
    uint32_t select_fg;
    
    /* Accent colors */
    uint32_t accent;
    uint32_t accent_dim;
    
    /* Semantic colors */
    uint32_t success;
    uint32_t warning;
    uint32_t error;
    uint32_t info;
    
    /* Border style */
    tui_border_style border_style;
} tui_theme;

/* Built-in themes */
extern const tui_theme TUI_THEME_DEFAULT;
extern const tui_theme TUI_THEME_DARK;
extern const tui_theme TUI_THEME_LIGHT;
extern const tui_theme TUI_THEME_BLUE;
extern const tui_theme TUI_THEME_GREEN;

void tui_set_theme(tui_context* ctx, const tui_theme* theme);
const tui_theme* tui_get_theme(tui_context* ctx);

/* Animation helpers */
uint32_t tui_lerp_color(uint32_t from, uint32_t to, float t);
float tui_ease_in_out(float t);

/* ============================================================================
 * HIERARCHICAL WIDGET SYSTEM
 * ============================================================================ */

/* Forward declarations */
typedef struct tui_widget tui_widget;
typedef struct tui_widget_event tui_widget_event;

/* Widget types */
typedef enum {
    TUI_WIDGET_CONTAINER,   /* Invisible container for grouping */
    TUI_WIDGET_PANEL,       /* Visible panel with optional border */
    TUI_WIDGET_LABEL,       /* Static text */
    TUI_WIDGET_BUTTON,      /* Clickable button */
    TUI_WIDGET_TEXTBOX,     /* Single-line text input */
    TUI_WIDGET_TEXTAREA,    /* Multi-line text editor */
    TUI_WIDGET_CHECKBOX,    /* Toggle checkbox */
    TUI_WIDGET_RADIO,       /* Radio button (exclusive selection) */
    TUI_WIDGET_LIST,        /* Scrollable list */
    TUI_WIDGET_PROGRESS,    /* Progress bar */
    TUI_WIDGET_SLIDER,      /* Value slider */
    TUI_WIDGET_SPINNER,     /* Numeric spinner */
    TUI_WIDGET_DROPDOWN,    /* Dropdown select */
    TUI_WIDGET_TABS,        /* Tab bar */
    TUI_WIDGET_SCROLLBAR,   /* Scrollbar */
    TUI_WIDGET_SPLITTER,    /* Resizable split pane */
    TUI_WIDGET_CUSTOM       /* User-defined widget */
} tui_widget_type;

/* Event phases for bubbling */
typedef enum {
    TUI_PHASE_CAPTURE,      /* Going down the tree (parent → child) */
    TUI_PHASE_TARGET,       /* At the target widget */
    TUI_PHASE_BUBBLE        /* Going up the tree (child → parent) */
} tui_event_phase;

/* Widget event (enhanced version of tui_event) */
struct tui_widget_event {
    tui_event base;             /* Original event data */
    tui_event_phase phase;      /* Current phase */
    tui_widget* target;         /* Widget that will receive the event */
    tui_widget* current;        /* Widget currently processing */
    bool stopped;               /* stopPropagation() called */
    bool prevented;             /* preventDefault() called */
    bool consumed;              /* Event fully handled, don't bubble */
};

/* Event handler callback */
typedef void (*tui_event_handler)(tui_widget* widget, tui_widget_event* event, void* userdata);

/* Widget draw callback (for custom widgets) */
typedef void (*tui_widget_draw_fn)(tui_widget* widget, tui_context* ctx);

/* Maximum children per widget */
#define TUI_MAX_CHILDREN 64
#define TUI_MAX_HANDLERS 8
#define TUI_MAX_HOTKEYS  32

/* Event handler entry */
typedef struct {
    tui_event_type event_type;
    tui_event_handler handler;
    void* userdata;
    bool capture;               /* Handle during capture phase */
} tui_handler_entry;

/* Hotkey registration */
typedef struct {
    tui_key key;
    uint32_t ch;                /* For TUI_KEY_CHAR */
    bool ctrl;
    bool alt;
    bool shift;
    tui_event_handler handler;
    void* userdata;
    bool active;
} tui_hotkey;

/* Widget structure */
struct tui_widget {
    /* Identity */
    tui_widget_type type;
    int id;                     /* User-assigned ID */
    const char* name;           /* Optional name for debugging */
    
    /* Bounds (relative to parent) */
    int x, y;
    int width, height;
    
    /* Tree structure */
    tui_widget* parent;
    tui_widget* children[TUI_MAX_CHILDREN];
    int child_count;
    
    /* State */
    bool visible;
    bool enabled;
    bool focusable;
    bool focused;
    int tab_index;              /* Tab order (-1 = not tabbable) */
    
    /* Event handlers */
    tui_handler_entry handlers[TUI_MAX_HANDLERS];
    int handler_count;
    
    /* Custom draw function */
    tui_widget_draw_fn draw_fn;
    
    /* Widget-specific data */
    void* data;
    
    /* Style overrides */
    uint32_t bg_color;
    uint32_t fg_color;
    tui_border_style border_style;
    bool has_border;
    
    /* For specific widget types */
    union {
        struct { const char* text; tui_align align; } label;
        struct { const char* text; bool pressed; } button;
        struct { char* buffer; int capacity; int length; int cursor; int scroll; } textbox;
        struct { 
            char** lines;           /* Array of line pointers (mutable if editable) */
            int line_count;         /* Total number of lines */
            int line_capacity;      /* Allocated capacity for lines array */
            int cursor_row;         /* Cursor row (0-based) */
            int cursor_col;         /* Cursor column (0-based) */
            int scroll_row;         /* First visible row */
            int scroll_col;         /* Horizontal scroll offset */
            int sel_start_row;      /* Selection start row (-1 = no selection) */
            int sel_start_col;      /* Selection start column */
            int sel_end_row;        /* Selection end row */
            int sel_end_col;        /* Selection end column */
            bool line_numbers;      /* Show line numbers gutter */
            bool word_wrap;         /* Enable word wrapping */
            bool editable;          /* Allow text editing */
            int max_line_len;       /* Max chars per line (for editable mode) */
        } textarea;
        struct { const char* text; bool checked; } checkbox;
        struct { const char* text; int* group_value; int value; } radio;
        struct { const char** items; int count; int selected; int scroll; int visible; } list;
        struct { float value; float min; float max; } progress;
        struct { float value; float min; float max; float step; bool dragging; } slider;
        struct { int value; int min; int max; int step; } spinner;
        struct { const char** items; int count; int selected; int scroll; bool open; } dropdown;
        struct { const char** labels; int count; int selected; } tabs;
        struct { int content_size; int view_size; int scroll; bool vertical; bool dragging; int drag_start; } scrollbar;
        struct {
            bool vertical;          /* Vertical or horizontal split */
            float ratio;            /* Split ratio (0.0 - 1.0) */
            int min_size;           /* Minimum size for each pane */
            bool dragging;          /* User is dragging the divider */
        } splitter;
    } state;
};

/* Focus stack for modals */
#define TUI_MAX_FOCUS_STACK 16

typedef struct {
    tui_widget* root;           /* Root widget */
    tui_widget* focus;          /* Currently focused widget */
    tui_widget* hover;          /* Widget under mouse */
    tui_widget* focus_stack[TUI_MAX_FOCUS_STACK];  /* For modals */
    int focus_stack_top;
    tui_hotkey hotkeys[TUI_MAX_HOTKEYS];
    int hotkey_count;
} tui_widget_manager;

/* Widget creation/destruction */
tui_widget* tui_widget_create(tui_widget_type type);
void tui_widget_destroy(tui_widget* widget);
void tui_widget_destroy_recursive(tui_widget* widget);

/* Tree management */
void tui_widget_add_child(tui_widget* parent, tui_widget* child);
void tui_widget_remove_child(tui_widget* parent, tui_widget* child);
tui_widget* tui_widget_find_by_id(tui_widget* root, int id);
tui_widget* tui_widget_find_by_name(tui_widget* root, const char* name);

/* Bounds */
void tui_widget_set_bounds(tui_widget* w, int x, int y, int width, int height);
void tui_widget_get_absolute_bounds(tui_widget* w, int* x, int* y, int* width, int* height);
bool tui_widget_contains_point(tui_widget* w, int x, int y);

/* Event handling */
void tui_widget_on(tui_widget* w, tui_event_type type, tui_event_handler handler, void* userdata);
void tui_widget_on_capture(tui_widget* w, tui_event_type type, tui_event_handler handler, void* userdata);
void tui_widget_off(tui_widget* w, tui_event_type type, tui_event_handler handler);

/* Event helpers */
void tui_widget_event_stop(tui_widget_event* e);        /* Stop propagation */
void tui_widget_event_prevent(tui_widget_event* e);     /* Prevent default action */
void tui_widget_event_consume(tui_widget_event* e);     /* Fully consume */

/* Widget manager */
void tui_wm_init(tui_widget_manager* wm);
void tui_wm_set_root(tui_widget_manager* wm, tui_widget* root);
void tui_wm_route_event(tui_widget_manager* wm, tui_event* event);
void tui_wm_draw(tui_widget_manager* wm, tui_context* ctx);

/* Focus management */
void tui_wm_focus(tui_widget_manager* wm, tui_widget* widget);
void tui_wm_focus_next(tui_widget_manager* wm);
void tui_wm_focus_prev(tui_widget_manager* wm);
tui_widget* tui_wm_get_focus(tui_widget_manager* wm);

/* Focus stack (for modals) */
void tui_wm_push_focus(tui_widget_manager* wm, tui_widget* modal_root);
void tui_wm_pop_focus(tui_widget_manager* wm);

/* Hit testing */
tui_widget* tui_wm_hit_test(tui_widget_manager* wm, int x, int y);

/* Hotkeys */
void tui_wm_register_hotkey(tui_widget_manager* wm, tui_key key, uint32_t ch,
                            bool ctrl, bool alt, bool shift,
                            tui_event_handler handler, void* userdata);
void tui_wm_unregister_hotkey(tui_widget_manager* wm, tui_key key, uint32_t ch,
                              bool ctrl, bool alt, bool shift);

#ifdef __cplusplus
}
#endif

#endif /* TUI_H */

/* ============================================================================
 * IMPLEMENTATION
 * ============================================================================ */

#ifdef TUI_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * Platform Detection
 * ============================================================================ */

#if defined(_WIN32) || defined(_WIN64)
    #define TUI_PLATFORM_WINDOWS 1
#else
    #define TUI_PLATFORM_POSIX 1
#endif

#ifdef TUI_PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <errno.h>
#endif

/* ============================================================================
 * Constants
 * ============================================================================ */

#define TUI_MAX_WIDTH  512
#define TUI_MAX_HEIGHT 256
#define TUI_INPUT_BUFFER_SIZE 64
#define TUI_OUTPUT_BUFFER_SIZE 65536

/* ============================================================================
 * Internal Structures
 * ============================================================================ */

struct tui_context {
    /* Terminal dimensions */
    int width;
    int height;
    
    /* Double buffering */
    tui_cell* front_buffer;
    tui_cell* back_buffer;
    
    /* Current drawing state */
    uint32_t current_fg;
    uint32_t current_bg;
    uint8_t  current_style;
    
    /* Input handling */
    uint8_t input_buffer[TUI_INPUT_BUFFER_SIZE];
    int input_start;
    int input_end;
    
    /* Output buffering */
    char output_buffer[TUI_OUTPUT_BUFFER_SIZE];
    int output_pos;
    
    /* Cursor state */
    int cursor_x;
    int cursor_y;
    bool cursor_visible;
    
    /* Button interaction state */
    int hot_button_x;
    int hot_button_y;
    bool button_pressed;
    
    /* Mouse state */
    bool mouse_enabled;
    int mouse_x;
    int mouse_y;
    tui_mouse_button mouse_button;
    
    /* Resize state */
    bool resized;
    int prev_width;
    int prev_height;
    
    /* Platform-specific state */
#ifdef TUI_PLATFORM_WINDOWS
    HANDLE stdin_handle;
    HANDLE stdout_handle;
    DWORD original_stdin_mode;
    DWORD original_stdout_mode;
#else
    struct termios original_termios;
    int tty_fd;
#endif
    
    /* Theme */
    const tui_theme* theme;
    
    /* Underline color (0x80000000 = default/none) */
    uint32_t current_underline_color;
    
    /* Cursor shape */
    tui_cursor_shape cursor_shape;
    
    /* Feature flags */
    bool bracketed_paste_enabled;
    bool focus_events_enabled;
    bool is_pasting;  /* Currently receiving bracketed paste */
    
    /* State flags */
    bool initialized;
    bool in_frame;
    bool needs_redraw;  /* Force full screen redraw on next frame */
};

/* ============================================================================
 * Internal Helpers - Output Buffering
 * ============================================================================ */

static void tui_output_flush(tui_context* ctx) {
    if (ctx->output_pos > 0) {
#ifdef TUI_PLATFORM_WINDOWS
        DWORD written;
        WriteConsoleA(ctx->stdout_handle, ctx->output_buffer, (DWORD)ctx->output_pos, &written, NULL);
#else
        ssize_t total = 0;
        while (total < ctx->output_pos) {
            ssize_t n = write(ctx->tty_fd, ctx->output_buffer + total, (size_t)(ctx->output_pos - total));
            if (n < 0) {
                if (errno == EINTR) continue;
                break;
            }
            total += n;
        }
#endif
        ctx->output_pos = 0;
    }
}

static void tui_output_write(tui_context* ctx, const char* data, int len) {
    while (len > 0) {
        int space = TUI_OUTPUT_BUFFER_SIZE - ctx->output_pos;
        if (space == 0) {
            tui_output_flush(ctx);
            space = TUI_OUTPUT_BUFFER_SIZE;
        }
        int chunk = (len < space) ? len : space;
        memcpy(ctx->output_buffer + ctx->output_pos, data, (size_t)chunk);
        ctx->output_pos += chunk;
        data += chunk;
        len -= chunk;
    }
}

static void tui_output_str(tui_context* ctx, const char* str) {
    tui_output_write(ctx, str, (int)strlen(str));
}

static void tui_output_int(tui_context* ctx, int value) {
    char buf[16];
    int pos = 15;
    buf[pos] = '\0';
    if (value == 0) {
        buf[--pos] = '0';
    } else {
        int neg = 0;
        if (value < 0) {
            neg = 1;
            value = -value;
        }
        while (value > 0 && pos > 0) {
            buf[--pos] = '0' + (value % 10);
            value /= 10;
        }
        if (neg && pos > 0) {
            buf[--pos] = '-';
        }
    }
    tui_output_str(ctx, buf + pos);
}

/* ============================================================================
 * Internal Helpers - ANSI/VT Sequences
 * ============================================================================ */

/* Move cursor to position (1-indexed for ANSI) */
static void tui_ansi_move_cursor(tui_context* ctx, int x, int y) {
    tui_output_str(ctx, "\x1b[");
    tui_output_int(ctx, y + 1);
    tui_output_str(ctx, ";");
    tui_output_int(ctx, x + 1);
    tui_output_str(ctx, "H");
}

/* Hide cursor */
static void tui_ansi_hide_cursor(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?25l");
}

/* Show cursor */
static void tui_ansi_show_cursor(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?25h");
}

/* Reset all attributes */
static void tui_ansi_reset(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[0m");
}

/* Set foreground color (RGB) */
static void tui_ansi_set_fg(tui_context* ctx, uint32_t color) {
    if (color & 0x80000000) {
        tui_output_str(ctx, "\x1b[39m");
    } else {
        tui_output_str(ctx, "\x1b[38;2;");
        tui_output_int(ctx, (int)((color >> 16) & 0xFF));
        tui_output_str(ctx, ";");
        tui_output_int(ctx, (int)((color >> 8) & 0xFF));
        tui_output_str(ctx, ";");
        tui_output_int(ctx, (int)(color & 0xFF));
        tui_output_str(ctx, "m");
    }
}

/* Set background color (RGB) */
static void tui_ansi_set_bg(tui_context* ctx, uint32_t color) {
    if (color & 0x80000000) {
        tui_output_str(ctx, "\x1b[49m");
    } else {
        tui_output_str(ctx, "\x1b[48;2;");
        tui_output_int(ctx, (int)((color >> 16) & 0xFF));
        tui_output_str(ctx, ";");
        tui_output_int(ctx, (int)((color >> 8) & 0xFF));
        tui_output_str(ctx, ";");
        tui_output_int(ctx, (int)(color & 0xFF));
        tui_output_str(ctx, "m");
    }
}

/* Set text style attributes */
static void tui_ansi_set_style(tui_context* ctx, uint8_t style) {
    if (style & TUI_STYLE_BOLD)        tui_output_str(ctx, "\x1b[1m");
    if (style & TUI_STYLE_DIM)         tui_output_str(ctx, "\x1b[2m");
    if (style & TUI_STYLE_ITALIC)      tui_output_str(ctx, "\x1b[3m");
    if (style & TUI_STYLE_UNDERLINE)   tui_output_str(ctx, "\x1b[4m");
    if (style & TUI_STYLE_BLINK)       tui_output_str(ctx, "\x1b[5m");
    if (style & TUI_STYLE_REVERSE)     tui_output_str(ctx, "\x1b[7m");
    if (style & TUI_STYLE_STRIKETHROUGH) tui_output_str(ctx, "\x1b[9m");
    if (style & TUI_STYLE_UNDERCURL)   tui_output_str(ctx, "\x1b[4:3m");  /* Curly underline */
}

/* Set cursor shape */
static void tui_ansi_set_cursor_shape(tui_context* ctx, tui_cursor_shape shape) {
    char buf[8];
    /* DECSCUSR: 0=default, 1=block blink, 2=block, 3=underline blink, 4=underline, 5=bar blink, 6=bar */
    int code = (int)shape;
    snprintf(buf, sizeof(buf), "\x1b[%d q", code);
    tui_output_str(ctx, buf);
}

/* Set underline color (for colored underlines/undercurl) */
static void tui_ansi_set_underline_color(tui_context* ctx, uint32_t color) {
    if (color & 0x80000000) {
        /* Default - reset underline color */
        tui_output_str(ctx, "\x1b[59m");
    } else {
        char buf[24];
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        snprintf(buf, sizeof(buf), "\x1b[58;2;%d;%d;%dm", r, g, b);
        tui_output_str(ctx, buf);
    }
}

/* Enable bracketed paste mode */
static void tui_ansi_enable_bracketed_paste(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?2004h");
}

/* Disable bracketed paste mode */
static void tui_ansi_disable_bracketed_paste(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?2004l");
}

/* Enable focus reporting */
static void tui_ansi_enable_focus_events(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?1004h");
}

/* Disable focus reporting */
static void tui_ansi_disable_focus_events(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?1004l");
}

/* Begin synchronized update (prevents tearing) */
static void tui_ansi_begin_sync(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?2026h");
}

/* End synchronized update */
static void tui_ansi_end_sync(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?2026l");
}

/* Set clipboard via OSC 52 */
static void tui_ansi_set_clipboard(tui_context* ctx, const char* base64_text) {
    tui_output_str(ctx, "\x1b]52;c;");
    tui_output_str(ctx, base64_text);
    tui_output_str(ctx, "\x1b\\");
}

/* Start hyperlink (OSC 8) */
static void tui_ansi_hyperlink_start(tui_context* ctx, const char* url) {
    tui_output_str(ctx, "\x1b]8;;");
    tui_output_str(ctx, url);
    tui_output_str(ctx, "\x1b\\");
}

/* End hyperlink */
static void tui_ansi_hyperlink_end(tui_context* ctx) {
    tui_output_str(ctx, "\x1b]8;;\x1b\\");
}

/* Enter alternate screen buffer */
static void tui_ansi_enter_alt_screen(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?1049h");
}

/* Leave alternate screen buffer */
static void tui_ansi_leave_alt_screen(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?1049l");
}

/* Clear entire screen */
static void tui_ansi_clear_screen(tui_context* ctx) {
    /* Reset attributes first, then clear to avoid colored background */
    tui_output_str(ctx, "\x1b[0m\x1b[2J\x1b[H");
}

/* Enable mouse tracking with SGR extended mode for better coordinates */
static void tui_ansi_enable_mouse(tui_context* ctx) {
    /* 1000: basic mouse tracking, 1002: button motion, 1006: SGR extended */
    tui_output_str(ctx, "\x1b[?1000h\x1b[?1002h\x1b[?1006h");
}

/* Disable mouse tracking */
static void tui_ansi_disable_mouse(tui_context* ctx) {
    tui_output_str(ctx, "\x1b[?1006l\x1b[?1002l\x1b[?1000l");
}

/* ============================================================================
 * Internal Helpers - Wide Character Width (wcwidth implementation)
 * ============================================================================ */

/* Returns the display width of a Unicode codepoint: 0, 1, or 2 */
int tui_char_width(uint32_t cp) {
    /* Combining characters and zero-width */
    if (cp == 0) return 0;
    if (cp < 32 || (cp >= 0x7F && cp < 0xA0)) return 0;  /* Control chars */
    
    /* Combining diacritical marks and other zero-width */
    if ((cp >= 0x0300 && cp <= 0x036F) ||   /* Combining Diacriticals */
        (cp >= 0x1AB0 && cp <= 0x1AFF) ||   /* Combining Diacriticals Extended */
        (cp >= 0x1DC0 && cp <= 0x1DFF) ||   /* Combining Diacriticals Supplement */
        (cp >= 0x20D0 && cp <= 0x20FF) ||   /* Combining Diacriticals for Symbols */
        (cp >= 0xFE20 && cp <= 0xFE2F) ||   /* Combining Half Marks */
        cp == 0x200B || cp == 0x200C ||     /* Zero-width space/non-joiner */
        cp == 0x200D || cp == 0xFEFF) {     /* Zero-width joiner/BOM */
        return 0;
    }
    
    /* East Asian Wide and Fullwidth characters */
    if ((cp >= 0x1100 && cp <= 0x115F) ||   /* Hangul Jamo */
        (cp >= 0x2E80 && cp <= 0x9FFF) ||   /* CJK, Radicals, etc. */
        (cp >= 0xAC00 && cp <= 0xD7A3) ||   /* Hangul Syllables */
        (cp >= 0xF900 && cp <= 0xFAFF) ||   /* CJK Compatibility Ideographs */
        (cp >= 0xFE10 && cp <= 0xFE1F) ||   /* Vertical forms */
        (cp >= 0xFE30 && cp <= 0xFE6F) ||   /* CJK Compatibility Forms */
        (cp >= 0xFF00 && cp <= 0xFF60) ||   /* Fullwidth Forms */
        (cp >= 0xFFE0 && cp <= 0xFFE6) ||   /* Fullwidth symbols */
        (cp >= 0x20000 && cp <= 0x2FFFD) || /* CJK Extension B+ */
        (cp >= 0x30000 && cp <= 0x3FFFD)) { /* CJK Extension G+ */
        return 2;
    }
    
    /* Emoji (simplified - most common ranges) */
    if ((cp >= 0x1F300 && cp <= 0x1F9FF) || /* Misc Symbols, Emoticons, etc. */
        (cp >= 0x2600 && cp <= 0x26FF) ||   /* Misc symbols */
        (cp >= 0x2700 && cp <= 0x27BF)) {   /* Dingbats */
        return 2;
    }
    
    return 1;
}

/* ============================================================================
 * Internal Helpers - Base64 Encoding (for clipboard)
 * ============================================================================ */

static const char tui_base64_table[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int tui_base64_encode(const char* input, int input_len, char* output, int output_size) {
    int i = 0, j = 0;
    uint8_t a3[3], a4[4];
    int a3_len = 0;
    
    while (i < input_len) {
        a3[a3_len++] = (uint8_t)input[i++];
        if (a3_len == 3) {
            a4[0] = (a3[0] >> 2) & 0x3F;
            a4[1] = ((a3[0] << 4) | (a3[1] >> 4)) & 0x3F;
            a4[2] = ((a3[1] << 2) | (a3[2] >> 6)) & 0x3F;
            a4[3] = a3[2] & 0x3F;
            for (int k = 0; k < 4 && j < output_size - 1; k++) {
                output[j++] = tui_base64_table[a4[k]];
            }
            a3_len = 0;
        }
    }
    
    if (a3_len > 0) {
        for (int k = a3_len; k < 3; k++) a3[k] = 0;
        a4[0] = (a3[0] >> 2) & 0x3F;
        a4[1] = ((a3[0] << 4) | (a3[1] >> 4)) & 0x3F;
        a4[2] = ((a3[1] << 2) | (a3[2] >> 6)) & 0x3F;
        for (int k = 0; k < a3_len + 1 && j < output_size - 1; k++) {
            output[j++] = tui_base64_table[a4[k]];
        }
        while (a3_len++ < 3 && j < output_size - 1) {
            output[j++] = '=';
        }
    }
    
    output[j] = '\0';
    return j;
}

/* ============================================================================
 * Internal Helpers - UTF-8 Encoding
 * ============================================================================ */

static int tui_utf8_encode(uint32_t codepoint, char* out) {
    if (codepoint < 0x80) {
        out[0] = (char)codepoint;
        return 1;
    } else if (codepoint < 0x800) {
        out[0] = (char)(0xC0 | (codepoint >> 6));
        out[1] = (char)(0x80 | (codepoint & 0x3F));
        return 2;
    } else if (codepoint < 0x10000) {
        out[0] = (char)(0xE0 | (codepoint >> 12));
        out[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[2] = (char)(0x80 | (codepoint & 0x3F));
        return 3;
    } else if (codepoint < 0x110000) {
        out[0] = (char)(0xF0 | (codepoint >> 18));
        out[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        out[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[3] = (char)(0x80 | (codepoint & 0x3F));
        return 4;
    }
    out[0] = '?';
    return 1;
}

static int tui_utf8_decode(const uint8_t* data, int len, uint32_t* out) {
    if (len < 1) return 0;
    
    uint8_t b0 = data[0];
    if (b0 < 0x80) {
        *out = b0;
        return 1;
    } else if ((b0 & 0xE0) == 0xC0 && len >= 2) {
        *out = ((uint32_t)(b0 & 0x1F) << 6) | (data[1] & 0x3F);
        return 2;
    } else if ((b0 & 0xF0) == 0xE0 && len >= 3) {
        *out = ((uint32_t)(b0 & 0x0F) << 12) | 
               ((uint32_t)(data[1] & 0x3F) << 6) | 
               (data[2] & 0x3F);
        return 3;
    } else if ((b0 & 0xF8) == 0xF0 && len >= 4) {
        *out = ((uint32_t)(b0 & 0x07) << 18) | 
               ((uint32_t)(data[1] & 0x3F) << 12) | 
               ((uint32_t)(data[2] & 0x3F) << 6) | 
               (data[3] & 0x3F);
        return 4;
    }
    
    *out = b0;
    return 1;
}

/* ============================================================================
 * Internal Helpers - Terminal Size Detection
 * ============================================================================ */

static void tui_get_terminal_size(tui_context* ctx) {
#ifdef TUI_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(ctx->stdout_handle, &csbi)) {
        ctx->width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        ctx->height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        ctx->width = 80;
        ctx->height = 24;
    }
#else
    struct winsize ws;
    if (ioctl(ctx->tty_fd, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0 && ws.ws_row > 0) {
        ctx->width = ws.ws_col;
        ctx->height = ws.ws_row;
    } else {
        ctx->width = 80;
        ctx->height = 24;
    }
#endif
    
    /* Clamp to maximum dimensions */
    if (ctx->width > TUI_MAX_WIDTH) ctx->width = TUI_MAX_WIDTH;
    if (ctx->height > TUI_MAX_HEIGHT) ctx->height = TUI_MAX_HEIGHT;
}

/* ============================================================================
 * Internal Helpers - Buffer Management
 * ============================================================================ */

static tui_cell tui_make_empty_cell(void) {
    tui_cell cell;
    cell.codepoint = ' ';
    cell.fg = TUI_COLOR_DEFAULT;
    cell.bg = TUI_COLOR_DEFAULT;
    cell.underline_color = TUI_COLOR_DEFAULT;
    cell.style = TUI_STYLE_NONE;
    return cell;
}

static void tui_clear_buffer(tui_cell* buffer, int width, int height) {
    tui_cell empty = tui_make_empty_cell();
    /* Clear using MAX_WIDTH stride to match buffer layout */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            buffer[y * TUI_MAX_WIDTH + x] = empty;
        }
    }
}

static bool tui_cell_equal(const tui_cell* a, const tui_cell* b) {
    return a->codepoint == b->codepoint &&
           a->fg == b->fg &&
           a->bg == b->bg &&
           a->underline_color == b->underline_color &&
           a->style == b->style;
}

/* ============================================================================
 * Platform-Specific - Terminal Setup (POSIX)
 * ============================================================================ */

#ifdef TUI_PLATFORM_POSIX

/* Global pointer for SIGWINCH handler (only one TUI context supported) */
static volatile sig_atomic_t tui_sigwinch_received = 0;

static void tui_sigwinch_handler(int sig) {
    (void)sig;
    tui_sigwinch_received = 1;
}

static int tui_posix_setup(tui_context* ctx) {
    /* Try to open /dev/tty for direct terminal access */
    ctx->tty_fd = open("/dev/tty", O_RDWR);
    if (ctx->tty_fd < 0) {
        /* Fall back to stdin/stdout */
        ctx->tty_fd = STDIN_FILENO;
    }
    
    /* Get and save current terminal settings */
    if (tcgetattr(ctx->tty_fd, &ctx->original_termios) < 0) {
        return -1;
    }
    
    /* Configure raw mode */
    struct termios raw = ctx->original_termios;
    
    /* Input flags: disable break signal, CR to NL, parity checking, strip bit, flow control */
    raw.c_iflag &= ~(unsigned int)(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    
    /* Output flags: disable post-processing */
    raw.c_oflag &= ~(unsigned int)(OPOST);
    
    /* Control flags: set 8-bit chars */
    raw.c_cflag |= (unsigned int)(CS8);
    
    /* Local flags: disable echo, canonical mode, signals, extended input processing */
    raw.c_lflag &= ~(unsigned int)(ECHO | ICANON | IEXTEN | ISIG);
    
    /* Control chars: set minimum bytes to read, no timeout */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    
    if (tcsetattr(ctx->tty_fd, TCSAFLUSH, &raw) < 0) {
        return -1;
    }
    
    /* Install SIGWINCH handler for terminal resize */
    struct sigaction sa;
    sa.sa_handler = tui_sigwinch_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGWINCH, &sa, NULL);
    
    return 0;
}

static void tui_posix_cleanup(tui_context* ctx) {
    /* Restore default SIGWINCH handler */
    signal(SIGWINCH, SIG_DFL);
    
    tcsetattr(ctx->tty_fd, TCSAFLUSH, &ctx->original_termios);
    if (ctx->tty_fd != STDIN_FILENO) {
        close(ctx->tty_fd);
    }
}

static int tui_posix_read_input(tui_context* ctx) {
    /* Calculate available space in circular buffer */
    int available;
    if (ctx->input_end >= ctx->input_start) {
        available = TUI_INPUT_BUFFER_SIZE - ctx->input_end;
        if (ctx->input_start == 0) available--;
    } else {
        available = ctx->input_start - ctx->input_end - 1;
    }
    
    if (available <= 0) return 0;
    
    /* Set non-blocking read */
    int flags = fcntl(ctx->tty_fd, F_GETFL, 0);
    fcntl(ctx->tty_fd, F_SETFL, flags | O_NONBLOCK);
    
    ssize_t n = read(ctx->tty_fd, ctx->input_buffer + ctx->input_end, (size_t)available);
    
    /* Restore blocking mode */
    fcntl(ctx->tty_fd, F_SETFL, flags);
    
    if (n > 0) {
        ctx->input_end = (ctx->input_end + (int)n) % TUI_INPUT_BUFFER_SIZE;
        return (int)n;
    }
    
    return 0;
}

#endif /* TUI_PLATFORM_POSIX */

/* ============================================================================
 * Platform-Specific - Terminal Setup (Windows)
 * ============================================================================ */

#ifdef TUI_PLATFORM_WINDOWS

static int tui_win32_setup(tui_context* ctx) {
    ctx->stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    ctx->stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (ctx->stdin_handle == INVALID_HANDLE_VALUE || 
        ctx->stdout_handle == INVALID_HANDLE_VALUE) {
        return -1;
    }
    
    /* Save original console modes */
    if (!GetConsoleMode(ctx->stdin_handle, &ctx->original_stdin_mode) ||
        !GetConsoleMode(ctx->stdout_handle, &ctx->original_stdout_mode)) {
        return -1;
    }
    
    /* Configure stdin: enable virtual terminal input, disable line input and echo */
    DWORD stdin_mode = ENABLE_VIRTUAL_TERMINAL_INPUT;
    if (!SetConsoleMode(ctx->stdin_handle, stdin_mode)) {
        return -1;
    }
    
    /* Configure stdout: enable virtual terminal processing */
    DWORD stdout_mode = ctx->original_stdout_mode | 
                        ENABLE_VIRTUAL_TERMINAL_PROCESSING | 
                        DISABLE_NEWLINE_AUTO_RETURN;
    if (!SetConsoleMode(ctx->stdout_handle, stdout_mode)) {
        /* Try without DISABLE_NEWLINE_AUTO_RETURN for older Windows versions */
        stdout_mode = ctx->original_stdout_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(ctx->stdout_handle, stdout_mode)) {
            return -1;
        }
    }
    
    /* Set console to UTF-8 */
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    return 0;
}

static void tui_win32_cleanup(tui_context* ctx) {
    SetConsoleMode(ctx->stdin_handle, ctx->original_stdin_mode);
    SetConsoleMode(ctx->stdout_handle, ctx->original_stdout_mode);
}

static int tui_win32_read_input(tui_context* ctx) {
    DWORD events_available = 0;
    if (!GetNumberOfConsoleInputEvents(ctx->stdin_handle, &events_available) || 
        events_available == 0) {
        return 0;
    }
    
    INPUT_RECORD records[16];
    DWORD events_read = 0;
    
    if (!ReadConsoleInputW(ctx->stdin_handle, records, 16, &events_read)) {
        return 0;
    }
    
    int bytes_added = 0;
    
    for (DWORD i = 0; i < events_read; i++) {
        if (records[i].EventType != KEY_EVENT) continue;
        
        KEY_EVENT_RECORD* key = &records[i].Event.KeyEvent;
        if (!key->bKeyDown) continue;
        
        /* Check available space in input buffer */
        int space = (ctx->input_start - ctx->input_end - 1 + TUI_INPUT_BUFFER_SIZE) % TUI_INPUT_BUFFER_SIZE;
        if (space < 4) break;
        
        /* Handle virtual key codes for special keys */
        switch (key->wVirtualKeyCode) {
            case VK_UP:
                ctx->input_buffer[ctx->input_end] = 0x1B;
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                ctx->input_buffer[ctx->input_end] = '[';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                ctx->input_buffer[ctx->input_end] = 'A';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                bytes_added += 3;
                continue;
            case VK_DOWN:
                ctx->input_buffer[ctx->input_end] = 0x1B;
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                ctx->input_buffer[ctx->input_end] = '[';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                ctx->input_buffer[ctx->input_end] = 'B';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                bytes_added += 3;
                continue;
            case VK_RIGHT:
                ctx->input_buffer[ctx->input_end] = 0x1B;
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                ctx->input_buffer[ctx->input_end] = '[';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                ctx->input_buffer[ctx->input_end] = 'C';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                bytes_added += 3;
                continue;
            case VK_LEFT:
                ctx->input_buffer[ctx->input_end] = 0x1B;
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                ctx->input_buffer[ctx->input_end] = '[';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                ctx->input_buffer[ctx->input_end] = 'D';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                bytes_added += 3;
                continue;
            case VK_ESCAPE:
                ctx->input_buffer[ctx->input_end] = 0x1B;
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                bytes_added += 1;
                continue;
            case VK_RETURN:
                ctx->input_buffer[ctx->input_end] = '\r';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                bytes_added += 1;
                continue;
            case VK_BACK:
                ctx->input_buffer[ctx->input_end] = 0x7F;
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                bytes_added += 1;
                continue;
            case VK_TAB:
                ctx->input_buffer[ctx->input_end] = '\t';
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
                bytes_added += 1;
                continue;
        }
        
        /* Handle regular character input */
        WCHAR wc = key->uChar.UnicodeChar;
        if (wc == 0) continue;
        
        /* Convert UTF-16 to UTF-8 */
        uint32_t codepoint = (uint32_t)wc;
        char utf8[4];
        int len = tui_utf8_encode(codepoint, utf8);
        
        if (space >= len) {
            for (int j = 0; j < len; j++) {
                ctx->input_buffer[ctx->input_end] = (uint8_t)utf8[j];
                ctx->input_end = (ctx->input_end + 1) % TUI_INPUT_BUFFER_SIZE;
            }
            bytes_added += len;
        }
    }
    
    return bytes_added;
}

#endif /* TUI_PLATFORM_WINDOWS */

/* ============================================================================
 * Internal Helpers - Input Parsing
 * ============================================================================ */

static int tui_input_available(tui_context* ctx) {
    return (ctx->input_end - ctx->input_start + TUI_INPUT_BUFFER_SIZE) % TUI_INPUT_BUFFER_SIZE;
}

static uint8_t tui_input_peek(tui_context* ctx, int offset) {
    return ctx->input_buffer[(ctx->input_start + offset) % TUI_INPUT_BUFFER_SIZE];
}

static void tui_input_consume(tui_context* ctx, int count) {
    ctx->input_start = (ctx->input_start + count) % TUI_INPUT_BUFFER_SIZE;
}

/* Parse CSI parameters (semicolon separated numbers) */
static int tui_parse_csi_params(tui_context* ctx, int start, int* params, int max_params, int* end_pos, uint8_t* final_char) {
    int available = tui_input_available(ctx);
    int i = start;
    int param_count = 0;
    int current_param = 0;
    bool has_digit = false;
    
    while (i < available && i < 32) {
        uint8_t c = tui_input_peek(ctx, i);
        
        if (c >= '0' && c <= '9') {
            current_param = current_param * 10 + (c - '0');
            has_digit = true;
            i++;
        } else if (c == ';') {
            if (param_count < max_params) {
                params[param_count++] = has_digit ? current_param : 0;
            }
            current_param = 0;
            has_digit = false;
            i++;
        } else if (c >= 0x40 && c <= 0x7E) {
            /* Final character */
            if (param_count < max_params) {
                params[param_count++] = has_digit ? current_param : 0;
            }
            *end_pos = i;
            *final_char = c;
            return param_count;
        } else if (c == '<' || c == '>' || c == '?' || c == '=') {
            /* Private mode indicator, skip */
            i++;
        } else {
            /* Invalid character */
            break;
        }
    }
    
    *end_pos = -1;
    return 0;
}

/* Decode modifier bits from CSI parameter (modifier = 1 + bits) */
static void tui_decode_modifiers(int modifier_param, bool* shift, bool* alt, bool* ctrl) {
    /* Terminal encodes: modifier = 1 + (shift?1:0) + (alt?2:0) + (ctrl?4:0) */
    int bits = modifier_param - 1;
    *shift = (bits & 1) != 0;
    *alt = (bits & 2) != 0;
    *ctrl = (bits & 4) != 0;
}

static int tui_parse_escape_sequence(tui_context* ctx, tui_event* event) {
    int available = tui_input_available(ctx);
    
    if (available < 1) return 0;
    
    /* Initialize event */
    event->type = TUI_EVENT_NONE;
    event->key = TUI_KEY_NONE;
    event->ch = 0;
    event->mouse_button = TUI_MOUSE_NONE;
    event->mouse_x = 0;
    event->mouse_y = 0;
    event->ctrl = false;
    event->alt = false;
    event->shift = false;
    
    uint8_t b0 = tui_input_peek(ctx, 0);
    
    /* Check for escape sequence */
    if (b0 == 0x1B) {
        if (available < 2) {
            return 0;
        }
        
        uint8_t b1 = tui_input_peek(ctx, 1);
        
        if (b1 == '[') {
            /* CSI sequence */
            if (available < 3) return 0;
            
            uint8_t b2 = tui_input_peek(ctx, 2);
            
            /* Check for focus events: ESC [ I (focus in) or ESC [ O (focus out) */
            if (b2 == 'I') {
                event->type = TUI_EVENT_FOCUS_IN;
                tui_input_consume(ctx, 3);
                return 1;
            }
            if (b2 == 'O') {
                event->type = TUI_EVENT_FOCUS_OUT;
                tui_input_consume(ctx, 3);
                return 1;
            }
            
            /* Check for bracketed paste: ESC [ 200 ~ (start) or ESC [ 201 ~ (end) */
            if (b2 == '2' && available >= 6) {
                if (tui_input_peek(ctx, 3) == '0' && 
                    tui_input_peek(ctx, 5) == '~') {
                    uint8_t b4 = tui_input_peek(ctx, 4);
                    if (b4 == '0') {
                        /* ESC [ 200 ~ - paste start */
                        event->type = TUI_EVENT_PASTE_START;
                        ctx->is_pasting = true;
                        tui_input_consume(ctx, 6);
                        return 1;
                    } else if (b4 == '1') {
                        /* ESC [ 201 ~ - paste end */
                        event->type = TUI_EVENT_PASTE_END;
                        ctx->is_pasting = false;
                        tui_input_consume(ctx, 6);
                        return 1;
                    }
                }
            }
            
            /* Check for SGR mouse sequence: ESC [ < ... */
            if (b2 == '<') {
                int params[4] = {0};
                int end_pos;
                uint8_t final_char;
                int param_count = tui_parse_csi_params(ctx, 3, params, 4, &end_pos, &final_char);
                
                if (end_pos < 0) return 0; /* Incomplete */
                
                if ((final_char == 'M' || final_char == 'm') && param_count >= 3) {
                    int button_code = params[0];
                    int mx = params[1] - 1; /* Convert to 0-indexed */
                    int my = params[2] - 1;
                    
                    event->type = TUI_EVENT_MOUSE;
                    event->mouse_x = mx;
                    event->mouse_y = my;
                    
                    if (final_char == 'm') {
                        event->mouse_button = TUI_MOUSE_RELEASE;
                    } else {
                        switch (button_code & 0x43) {
                            case 0: event->mouse_button = TUI_MOUSE_LEFT; break;
                            case 1: event->mouse_button = TUI_MOUSE_MIDDLE; break;
                            case 2: event->mouse_button = TUI_MOUSE_RIGHT; break;
                            case 3: event->mouse_button = TUI_MOUSE_RELEASE; break;
                            case 64: event->mouse_button = TUI_MOUSE_WHEEL_UP; break;
                            case 65: event->mouse_button = TUI_MOUSE_WHEEL_DOWN; break;
                            default:
                                if (button_code & 32) {
                                    event->mouse_button = TUI_MOUSE_MOVE;
                                } else {
                                    event->mouse_button = TUI_MOUSE_NONE;
                                }
                                break;
                        }
                    }
                    
                    tui_input_consume(ctx, end_pos + 1);
                    return 1;
                }
            }
            
            /* Simple arrow keys */
            switch (b2) {
                case 'A':
                    event->type = TUI_EVENT_KEY;
                    event->key = TUI_KEY_UP;
                    tui_input_consume(ctx, 3);
                    return 1;
                case 'B':
                    event->type = TUI_EVENT_KEY;
                    event->key = TUI_KEY_DOWN;
                    tui_input_consume(ctx, 3);
                    return 1;
                case 'C':
                    event->type = TUI_EVENT_KEY;
                    event->key = TUI_KEY_RIGHT;
                    tui_input_consume(ctx, 3);
                    return 1;
                case 'D':
                    event->type = TUI_EVENT_KEY;
                    event->key = TUI_KEY_LEFT;
                    tui_input_consume(ctx, 3);
                    return 1;
                case 'H':
                    event->type = TUI_EVENT_KEY;
                    event->key = TUI_KEY_HOME;
                    tui_input_consume(ctx, 3);
                    return 1;
                case 'F':
                    event->type = TUI_EVENT_KEY;
                    event->key = TUI_KEY_END;
                    tui_input_consume(ctx, 3);
                    return 1;
            }
            
            /* Extended CSI sequences with parameters */
            int params[4] = {0};
            int end_pos;
            uint8_t final_char;
            int param_count = tui_parse_csi_params(ctx, 2, params, 4, &end_pos, &final_char);
            
            if (end_pos < 0) return 0; /* Incomplete */
            
            event->type = TUI_EVENT_KEY;
            
            /* Extract modifier from param[1] if present (format: key;modifier) */
            if (param_count >= 2 && params[1] > 1) {
                tui_decode_modifiers(params[1], &event->shift, &event->alt, &event->ctrl);
            }
            
            /* Handle arrow keys with modifiers: ESC[1;5A = Ctrl+Up */
            if (param_count >= 2 && params[0] == 1) {
                switch (final_char) {
                    case 'A': event->key = TUI_KEY_UP; break;
                    case 'B': event->key = TUI_KEY_DOWN; break;
                    case 'C': event->key = TUI_KEY_RIGHT; break;
                    case 'D': event->key = TUI_KEY_LEFT; break;
                    case 'H': event->key = TUI_KEY_HOME; break;
                    case 'F': event->key = TUI_KEY_END; break;
                    default: event->key = TUI_KEY_NONE; break;
                }
                if (event->key != TUI_KEY_NONE) {
                    tui_input_consume(ctx, end_pos + 1);
                    return 1;
                }
            }
            
            if (final_char == '~' && param_count >= 1) {
                switch (params[0]) {
                    case 1: event->key = TUI_KEY_HOME; break;
                    case 2: event->key = TUI_KEY_INSERT; break;
                    case 3: event->key = TUI_KEY_DELETE; break;
                    case 4: event->key = TUI_KEY_END; break;
                    case 5: event->key = TUI_KEY_PAGEUP; break;
                    case 6: event->key = TUI_KEY_PAGEDOWN; break;
                    case 11: event->key = TUI_KEY_F1; break;
                    case 12: event->key = TUI_KEY_F2; break;
                    case 13: event->key = TUI_KEY_F3; break;
                    case 14: event->key = TUI_KEY_F4; break;
                    case 15: event->key = TUI_KEY_F5; break;
                    case 17: event->key = TUI_KEY_F6; break;
                    case 18: event->key = TUI_KEY_F7; break;
                    case 19: event->key = TUI_KEY_F8; break;
                    case 20: event->key = TUI_KEY_F9; break;
                    case 21: event->key = TUI_KEY_F10; break;
                    case 23: event->key = TUI_KEY_F11; break;
                    case 24: event->key = TUI_KEY_F12; break;
                    default: event->key = TUI_KEY_NONE; break;
                }
            } else {
                event->key = TUI_KEY_NONE;
            }
            
            tui_input_consume(ctx, end_pos + 1);
            return 1;
            
        } else if (b1 == 'O') {
            /* SS3 sequence */
            if (available < 3) return 0;
            
            uint8_t b2 = tui_input_peek(ctx, 2);
            event->type = TUI_EVENT_KEY;
            
            switch (b2) {
                case 'A': event->key = TUI_KEY_UP; break;
                case 'B': event->key = TUI_KEY_DOWN; break;
                case 'C': event->key = TUI_KEY_RIGHT; break;
                case 'D': event->key = TUI_KEY_LEFT; break;
                case 'H': event->key = TUI_KEY_HOME; break;
                case 'F': event->key = TUI_KEY_END; break;
                case 'P': event->key = TUI_KEY_F1; break;
                case 'Q': event->key = TUI_KEY_F2; break;
                case 'R': event->key = TUI_KEY_F3; break;
                case 'S': event->key = TUI_KEY_F4; break;
                default: event->key = TUI_KEY_NONE; break;
            }
            
            tui_input_consume(ctx, 3);
            return 1;
        } else {
            /* Standalone ESC */
            event->type = TUI_EVENT_KEY;
            event->key = TUI_KEY_ESC;
            tui_input_consume(ctx, 1);
            return 1;
        }
    }
    
    /* Control characters */
    if (b0 == '\r' || b0 == '\n') {
        event->type = TUI_EVENT_KEY;
        event->key = TUI_KEY_ENTER;
        tui_input_consume(ctx, 1);
        return 1;
    }
    
    if (b0 == 0x7F || b0 == 0x08) {
        event->type = TUI_EVENT_KEY;
        event->key = TUI_KEY_BACKSPACE;
        tui_input_consume(ctx, 1);
        return 1;
    }
    
    if (b0 == '\t') {
        event->type = TUI_EVENT_KEY;
        event->key = TUI_KEY_TAB;
        event->ch = '\t';
        tui_input_consume(ctx, 1);
        return 1;
    }
    
    if (b0 == ' ') {
        event->type = TUI_EVENT_KEY;
        event->key = TUI_KEY_SPACE;
        event->ch = ' ';
        tui_input_consume(ctx, 1);
        return 1;
    }
    
    /* Ctrl+Space sends NUL (byte 0) */
    if (b0 == 0) {
        event->type = TUI_EVENT_KEY;
        event->key = TUI_KEY_SPACE;
        event->ch = ' ';
        event->ctrl = true;
        tui_input_consume(ctx, 1);
        return 1;
    }
    
    /* Ctrl+letter (ASCII 1-26 except special chars already handled) */
    if (b0 >= 1 && b0 <= 26 && b0 != '\t' && b0 != '\r' && b0 != '\n') {
        event->type = TUI_EVENT_KEY;
        event->key = TUI_KEY_CHAR;
        event->ch = 'a' + (b0 - 1);  /* Convert to lowercase letter */
        event->ctrl = true;
        tui_input_consume(ctx, 1);
        return 1;
    }
    
    /* Regular character (possibly UTF-8) */
    uint32_t codepoint;
    uint8_t temp[4];
    int len = 0;
    
    if ((b0 & 0x80) == 0) {
        len = 1;
    } else if ((b0 & 0xE0) == 0xC0) {
        len = 2;
    } else if ((b0 & 0xF0) == 0xE0) {
        len = 3;
    } else if ((b0 & 0xF8) == 0xF0) {
        len = 4;
    } else {
        tui_input_consume(ctx, 1);
        return 0;
    }
    
    if (available < len) return 0;
    
    for (int i = 0; i < len; i++) {
        temp[i] = tui_input_peek(ctx, i);
    }
    
    tui_utf8_decode(temp, len, &codepoint);
    tui_input_consume(ctx, len);
    
    event->type = TUI_EVENT_KEY;
    event->key = TUI_KEY_CHAR;
    event->ch = codepoint;
    return 1;
}

/* ============================================================================
 * Public API Implementation
 * ============================================================================ */

tui_context* tui_create(void) {
    /* Allocate context */
    tui_context* ctx = (tui_context*)malloc(sizeof(tui_context));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(tui_context));
    
    /* Setup platform-specific terminal handling */
#ifdef TUI_PLATFORM_WINDOWS
    if (tui_win32_setup(ctx) < 0) {
        free(ctx);
        return NULL;
    }
#else
    if (tui_posix_setup(ctx) < 0) {
        free(ctx);
        return NULL;
    }
#endif
    
    /* Get terminal size */
    tui_get_terminal_size(ctx);
    
    /* Allocate buffers */
    size_t buffer_size = (size_t)(TUI_MAX_WIDTH * TUI_MAX_HEIGHT) * sizeof(tui_cell);
    ctx->front_buffer = (tui_cell*)malloc(buffer_size);
    ctx->back_buffer = (tui_cell*)malloc(buffer_size);
    
    if (!ctx->front_buffer || !ctx->back_buffer) {
#ifdef TUI_PLATFORM_WINDOWS
        tui_win32_cleanup(ctx);
#else
        tui_posix_cleanup(ctx);
#endif
        free(ctx->front_buffer);
        free(ctx->back_buffer);
        free(ctx);
        return NULL;
    }
    
    /* Initialize buffers */
    tui_clear_buffer(ctx->front_buffer, TUI_MAX_WIDTH, TUI_MAX_HEIGHT);
    tui_clear_buffer(ctx->back_buffer, TUI_MAX_WIDTH, TUI_MAX_HEIGHT);
    
    /* Initialize drawing state */
    ctx->current_fg = TUI_COLOR_DEFAULT;
    ctx->current_bg = TUI_COLOR_DEFAULT;
    ctx->current_style = TUI_STYLE_NONE;
    ctx->current_underline_color = TUI_COLOR_DEFAULT;
    
    /* Initialize cursor state */
    ctx->cursor_x = 0;
    ctx->cursor_y = 0;
    ctx->cursor_visible = false;
    ctx->cursor_shape = TUI_CURSOR_DEFAULT;
    
    /* Initialize button state */
    ctx->hot_button_x = -1;
    ctx->hot_button_y = -1;
    ctx->button_pressed = false;
    
    /* Initialize feature flags */
    ctx->bracketed_paste_enabled = false;
    ctx->focus_events_enabled = false;
    ctx->is_pasting = false;
    
    /* Initialize theme */
    ctx->theme = &TUI_THEME_DEFAULT;
    
    /* Enter alternate screen and hide cursor */
    tui_ansi_enter_alt_screen(ctx);
    tui_ansi_hide_cursor(ctx);
    tui_ansi_clear_screen(ctx);
    tui_output_flush(ctx);
    
    ctx->initialized = true;
    ctx->needs_redraw = true;  /* Force initial full draw */
    
    return ctx;
}

void tui_destroy(tui_context* ctx) {
    if (!ctx) return;
    
    if (ctx->initialized) {
        /* Disable features */
        if (ctx->mouse_enabled) {
            tui_ansi_disable_mouse(ctx);
        }
        if (ctx->bracketed_paste_enabled) {
            tui_ansi_disable_bracketed_paste(ctx);
        }
        if (ctx->focus_events_enabled) {
            tui_ansi_disable_focus_events(ctx);
        }
        
        /* Reset cursor shape to default */
        tui_ansi_set_cursor_shape(ctx, TUI_CURSOR_DEFAULT);
        
        /* Show cursor and leave alternate screen */
        tui_ansi_show_cursor(ctx);
        tui_ansi_reset(ctx);
        tui_ansi_leave_alt_screen(ctx);
        tui_output_flush(ctx);
        
        /* Restore terminal settings */
#ifdef TUI_PLATFORM_WINDOWS
        tui_win32_cleanup(ctx);
#else
        tui_posix_cleanup(ctx);
#endif
    }
    
    free(ctx->front_buffer);
    free(ctx->back_buffer);
    free(ctx);
}

void tui_clear(tui_context* ctx) {
    if (!ctx || !ctx->in_frame) return;
    
    /* Fill entire visible area with current background color */
    for (int y = 0; y < ctx->height; y++) {
        for (int x = 0; x < ctx->width; x++) {
            tui_set_cell(ctx, x, y, ' ');
        }
    }
}

void tui_begin_frame(tui_context* ctx) {
    if (!ctx || !ctx->initialized) return;
    
    /* Update terminal size */
    tui_get_terminal_size(ctx);
    
    /* Clear back buffer - use MAX_WIDTH/MAX_HEIGHT to clear entire buffer */
    tui_clear_buffer(ctx->back_buffer, TUI_MAX_WIDTH, TUI_MAX_HEIGHT);
    
    /* Reset drawing state */
    ctx->current_fg = TUI_COLOR_DEFAULT;
    ctx->current_bg = TUI_COLOR_DEFAULT;
    ctx->current_style = TUI_STYLE_NONE;
    
    /* Reset button state for this frame */
    ctx->button_pressed = false;
    
    ctx->in_frame = true;
}

void tui_end_frame(tui_context* ctx) {
    if (!ctx || !ctx->initialized || !ctx->in_frame) return;
    
    /* Handle full redraw (after resize) - like neovim */
    if (ctx->needs_redraw) {
        ctx->needs_redraw = false;
        /* Reset attributes and clear screen */
        tui_output_str(ctx, "\x1b[0m");    /* Reset all attributes */
        tui_output_str(ctx, "\x1b[2J");    /* Clear entire screen */
        tui_output_str(ctx, "\x1b[H");     /* Move cursor home */
        tui_output_flush(ctx);
        /* Clear front buffer so all cells will be redrawn */
        tui_clear_buffer(ctx->front_buffer, TUI_MAX_WIDTH, TUI_MAX_HEIGHT);
    }
    
    /* Begin synchronized output (prevents tearing on supported terminals) */
    tui_ansi_begin_sync(ctx);
    
    /* Diff-based rendering */
    uint32_t last_fg = 0xFFFFFFFF;
    uint32_t last_bg = 0xFFFFFFFF;
    uint32_t last_underline_color = 0xFFFFFFFF;
    uint8_t last_style = 0xFF;
    int last_x = -2;
    int last_y = -2;
    
    for (int y = 0; y < ctx->height; y++) {
        for (int x = 0; x < ctx->width; x++) {
            int idx = y * TUI_MAX_WIDTH + x;
            tui_cell* front = &ctx->front_buffer[idx];
            tui_cell* back = &ctx->back_buffer[idx];
            
            if (!tui_cell_equal(front, back)) {
                /* Cell has changed, need to update */
                
                /* Move cursor if not adjacent */
                if (last_x != x - 1 || last_y != y) {
                    tui_ansi_move_cursor(ctx, x, y);
                }
                
                /* Update style if changed */
                if (back->style != last_style) {
                    tui_ansi_reset(ctx);
                    tui_ansi_set_style(ctx, back->style);
                    last_style = back->style;
                    last_fg = 0xFFFFFFFF;
                    last_bg = 0xFFFFFFFF;
                    last_underline_color = 0xFFFFFFFF;
                }
                
                /* Update foreground if changed */
                if (back->fg != last_fg) {
                    tui_ansi_set_fg(ctx, back->fg);
                    last_fg = back->fg;
                }
                
                /* Update background if changed */
                if (back->bg != last_bg) {
                    tui_ansi_set_bg(ctx, back->bg);
                    last_bg = back->bg;
                }
                
                /* Update underline color if changed (for undercurl/colored underlines) */
                if (back->underline_color != last_underline_color) {
                    tui_ansi_set_underline_color(ctx, back->underline_color);
                    last_underline_color = back->underline_color;
                }
                
                /* Output character */
                char utf8[4];
                int len = tui_utf8_encode(back->codepoint, utf8);
                tui_output_write(ctx, utf8, len);
                
                /* Update front buffer */
                *front = *back;
                
                last_x = x;
                last_y = y;
            }
        }
    }
    
    /* Position cursor */
    if (ctx->cursor_visible) {
        tui_ansi_move_cursor(ctx, ctx->cursor_x, ctx->cursor_y);
        tui_ansi_show_cursor(ctx);
    }
    
    /* End synchronized output */
    tui_ansi_end_sync(ctx);
    
    tui_output_flush(ctx);
    
    ctx->in_frame = false;
}

int tui_get_width(tui_context* ctx) {
    return ctx ? ctx->width : 0;
}

int tui_get_height(tui_context* ctx) {
    return ctx ? ctx->height : 0;
}

void tui_set_fg(tui_context* ctx, uint32_t color) {
    if (ctx) ctx->current_fg = color;
}

void tui_set_bg(tui_context* ctx, uint32_t color) {
    if (ctx) ctx->current_bg = color;
}

void tui_set_style(tui_context* ctx, uint8_t style) {
    if (ctx) ctx->current_style = style;
}

void tui_set_cell(tui_context* ctx, int x, int y, uint32_t codepoint) {
    if (!ctx || !ctx->in_frame) return;
    if (x < 0 || x >= ctx->width || y < 0 || y >= ctx->height) return;
    
    int idx = y * TUI_MAX_WIDTH + x;
    ctx->back_buffer[idx].codepoint = codepoint;
    ctx->back_buffer[idx].fg = ctx->current_fg;
    ctx->back_buffer[idx].bg = ctx->current_bg;
    ctx->back_buffer[idx].underline_color = ctx->current_underline_color;
    ctx->back_buffer[idx].style = ctx->current_style;
}

void tui_label(tui_context* ctx, int x, int y, const char* text) {
    if (!ctx || !ctx->in_frame || !text) return;
    
    const uint8_t* ptr = (const uint8_t*)text;
    int len = (int)strlen(text);
    int pos = 0;
    int cur_x = x;
    
    while (pos < len && cur_x < ctx->width) {
        uint32_t codepoint;
        int char_len = tui_utf8_decode(ptr + pos, len - pos, &codepoint);
        
        if (codepoint == '\n') {
            y++;
            cur_x = x;
        } else if (codepoint >= 32) {
            int char_width = tui_char_width(codepoint);
            if (char_width == 2) {
                /* Wide character (CJK, emoji) */
                if (cur_x + 1 < ctx->width) {
                    tui_set_cell_wide(ctx, cur_x, y, codepoint);
                    cur_x += 2;
                } else {
                    /* Not enough space, skip */
                    cur_x++;
                }
            } else if (char_width == 1) {
                tui_set_cell(ctx, cur_x, y, codepoint);
                cur_x++;
            }
            /* char_width == 0 means combining char, don't advance */
        }
        
        pos += char_len;
    }
}

int tui_button(tui_context* ctx, int x, int y, const char* text) {
    if (!ctx || !ctx->in_frame || !text) return 0;
    
    const uint8_t* ptr = (const uint8_t*)text;
    int byte_len = (int)strlen(text);
    
    /* Check if this button is "hot" (focused) */
    bool is_hot = (ctx->hot_button_x == x && ctx->hot_button_y == y);
    
    /* Save current colors */
    uint32_t save_fg = ctx->current_fg;
    uint32_t save_bg = ctx->current_bg;
    uint8_t save_style = ctx->current_style;
    
    /* Set button colors */
    if (is_hot) {
        ctx->current_fg = TUI_COLOR_BLACK;
        ctx->current_bg = TUI_COLOR_WHITE;
        ctx->current_style = TUI_STYLE_BOLD;
    } else {
        ctx->current_fg = TUI_COLOR_WHITE;
        ctx->current_bg = TUI_RGB(60, 60, 60);
        ctx->current_style = TUI_STYLE_NONE;
    }
    
    /* Draw button frame */
    tui_set_cell(ctx, x, y, '[');
    tui_set_cell(ctx, x + 1, y, ' ');
    
    /* Draw button text */
    int pos = 0;
    int cur_x = x + 2;
    while (pos < byte_len) {
        uint32_t codepoint;
        int char_len = tui_utf8_decode(ptr + pos, byte_len - pos, &codepoint);
        tui_set_cell(ctx, cur_x, y, codepoint);
        cur_x++;
        pos += char_len;
    }
    
    tui_set_cell(ctx, cur_x, y, ' ');
    tui_set_cell(ctx, cur_x + 1, y, ']');
    
    /* Restore colors */
    ctx->current_fg = save_fg;
    ctx->current_bg = save_bg;
    ctx->current_style = save_style;
    
    /* Return 1 if button was pressed */
    return (is_hot && ctx->button_pressed) ? 1 : 0;
}

int tui_poll_event(tui_context* ctx, tui_event* event) {
    if (!ctx || !event) return 0;
    
    /* Initialize event */
    event->type = TUI_EVENT_NONE;
    event->key = TUI_KEY_NONE;
    event->ch = 0;
    event->mouse_button = TUI_MOUSE_NONE;
    event->mouse_x = 0;
    event->mouse_y = 0;
    event->new_width = 0;
    event->new_height = 0;
    
#ifdef TUI_PLATFORM_POSIX
    /* Check for resize signal */
    if (tui_sigwinch_received) {
        tui_sigwinch_received = 0;
        int old_w = ctx->width;
        int old_h = ctx->height;
        tui_get_terminal_size(ctx);
        if (ctx->width != old_w || ctx->height != old_h) {
            ctx->resized = true;
            ctx->needs_redraw = true;  /* Mark for full redraw */
            event->type = TUI_EVENT_RESIZE;
            event->new_width = ctx->width;
            event->new_height = ctx->height;
            return 1;
        }
    }
#endif
    
    /* Read more input from terminal */
#ifdef TUI_PLATFORM_WINDOWS
    tui_win32_read_input(ctx);
#else
    tui_posix_read_input(ctx);
#endif
    
    /* Parse input buffer */
    if (tui_input_available(ctx) > 0) {
        if (tui_parse_escape_sequence(ctx, event)) {
            /* Update mouse state */
            if (event->type == TUI_EVENT_MOUSE) {
                ctx->mouse_x = event->mouse_x;
                ctx->mouse_y = event->mouse_y;
                ctx->mouse_button = event->mouse_button;
            }
            /* Update button state for navigation */
            if (event->key == TUI_KEY_ENTER) {
                ctx->button_pressed = true;
            }
            return 1;
        }
    }
    
    return 0;
}

void tui_set_cursor(tui_context* ctx, int x, int y) {
    if (ctx) {
        ctx->cursor_x = x;
        ctx->cursor_y = y;
    }
}

void tui_show_cursor(tui_context* ctx, bool show) {
    if (ctx) {
        ctx->cursor_visible = show;
        if (!ctx->in_frame) {
            if (show) {
                tui_ansi_show_cursor(ctx);
            } else {
                tui_ansi_hide_cursor(ctx);
            }
            tui_output_flush(ctx);
        }
    }
}

void tui_enable_mouse(tui_context* ctx) {
    if (ctx && !ctx->mouse_enabled) {
        ctx->mouse_enabled = true;
        tui_ansi_enable_mouse(ctx);
        tui_output_flush(ctx);
    }
}

void tui_disable_mouse(tui_context* ctx) {
    if (ctx && ctx->mouse_enabled) {
        ctx->mouse_enabled = false;
        tui_ansi_disable_mouse(ctx);
        tui_output_flush(ctx);
    }
}

bool tui_resized(tui_context* ctx) {
    if (!ctx) return false;
    bool r = ctx->resized;
    ctx->resized = false;
    return r;
}

/* ============================================================================
 * Cursor Shape
 * ============================================================================ */

void tui_set_cursor_shape(tui_context* ctx, tui_cursor_shape shape) {
    if (!ctx) return;
    ctx->cursor_shape = shape;
    tui_ansi_set_cursor_shape(ctx, shape);
    tui_output_flush(ctx);
}

/* ============================================================================
 * Underline Color
 * ============================================================================ */

void tui_set_underline_color(tui_context* ctx, uint32_t color) {
    if (ctx) ctx->current_underline_color = color;
}

void tui_reset_underline_color(tui_context* ctx) {
    if (ctx) ctx->current_underline_color = TUI_COLOR_DEFAULT;
}

/* ============================================================================
 * Bracketed Paste Mode
 * ============================================================================ */

void tui_enable_bracketed_paste(tui_context* ctx) {
    if (ctx && !ctx->bracketed_paste_enabled) {
        ctx->bracketed_paste_enabled = true;
        tui_ansi_enable_bracketed_paste(ctx);
        tui_output_flush(ctx);
    }
}

void tui_disable_bracketed_paste(tui_context* ctx) {
    if (ctx && ctx->bracketed_paste_enabled) {
        ctx->bracketed_paste_enabled = false;
        tui_ansi_disable_bracketed_paste(ctx);
        tui_output_flush(ctx);
    }
}

bool tui_is_pasting(tui_context* ctx) {
    return ctx ? ctx->is_pasting : false;
}

/* ============================================================================
 * Focus Events
 * ============================================================================ */

void tui_enable_focus_events(tui_context* ctx) {
    if (ctx && !ctx->focus_events_enabled) {
        ctx->focus_events_enabled = true;
        tui_ansi_enable_focus_events(ctx);
        tui_output_flush(ctx);
    }
}

void tui_disable_focus_events(tui_context* ctx) {
    if (ctx && ctx->focus_events_enabled) {
        ctx->focus_events_enabled = false;
        tui_ansi_disable_focus_events(ctx);
        tui_output_flush(ctx);
    }
}

/* ============================================================================
 * Synchronized Output
 * ============================================================================ */

void tui_begin_sync(tui_context* ctx) {
    if (ctx) {
        tui_ansi_begin_sync(ctx);
    }
}

void tui_end_sync(tui_context* ctx) {
    if (ctx) {
        tui_ansi_end_sync(ctx);
        tui_output_flush(ctx);
    }
}

/* ============================================================================
 * Clipboard (OSC 52)
 * ============================================================================ */

void tui_clipboard_set(tui_context* ctx, const char* text) {
    if (!ctx || !text) return;
    
    int text_len = (int)strlen(text);
    int base64_size = ((text_len + 2) / 3) * 4 + 1;
    
    /* Limit to reasonable size */
    if (base64_size > 65536) return;
    
    char* base64 = (char*)malloc((size_t)base64_size);
    if (!base64) return;
    
    tui_base64_encode(text, text_len, base64, base64_size);
    tui_ansi_set_clipboard(ctx, base64);
    tui_output_flush(ctx);
    
    free(base64);
}

/* ============================================================================
 * Hyperlinks (OSC 8)
 * ============================================================================ */

void tui_hyperlink_start(tui_context* ctx, const char* url) {
    if (ctx && url) {
        tui_ansi_hyperlink_start(ctx, url);
    }
}

void tui_hyperlink_end(tui_context* ctx) {
    if (ctx) {
        tui_ansi_hyperlink_end(ctx);
    }
}

/* ============================================================================
 * Wide Character Support
 * ============================================================================ */

void tui_set_cell_wide(tui_context* ctx, int x, int y, uint32_t codepoint) {
    if (!ctx || !ctx->in_frame) return;
    if (x < 0 || x >= ctx->width - 1 || y < 0 || y >= ctx->height) return;
    
    /* Set the wide character in first cell */
    int idx = y * TUI_MAX_WIDTH + x;
    ctx->back_buffer[idx].codepoint = codepoint;
    ctx->back_buffer[idx].fg = ctx->current_fg;
    ctx->back_buffer[idx].bg = ctx->current_bg;
    ctx->back_buffer[idx].underline_color = ctx->current_underline_color;
    ctx->back_buffer[idx].style = ctx->current_style;
    
    /* Set second cell as a continuation marker (space with same bg) */
    int idx2 = y * TUI_MAX_WIDTH + x + 1;
    ctx->back_buffer[idx2].codepoint = ' ';
    ctx->back_buffer[idx2].fg = ctx->current_fg;
    ctx->back_buffer[idx2].bg = ctx->current_bg;
    ctx->back_buffer[idx2].underline_color = ctx->current_underline_color;
    ctx->back_buffer[idx2].style = ctx->current_style;
}

/* ============================================================================
 * Box Drawing Characters (Unicode)
 * ============================================================================ */

/* Border character indices */
#define TUI_BOX_TL 0  /* Top left */
#define TUI_BOX_TR 1  /* Top right */
#define TUI_BOX_BL 2  /* Bottom left */
#define TUI_BOX_BR 3  /* Bottom right */
#define TUI_BOX_H  4  /* Horizontal */
#define TUI_BOX_V  5  /* Vertical */

static const uint32_t tui_box_chars[][6] = {
    /* NONE */    {' ', ' ', ' ', ' ', ' ', ' '},
    /* SINGLE */  {0x250C, 0x2510, 0x2514, 0x2518, 0x2500, 0x2502},
    /* DOUBLE */  {0x2554, 0x2557, 0x255A, 0x255D, 0x2550, 0x2551},
    /* ROUNDED */ {0x256D, 0x256E, 0x2570, 0x256F, 0x2500, 0x2502},
    /* BOLD */    {0x250F, 0x2513, 0x2517, 0x251B, 0x2501, 0x2503},
    /* ASCII */   {'+', '+', '+', '+', '-', '|'}
};

void tui_box(tui_context* ctx, int x, int y, int w, int h, tui_border_style style) {
    if (!ctx || !ctx->in_frame || w < 2 || h < 2) return;
    if (style < 0 || style > TUI_BORDER_ASCII) style = TUI_BORDER_SINGLE;
    
    const uint32_t* chars = tui_box_chars[style];
    
    /* Corners */
    tui_set_cell(ctx, x, y, chars[TUI_BOX_TL]);
    tui_set_cell(ctx, x + w - 1, y, chars[TUI_BOX_TR]);
    tui_set_cell(ctx, x, y + h - 1, chars[TUI_BOX_BL]);
    tui_set_cell(ctx, x + w - 1, y + h - 1, chars[TUI_BOX_BR]);
    
    /* Horizontal lines */
    for (int i = 1; i < w - 1; i++) {
        tui_set_cell(ctx, x + i, y, chars[TUI_BOX_H]);
        tui_set_cell(ctx, x + i, y + h - 1, chars[TUI_BOX_H]);
    }
    
    /* Vertical lines */
    for (int i = 1; i < h - 1; i++) {
        tui_set_cell(ctx, x, y + i, chars[TUI_BOX_V]);
        tui_set_cell(ctx, x + w - 1, y + i, chars[TUI_BOX_V]);
    }
}

void tui_fill(tui_context* ctx, int x, int y, int w, int h, uint32_t ch) {
    if (!ctx || !ctx->in_frame) return;
    
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            tui_set_cell(ctx, x + i, y + j, ch);
        }
    }
}

void tui_hline(tui_context* ctx, int x, int y, int len, uint32_t ch) {
    if (!ctx || !ctx->in_frame) return;
    for (int i = 0; i < len; i++) {
        tui_set_cell(ctx, x + i, y, ch);
    }
}

void tui_vline(tui_context* ctx, int x, int y, int len, uint32_t ch) {
    if (!ctx || !ctx->in_frame) return;
    for (int i = 0; i < len; i++) {
        tui_set_cell(ctx, x, y + i, ch);
    }
}

int tui_text_width(const char* text) {
    if (!text) return 0;
    
    const uint8_t* ptr = (const uint8_t*)text;
    int len = (int)strlen(text);
    int pos = 0;
    int width = 0;
    
    while (pos < len) {
        uint8_t b = ptr[pos];
        int char_len = 1;
        
        if ((b & 0x80) == 0) {
            char_len = 1;
        } else if ((b & 0xE0) == 0xC0) {
            char_len = 2;
        } else if ((b & 0xF0) == 0xE0) {
            char_len = 3;
        } else if ((b & 0xF8) == 0xF0) {
            char_len = 4;
        }
        
        width++;
        pos += char_len;
    }
    
    return width;
}

void tui_label_aligned(tui_context* ctx, int x, int y, int width, const char* text, tui_align align) {
    if (!ctx || !ctx->in_frame || !text) return;
    
    int text_w = tui_text_width(text);
    int offset = 0;
    
    switch (align) {
        case TUI_ALIGN_LEFT:
            offset = 0;
            break;
        case TUI_ALIGN_CENTER:
            offset = (width - text_w) / 2;
            break;
        case TUI_ALIGN_RIGHT:
            offset = width - text_w;
            break;
    }
    
    if (offset < 0) offset = 0;
    
    /* Clear the area first */
    for (int i = 0; i < width; i++) {
        tui_set_cell(ctx, x + i, y, ' ');
    }
    
    /* Draw text at offset */
    tui_label(ctx, x + offset, y, text);
}

/* ============================================================================
 * Popup/Modal Widget
 * ============================================================================ */

void tui_popup_box(tui_context* ctx, int x, int y, int w, int h, const char* title, 
                   tui_border_style style) {
    if (!ctx || !ctx->in_frame || w < 4 || h < 3) return;
    
    uint32_t save_fg = ctx->current_fg;
    uint32_t save_bg = ctx->current_bg;
    
    /* Fill background */
    ctx->current_bg = TUI_RGB(30, 30, 30);
    tui_fill(ctx, x + 1, y + 1, w - 2, h - 2, ' ');
    
    /* Draw border */
    ctx->current_fg = TUI_COLOR_WHITE;
    tui_box(ctx, x, y, w, h, style);
    
    /* Draw title if provided */
    if (title && *title) {
        int title_len = tui_text_width(title);
        int title_x = x + (w - title_len - 2) / 2;
        
        ctx->current_fg = TUI_COLOR_YELLOW;
        ctx->current_bg = TUI_RGB(30, 30, 30);
        tui_set_cell(ctx, title_x, y, ' ');
        tui_label(ctx, title_x + 1, y, title);
        tui_set_cell(ctx, title_x + title_len + 1, y, ' ');
    }
    
    ctx->current_fg = save_fg;
    ctx->current_bg = save_bg;
}

/* ============================================================================
 * Text Wrapping
 * ============================================================================ */

int tui_wrap_text(tui_context* ctx, int x, int y, int width, int max_lines, const char* text) {
    if (!ctx || !ctx->in_frame || !text || width < 1) return 0;
    
    const uint8_t* ptr = (const uint8_t*)text;
    int len = (int)strlen(text);
    int pos = 0;
    int line = 0;
    int col = 0;
    
    while (pos < len && (max_lines <= 0 || line < max_lines)) {
        uint32_t codepoint;
        int char_len = tui_utf8_decode(ptr + pos, len - pos, &codepoint);
        
        if (codepoint == '\n') {
            line++;
            col = 0;
        } else if (codepoint >= 32) {
            if (col >= width) {
                line++;
                col = 0;
                if (max_lines > 0 && line >= max_lines) break;
            }
            tui_set_cell(ctx, x + col, y + line, codepoint);
            col++;
        }
        
        pos += char_len;
    }
    
    return line + 1;
}

/* ============================================================================
 * Theme Definitions
 * ============================================================================ */

const tui_theme TUI_THEME_DEFAULT = {
    .bg = TUI_COLOR_DEFAULT,
    .fg = TUI_COLOR_DEFAULT,
    .fg_dim = TUI_RGB(128, 128, 128),
    .widget_bg = TUI_RGB(40, 40, 40),
    .widget_fg = TUI_RGB(220, 220, 220),
    .widget_border = TUI_RGB(80, 80, 80),
    .focus_bg = TUI_RGB(60, 60, 100),
    .focus_fg = TUI_COLOR_WHITE,
    .select_bg = TUI_RGB(0, 120, 180),
    .select_fg = TUI_COLOR_WHITE,
    .accent = TUI_RGB(100, 150, 255),
    .accent_dim = TUI_RGB(60, 90, 160),
    .success = TUI_RGB(100, 200, 100),
    .warning = TUI_RGB(220, 180, 50),
    .error = TUI_RGB(220, 80, 80),
    .info = TUI_RGB(100, 180, 220),
    .border_style = TUI_BORDER_SINGLE
};

const tui_theme TUI_THEME_DARK = {
    .bg = TUI_RGB(20, 20, 25),
    .fg = TUI_RGB(200, 200, 200),
    .fg_dim = TUI_RGB(100, 100, 100),
    .widget_bg = TUI_RGB(35, 35, 40),
    .widget_fg = TUI_RGB(200, 200, 200),
    .widget_border = TUI_RGB(60, 60, 70),
    .focus_bg = TUI_RGB(50, 80, 120),
    .focus_fg = TUI_COLOR_WHITE,
    .select_bg = TUI_RGB(70, 100, 140),
    .select_fg = TUI_COLOR_WHITE,
    .accent = TUI_RGB(80, 140, 220),
    .accent_dim = TUI_RGB(50, 90, 140),
    .success = TUI_RGB(80, 180, 80),
    .warning = TUI_RGB(200, 160, 40),
    .error = TUI_RGB(200, 60, 60),
    .info = TUI_RGB(80, 160, 200),
    .border_style = TUI_BORDER_ROUNDED
};

const tui_theme TUI_THEME_LIGHT = {
    .bg = TUI_RGB(240, 240, 240),
    .fg = TUI_RGB(30, 30, 30),
    .fg_dim = TUI_RGB(120, 120, 120),
    .widget_bg = TUI_RGB(255, 255, 255),
    .widget_fg = TUI_RGB(30, 30, 30),
    .widget_border = TUI_RGB(180, 180, 180),
    .focus_bg = TUI_RGB(200, 220, 255),
    .focus_fg = TUI_RGB(20, 20, 20),
    .select_bg = TUI_RGB(0, 120, 200),
    .select_fg = TUI_COLOR_WHITE,
    .accent = TUI_RGB(0, 100, 180),
    .accent_dim = TUI_RGB(100, 150, 200),
    .success = TUI_RGB(40, 160, 40),
    .warning = TUI_RGB(200, 140, 0),
    .error = TUI_RGB(200, 40, 40),
    .info = TUI_RGB(40, 140, 200),
    .border_style = TUI_BORDER_SINGLE
};

const tui_theme TUI_THEME_BLUE = {
    .bg = TUI_RGB(15, 25, 45),
    .fg = TUI_RGB(180, 200, 230),
    .fg_dim = TUI_RGB(80, 100, 140),
    .widget_bg = TUI_RGB(25, 40, 70),
    .widget_fg = TUI_RGB(180, 200, 230),
    .widget_border = TUI_RGB(50, 80, 130),
    .focus_bg = TUI_RGB(40, 80, 140),
    .focus_fg = TUI_COLOR_WHITE,
    .select_bg = TUI_RGB(60, 120, 200),
    .select_fg = TUI_COLOR_WHITE,
    .accent = TUI_RGB(100, 180, 255),
    .accent_dim = TUI_RGB(60, 120, 180),
    .success = TUI_RGB(80, 200, 120),
    .warning = TUI_RGB(230, 180, 60),
    .error = TUI_RGB(230, 80, 80),
    .info = TUI_RGB(100, 180, 240),
    .border_style = TUI_BORDER_DOUBLE
};

const tui_theme TUI_THEME_GREEN = {
    .bg = TUI_RGB(15, 30, 20),
    .fg = TUI_RGB(180, 230, 180),
    .fg_dim = TUI_RGB(80, 120, 80),
    .widget_bg = TUI_RGB(25, 50, 35),
    .widget_fg = TUI_RGB(180, 230, 180),
    .widget_border = TUI_RGB(60, 100, 70),
    .focus_bg = TUI_RGB(40, 100, 60),
    .focus_fg = TUI_COLOR_WHITE,
    .select_bg = TUI_RGB(60, 140, 80),
    .select_fg = TUI_COLOR_WHITE,
    .accent = TUI_RGB(100, 220, 130),
    .accent_dim = TUI_RGB(60, 150, 90),
    .success = TUI_RGB(100, 230, 130),
    .warning = TUI_RGB(220, 200, 80),
    .error = TUI_RGB(220, 100, 100),
    .info = TUI_RGB(100, 200, 180),
    .border_style = TUI_BORDER_BOLD
};

void tui_set_theme(tui_context* ctx, const tui_theme* theme) {
    if (ctx) {
        ctx->theme = theme ? theme : &TUI_THEME_DEFAULT;
    }
}

const tui_theme* tui_get_theme(tui_context* ctx) {
    if (ctx && ctx->theme) {
        return ctx->theme;
    }
    return &TUI_THEME_DEFAULT;
}

/* ============================================================================
 * Animation Helpers
 * ============================================================================ */

uint32_t tui_lerp_color(uint32_t from, uint32_t to, float t) {
    if (t <= 0.0f) return from;
    if (t >= 1.0f) return to;
    
    int r1 = (int)((from >> 16) & 0xFF);
    int g1 = (int)((from >> 8) & 0xFF);
    int b1 = (int)(from & 0xFF);
    
    int r2 = (int)((to >> 16) & 0xFF);
    int g2 = (int)((to >> 8) & 0xFF);
    int b2 = (int)(to & 0xFF);
    
    int r = r1 + (int)((float)(r2 - r1) * t);
    int g = g1 + (int)((float)(g2 - g1) * t);
    int b = b1 + (int)((float)(b2 - b1) * t);
    
    return TUI_RGB(r, g, b);
}

float tui_ease_in_out(float t) {
    if (t < 0.5f) {
        return 2.0f * t * t;
    }
    t = t - 0.5f;
    return 0.5f + t * (2.0f - 2.0f * t);
}

/* ============================================================================
 * Hierarchical Widget System - Implementation
 * ============================================================================ */

/* Create a new widget */
tui_widget* tui_widget_create(tui_widget_type type) {
    tui_widget* w = (tui_widget*)calloc(1, sizeof(tui_widget));
    if (!w) return NULL;
    
    w->type = type;
    w->id = -1;
    w->visible = true;
    w->enabled = true;
    w->focusable = (type == TUI_WIDGET_BUTTON || 
                    type == TUI_WIDGET_TEXTBOX || 
                    type == TUI_WIDGET_TEXTAREA ||
                    type == TUI_WIDGET_CHECKBOX ||
                    type == TUI_WIDGET_RADIO ||
                    type == TUI_WIDGET_SLIDER ||
                    type == TUI_WIDGET_SPINNER ||
                    type == TUI_WIDGET_TABS ||
                    type == TUI_WIDGET_LIST);
    w->tab_index = w->focusable ? 0 : -1;
    w->bg_color = TUI_COLOR_DEFAULT;
    w->fg_color = TUI_COLOR_DEFAULT;
    w->border_style = TUI_BORDER_NONE;
    
    /* Type-specific initialization */
    if (type == TUI_WIDGET_TEXTAREA) {
        w->state.textarea.sel_start_row = -1;  /* No selection */
    } else if (type == TUI_WIDGET_SPLITTER) {
        w->state.splitter.ratio = 0.5f;
        w->state.splitter.min_size = 3;
    }
    
    return w;
}

/* Destroy a widget (not recursive) */
void tui_widget_destroy(tui_widget* widget) {
    if (widget) {
        free(widget);
    }
}

/* Destroy widget and all children */
void tui_widget_destroy_recursive(tui_widget* widget) {
    if (!widget) return;
    
    for (int i = 0; i < widget->child_count; i++) {
        tui_widget_destroy_recursive(widget->children[i]);
    }
    
    tui_widget_destroy(widget);
}

/* Add a child widget */
void tui_widget_add_child(tui_widget* parent, tui_widget* child) {
    if (!parent || !child) return;
    if (parent->child_count >= TUI_MAX_CHILDREN) return;
    
    /* Remove from current parent if any */
    if (child->parent) {
        tui_widget_remove_child(child->parent, child);
    }
    
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

/* Remove a child widget */
void tui_widget_remove_child(tui_widget* parent, tui_widget* child) {
    if (!parent || !child) return;
    
    for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == child) {
            /* Shift remaining children */
            for (int j = i; j < parent->child_count - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->child_count--;
            child->parent = NULL;
            return;
        }
    }
}

/* Find widget by ID */
tui_widget* tui_widget_find_by_id(tui_widget* root, int id) {
    if (!root) return NULL;
    if (root->id == id) return root;
    
    for (int i = 0; i < root->child_count; i++) {
        tui_widget* found = tui_widget_find_by_id(root->children[i], id);
        if (found) return found;
    }
    
    return NULL;
}

/* Find widget by name */
tui_widget* tui_widget_find_by_name(tui_widget* root, const char* name) {
    if (!root || !name) return NULL;
    if (root->name && strcmp(root->name, name) == 0) return root;
    
    for (int i = 0; i < root->child_count; i++) {
        tui_widget* found = tui_widget_find_by_name(root->children[i], name);
        if (found) return found;
    }
    
    return NULL;
}

/* Set widget bounds */
void tui_widget_set_bounds(tui_widget* w, int x, int y, int width, int height) {
    if (!w) return;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
}

/* Get absolute bounds (accounting for parent positions) */
void tui_widget_get_absolute_bounds(tui_widget* w, int* x, int* y, int* width, int* height) {
    if (!w) return;
    
    int abs_x = w->x;
    int abs_y = w->y;
    
    tui_widget* p = w->parent;
    while (p) {
        abs_x += p->x;
        abs_y += p->y;
        p = p->parent;
    }
    
    if (x) *x = abs_x;
    if (y) *y = abs_y;
    if (width) *width = w->width;
    if (height) *height = w->height;
}

/* Check if point is inside widget */
bool tui_widget_contains_point(tui_widget* w, int px, int py) {
    if (!w) return false;
    
    int x, y, width, height;
    tui_widget_get_absolute_bounds(w, &x, &y, &width, &height);
    
    return px >= x && px < x + width && py >= y && py < y + height;
}

/* Register event handler (bubble phase) */
void tui_widget_on(tui_widget* w, tui_event_type type, tui_event_handler handler, void* userdata) {
    if (!w || !handler) return;
    if (w->handler_count >= TUI_MAX_HANDLERS) return;
    
    tui_handler_entry* entry = &w->handlers[w->handler_count++];
    entry->event_type = type;
    entry->handler = handler;
    entry->userdata = userdata;
    entry->capture = false;
}

/* Register event handler (capture phase) */
void tui_widget_on_capture(tui_widget* w, tui_event_type type, tui_event_handler handler, void* userdata) {
    if (!w || !handler) return;
    if (w->handler_count >= TUI_MAX_HANDLERS) return;
    
    tui_handler_entry* entry = &w->handlers[w->handler_count++];
    entry->event_type = type;
    entry->handler = handler;
    entry->userdata = userdata;
    entry->capture = true;
}

/* Unregister event handler */
void tui_widget_off(tui_widget* w, tui_event_type type, tui_event_handler handler) {
    if (!w) return;
    
    for (int i = 0; i < w->handler_count; i++) {
        if (w->handlers[i].event_type == type && w->handlers[i].handler == handler) {
            /* Shift remaining */
            for (int j = i; j < w->handler_count - 1; j++) {
                w->handlers[j] = w->handlers[j + 1];
            }
            w->handler_count--;
            return;
        }
    }
}

/* Event helpers */
void tui_widget_event_stop(tui_widget_event* e) {
    if (e) e->stopped = true;
}

void tui_widget_event_prevent(tui_widget_event* e) {
    if (e) e->prevented = true;
}

void tui_widget_event_consume(tui_widget_event* e) {
    if (e) {
        e->consumed = true;
        e->stopped = true;
    }
}

/* Initialize widget manager */
void tui_wm_init(tui_widget_manager* wm) {
    if (!wm) return;
    memset(wm, 0, sizeof(tui_widget_manager));
}

/* Set root widget */
void tui_wm_set_root(tui_widget_manager* wm, tui_widget* root) {
    if (wm) wm->root = root;
}

/* Call handlers for a widget during a phase */
static void tui_widget_call_handlers(tui_widget* w, tui_widget_event* event, bool capture) {
    for (int i = 0; i < w->handler_count && !event->stopped; i++) {
        tui_handler_entry* entry = &w->handlers[i];
        if (entry->capture == capture && 
            entry->event_type == event->base.type) {
            event->current = w;
            entry->handler(w, event, entry->userdata);
        }
    }
}

/* Build path from root to target */
static int tui_widget_build_path(tui_widget* target, tui_widget** path, int max_depth) {
    int depth = 0;
    tui_widget* w = target;
    
    /* Count depth */
    while (w && depth < max_depth) {
        path[depth++] = w;
        w = w->parent;
    }
    
    /* Reverse to get root-first order */
    for (int i = 0; i < depth / 2; i++) {
        tui_widget* tmp = path[i];
        path[i] = path[depth - 1 - i];
        path[depth - 1 - i] = tmp;
    }
    
    return depth;
}

/* Hit test (find deepest widget at point) */
static tui_widget* tui_widget_hit_test_recursive(tui_widget* w, int x, int y) {
    if (!w || !w->visible) return NULL;
    
    if (!tui_widget_contains_point(w, x, y)) return NULL;
    
    /* Check children in reverse order (top-most first) */
    for (int i = w->child_count - 1; i >= 0; i--) {
        tui_widget* hit = tui_widget_hit_test_recursive(w->children[i], x, y);
        if (hit) return hit;
    }
    
    return w;
}

tui_widget* tui_wm_hit_test(tui_widget_manager* wm, int x, int y) {
    if (!wm || !wm->root) return NULL;
    return tui_widget_hit_test_recursive(wm->root, x, y);
}

/* Find next focusable widget */
static tui_widget* tui_widget_find_next_focusable(tui_widget* root, tui_widget* current, bool forward) {
    if (!root) return NULL;
    
    /* Collect all focusable widgets */
    tui_widget* focusable[256];
    int count = 0;
    
    /* DFS to collect focusables */
    tui_widget* stack[256];
    int stack_top = 0;
    stack[stack_top++] = root;
    
    while (stack_top > 0 && count < 256) {
        tui_widget* w = stack[--stack_top];
        if (w->focusable && w->visible && w->enabled) {
            focusable[count++] = w;
        }
        /* Add children in reverse for correct order */
        for (int i = w->child_count - 1; i >= 0; i--) {
            if (stack_top < 256) {
                stack[stack_top++] = w->children[i];
            }
        }
    }
    
    if (count == 0) return NULL;
    
    /* Sort by tab_index (simple bubble sort) */
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (focusable[j]->tab_index > focusable[j + 1]->tab_index) {
                tui_widget* tmp = focusable[j];
                focusable[j] = focusable[j + 1];
                focusable[j + 1] = tmp;
            }
        }
    }
    
    /* Find current and move to next/prev */
    int current_idx = -1;
    for (int i = 0; i < count; i++) {
        if (focusable[i] == current) {
            current_idx = i;
            break;
        }
    }
    
    if (current_idx < 0) {
        return focusable[0];
    }
    
    if (forward) {
        return focusable[(current_idx + 1) % count];
    } else {
        return focusable[(current_idx - 1 + count) % count];
    }
}

/* Focus a widget */
void tui_wm_focus(tui_widget_manager* wm, tui_widget* widget) {
    if (!wm) return;
    
    /* Unfocus current */
    if (wm->focus) {
        wm->focus->focused = false;
    }
    
    /* Focus new */
    wm->focus = widget;
    if (widget) {
        widget->focused = true;
    }
}

void tui_wm_focus_next(tui_widget_manager* wm) {
    if (!wm || !wm->root) return;
    
    tui_widget* search_root = wm->root;
    if (wm->focus_stack_top > 0) {
        search_root = wm->focus_stack[wm->focus_stack_top - 1];
    }
    
    tui_widget* next = tui_widget_find_next_focusable(search_root, wm->focus, true);
    tui_wm_focus(wm, next);
}

void tui_wm_focus_prev(tui_widget_manager* wm) {
    if (!wm || !wm->root) return;
    
    tui_widget* search_root = wm->root;
    if (wm->focus_stack_top > 0) {
        search_root = wm->focus_stack[wm->focus_stack_top - 1];
    }
    
    tui_widget* prev = tui_widget_find_next_focusable(search_root, wm->focus, false);
    tui_wm_focus(wm, prev);
}

tui_widget* tui_wm_get_focus(tui_widget_manager* wm) {
    return wm ? wm->focus : NULL;
}

/* Push focus scope (for modals) */
void tui_wm_push_focus(tui_widget_manager* wm, tui_widget* modal_root) {
    if (!wm || wm->focus_stack_top >= TUI_MAX_FOCUS_STACK) return;
    wm->focus_stack[wm->focus_stack_top++] = modal_root;
    
    /* Focus first focusable in modal */
    tui_widget* first = tui_widget_find_next_focusable(modal_root, NULL, true);
    tui_wm_focus(wm, first);
}

/* Pop focus scope */
void tui_wm_pop_focus(tui_widget_manager* wm) {
    if (!wm || wm->focus_stack_top <= 0) return;
    wm->focus_stack_top--;
    
    /* Restore focus to something in parent scope */
    if (wm->focus_stack_top > 0) {
        tui_widget* scope = wm->focus_stack[wm->focus_stack_top - 1];
        tui_widget* first = tui_widget_find_next_focusable(scope, NULL, true);
        tui_wm_focus(wm, first);
    } else if (wm->root) {
        tui_widget* first = tui_widget_find_next_focusable(wm->root, NULL, true);
        tui_wm_focus(wm, first);
    }
}

/* Register hotkey */
void tui_wm_register_hotkey(tui_widget_manager* wm, tui_key key, uint32_t ch,
                            bool ctrl, bool alt, bool shift,
                            tui_event_handler handler, void* userdata) {
    if (!wm || wm->hotkey_count >= TUI_MAX_HOTKEYS) return;
    
    tui_hotkey* hk = &wm->hotkeys[wm->hotkey_count++];
    hk->key = key;
    hk->ch = ch;
    hk->ctrl = ctrl;
    hk->alt = alt;
    hk->shift = shift;
    hk->handler = handler;
    hk->userdata = userdata;
    hk->active = true;
}

/* Unregister hotkey */
void tui_wm_unregister_hotkey(tui_widget_manager* wm, tui_key key, uint32_t ch,
                              bool ctrl, bool alt, bool shift) {
    if (!wm) return;
    
    for (int i = 0; i < wm->hotkey_count; i++) {
        tui_hotkey* hk = &wm->hotkeys[i];
        if (hk->key == key && hk->ch == ch &&
            hk->ctrl == ctrl && hk->alt == alt && hk->shift == shift) {
            /* Shift remaining */
            for (int j = i; j < wm->hotkey_count - 1; j++) {
                wm->hotkeys[j] = wm->hotkeys[j + 1];
            }
            wm->hotkey_count--;
            return;
        }
    }
}

/* Check and dispatch hotkeys */
static bool tui_wm_check_hotkeys(tui_widget_manager* wm, tui_widget_event* event) {
    if (event->base.type != TUI_EVENT_KEY) return false;
    
    for (int i = 0; i < wm->hotkey_count; i++) {
        tui_hotkey* hk = &wm->hotkeys[i];
        if (!hk->active) continue;
        
        bool match = false;
        if (hk->key == TUI_KEY_CHAR) {
            match = (event->base.key == TUI_KEY_CHAR && event->base.ch == hk->ch);
        } else {
            match = (event->base.key == hk->key);
        }
        
        /* TODO: Check modifiers when we have them */
        
        if (match && hk->handler) {
            hk->handler(NULL, event, hk->userdata);
            if (event->consumed) return true;
        }
    }
    
    return false;
}

/* ============================================================================
 * Default Widget Input Handling
 * ============================================================================ */

/* Handle textbox input */
static bool tui_widget_handle_textbox_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e || e->base.type != TUI_EVENT_KEY) return false;
    if (!w->state.textbox.buffer) return false;
    
    char* buf = w->state.textbox.buffer;
    int cap = w->state.textbox.capacity;
    int* len = &w->state.textbox.length;
    int* cursor = &w->state.textbox.cursor;
    
    tui_key key = e->base.key;
    uint32_t ch = e->base.ch;
    
    switch (key) {
        case TUI_KEY_LEFT:
            if (*cursor > 0) (*cursor)--;
            return true;
            
        case TUI_KEY_RIGHT:
            if (*cursor < *len) (*cursor)++;
            return true;
            
        case TUI_KEY_HOME:
            *cursor = 0;
            return true;
            
        case TUI_KEY_END:
            *cursor = *len;
            return true;
            
        case TUI_KEY_BACKSPACE:
            if (*cursor > 0) {
                memmove(buf + *cursor - 1, buf + *cursor, (size_t)(*len - *cursor + 1));
                (*cursor)--;
                (*len)--;
            }
            return true;
            
        case TUI_KEY_DELETE:
            if (*cursor < *len) {
                memmove(buf + *cursor, buf + *cursor + 1, (size_t)(*len - *cursor));
                (*len)--;
            }
            return true;
            
        case TUI_KEY_CHAR:
            if (ch >= 32 && ch < 127 && *len < cap - 1) {
                memmove(buf + *cursor + 1, buf + *cursor, (size_t)(*len - *cursor + 1));
                buf[*cursor] = (char)ch;
                (*cursor)++;
                (*len)++;
            }
            return true;
            
        case TUI_KEY_SPACE:
            /* Space character for text input */
            if (*len < cap - 1) {
                memmove(buf + *cursor + 1, buf + *cursor, (size_t)(*len - *cursor + 1));
                buf[*cursor] = ' ';
                (*cursor)++;
                (*len)++;
            }
            return true;
            
        default:
            break;
    }
    return false;
}

/* Handle checkbox input */
static bool tui_widget_handle_checkbox_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    bool toggle = false;
    if (e->base.type == TUI_EVENT_KEY) {
        if (e->base.key == TUI_KEY_ENTER || e->base.key == TUI_KEY_SPACE) {
            toggle = true;
        }
    } else if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            toggle = true;
        }
    }
    
    if (toggle) {
        w->state.checkbox.checked = !w->state.checkbox.checked;
        return true;
    }
    return false;
}

/* Handle radio button input */
static bool tui_widget_handle_radio_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e || !w->state.radio.group_value) return false;
    
    bool select = false;
    if (e->base.type == TUI_EVENT_KEY) {
        if (e->base.key == TUI_KEY_ENTER || e->base.key == TUI_KEY_SPACE) {
            select = true;
        }
    } else if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            select = true;
        }
    }
    
    if (select) {
        *w->state.radio.group_value = w->state.radio.value;
        return true;
    }
    return false;
}

/* Handle list input */
static bool tui_widget_handle_list_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    int* sel = &w->state.list.selected;
    int* scr = &w->state.list.scroll;
    int count = w->state.list.count;
    int visible = w->state.list.visible;
    
    if (e->base.type == TUI_EVENT_KEY) {
        switch (e->base.key) {
            case TUI_KEY_UP:
                if (*sel > 0) {
                    (*sel)--;
                    if (*sel < *scr) *scr = *sel;
                }
                return true;
                
            case TUI_KEY_DOWN:
                if (*sel < count - 1) {
                    (*sel)++;
                    if (*sel >= *scr + visible) *scr = *sel - visible + 1;
                }
                return true;
                
            case TUI_KEY_PAGEUP:
                *sel -= visible;
                if (*sel < 0) *sel = 0;
                if (*sel < *scr) *scr = *sel;
                return true;
                
            case TUI_KEY_PAGEDOWN:
                *sel += visible;
                if (*sel >= count) *sel = count - 1;
                if (*sel >= *scr + visible) *scr = *sel - visible + 1;
                return true;
                
            case TUI_KEY_HOME:
                *sel = 0;
                *scr = 0;
                return true;
                
            case TUI_KEY_END:
                *sel = count - 1;
                if (*sel >= visible) *scr = *sel - visible + 1;
                return true;
                
            default:
                break;
        }
    } else if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            int ax, ay, aw, ah;
            tui_widget_get_absolute_bounds(w, &ax, &ay, &aw, &ah);
            int clicked_row = e->base.mouse_y - ay;
            int clicked_item = *scr + clicked_row;
            if (clicked_item >= 0 && clicked_item < count) {
                *sel = clicked_item;
                return true;
            }
        } else if (e->base.mouse_button == TUI_MOUSE_WHEEL_UP) {
            if (*scr > 0) (*scr)--;
            return true;
        } else if (e->base.mouse_button == TUI_MOUSE_WHEEL_DOWN) {
            int max_scroll = count - visible;
            if (max_scroll < 0) max_scroll = 0;
            if (*scr < max_scroll) (*scr)++;
            return true;
        }
    }
    return false;
}

/* Handle slider input */
static bool tui_widget_handle_slider_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    float* val = &w->state.slider.value;
    float min = w->state.slider.min;
    float max = w->state.slider.max;
    float step = w->state.slider.step;
    if (step <= 0) step = (max - min) / 20.0f;
    
    if (e->base.type == TUI_EVENT_KEY) {
        switch (e->base.key) {
            case TUI_KEY_LEFT:
            case TUI_KEY_DOWN:
                *val -= step;
                if (*val < min) *val = min;
                return true;
                
            case TUI_KEY_RIGHT:
            case TUI_KEY_UP:
                *val += step;
                if (*val > max) *val = max;
                return true;
                
            case TUI_KEY_HOME:
                *val = min;
                return true;
                
            case TUI_KEY_END:
                *val = max;
                return true;
                
            default:
                break;
        }
    } else if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            int ax, ay, aw, ah;
            tui_widget_get_absolute_bounds(w, &ax, &ay, &aw, &ah);
            float ratio = (float)(e->base.mouse_x - ax) / (float)(aw - 1);
            if (ratio < 0) ratio = 0;
            if (ratio > 1) ratio = 1;
            *val = min + ratio * (max - min);
            w->state.slider.dragging = true;
            return true;
        }
    }
    return false;
}

/* Handle spinner input */
static bool tui_widget_handle_spinner_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    int* val = &w->state.spinner.value;
    int min = w->state.spinner.min;
    int max = w->state.spinner.max;
    int step = w->state.spinner.step;
    if (step <= 0) step = 1;
    
    if (e->base.type == TUI_EVENT_KEY) {
        switch (e->base.key) {
            case TUI_KEY_LEFT:
            case TUI_KEY_DOWN:
                *val -= step;
                if (*val < min) *val = min;
                return true;
                
            case TUI_KEY_RIGHT:
            case TUI_KEY_UP:
                *val += step;
                if (*val > max) *val = max;
                return true;
                
            default:
                break;
        }
    } else if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            int ax, ay, aw, ah;
            tui_widget_get_absolute_bounds(w, &ax, &ay, &aw, &ah);
            /* Left third = decrement, right third = increment */
            int rel_x = e->base.mouse_x - ax;
            if (rel_x < aw / 3) {
                *val -= step;
                if (*val < min) *val = min;
            } else if (rel_x > 2 * aw / 3) {
                *val += step;
                if (*val > max) *val = max;
            }
            return true;
        }
    }
    return false;
}

/* Handle dropdown input */
static bool tui_widget_handle_dropdown_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    bool* open = &w->state.dropdown.open;
    int* sel = &w->state.dropdown.selected;
    int count = w->state.dropdown.count;
    
    if (e->base.type == TUI_EVENT_KEY) {
        if (*open) {
            switch (e->base.key) {
                case TUI_KEY_UP:
                    if (*sel > 0) (*sel)--;
                    return true;
                case TUI_KEY_DOWN:
                    if (*sel < count - 1) (*sel)++;
                    return true;
                case TUI_KEY_ENTER:
                case TUI_KEY_SPACE:
                    *open = false;
                    return true;
                case TUI_KEY_ESC:
                    *open = false;
                    return true;
                default:
                    break;
            }
        } else {
            if (e->base.key == TUI_KEY_ENTER || e->base.key == TUI_KEY_SPACE) {
                *open = true;
                return true;
            }
        }
    } else if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            if (*open) {
                int ax, ay, aw, ah;
                tui_widget_get_absolute_bounds(w, &ax, &ay, &aw, &ah);
                int clicked_row = e->base.mouse_y - ay - 1;
                if (clicked_row >= 0 && clicked_row < count && clicked_row < 5) {
                    *sel = w->state.dropdown.scroll + clicked_row;
                    *open = false;
                } else {
                    *open = false;
                }
            } else {
                *open = true;
            }
            return true;
        }
    }
    return false;
}

/* Handle tabs input */
static bool tui_widget_handle_tabs_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    int* sel = &w->state.tabs.selected;
    int count = w->state.tabs.count;
    
    if (e->base.type == TUI_EVENT_KEY) {
        switch (e->base.key) {
            case TUI_KEY_LEFT:
                if (*sel > 0) (*sel)--;
                return true;
            case TUI_KEY_RIGHT:
                if (*sel < count - 1) (*sel)++;
                return true;
            default:
                break;
        }
    } else if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            /* Calculate which tab was clicked */
            int ax, ay, aw, ah;
            tui_widget_get_absolute_bounds(w, &ax, &ay, &aw, &ah);
            int rel_x = e->base.mouse_x - ax;
            int tab_x = 0;
            for (int i = 0; i < count; i++) {
                int tab_width = w->state.tabs.labels[i] ? 
                    (int)strlen(w->state.tabs.labels[i]) + 2 : 4;
                if (rel_x >= tab_x && rel_x < tab_x + tab_width) {
                    *sel = i;
                    return true;
                }
                tab_x += tab_width + 1; /* +1 for separator */
            }
        }
    }
    return false;
}

/* Handle scrollbar input */
static bool tui_widget_handle_scrollbar_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    int* scr = &w->state.scrollbar.scroll;
    int content = w->state.scrollbar.content_size;
    int view = w->state.scrollbar.view_size;
    int max_scroll = content - view;
    if (max_scroll < 0) max_scroll = 0;
    
    if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            int ax, ay, aw, ah;
            tui_widget_get_absolute_bounds(w, &ax, &ay, &aw, &ah);
            
            if (w->state.scrollbar.vertical) {
                float ratio = (float)(e->base.mouse_y - ay) / (float)(ah - 1);
                *scr = (int)(ratio * (float)max_scroll + 0.5f);
            } else {
                float ratio = (float)(e->base.mouse_x - ax) / (float)(aw - 1);
                *scr = (int)(ratio * (float)max_scroll + 0.5f);
            }
            if (*scr < 0) *scr = 0;
            if (*scr > max_scroll) *scr = max_scroll;
            w->state.scrollbar.dragging = true;
            return true;
        } else if (e->base.mouse_button == TUI_MOUSE_WHEEL_UP) {
            *scr -= 3;
            if (*scr < 0) *scr = 0;
            return true;
        } else if (e->base.mouse_button == TUI_MOUSE_WHEEL_DOWN) {
            *scr += 3;
            if (*scr > max_scroll) *scr = max_scroll;
            return true;
        }
    }
    return false;
}

/* Handle textarea input */
static bool tui_widget_handle_textarea_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    if (!w->state.textarea.lines || w->state.textarea.line_count == 0) return false;
    
    int* row = &w->state.textarea.cursor_row;
    int* col = &w->state.textarea.cursor_col;
    int* scroll_row = &w->state.textarea.scroll_row;
    int line_count = w->state.textarea.line_count;
    bool editable = w->state.textarea.editable;
    int max_line_len = w->state.textarea.max_line_len > 0 ? w->state.textarea.max_line_len : 256;
    
    /* Calculate visible rows */
    int visible_rows = w->height - (w->has_border ? 2 : 0);
    int gutter_width = w->state.textarea.line_numbers ? 5 : 0;
    int visible_cols = w->width - (w->has_border ? 2 : 0) - gutter_width;
    (void)visible_cols;
    
    /* Handle mouse events */
    if (e->base.type == TUI_EVENT_MOUSE) {
        int ax, ay, aw, ah;
        tui_widget_get_absolute_bounds(w, &ax, &ay, &aw, &ah);
        
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            /* Click to position cursor */
            int click_row = e->base.mouse_y - ay + *scroll_row;
            int click_col = e->base.mouse_x - ax - gutter_width;
            
            if (click_row >= 0 && click_row < line_count) {
                *row = click_row;
                int line_len = w->state.textarea.lines[*row] ? 
                               (int)strlen(w->state.textarea.lines[*row]) : 0;
                *col = (click_col >= 0) ? click_col : 0;
                if (*col > line_len) *col = line_len;
            }
            return true;
        } else if (e->base.mouse_button == TUI_MOUSE_WHEEL_UP) {
            *scroll_row -= 3;
            if (*scroll_row < 0) *scroll_row = 0;
            return true;
        } else if (e->base.mouse_button == TUI_MOUSE_WHEEL_DOWN) {
            *scroll_row += 3;
            int max_scroll = line_count - visible_rows;
            if (max_scroll < 0) max_scroll = 0;
            if (*scroll_row > max_scroll) *scroll_row = max_scroll;
            return true;
        }
        return false;
    }
    
    /* Only handle key events from here */
    if (e->base.type != TUI_EVENT_KEY) return false;
    
    /* Get current line */
    char* current_line = w->state.textarea.lines[*row];
    int current_line_len = current_line ? (int)strlen(current_line) : 0;
    
    /* Navigation keys */
    switch (e->base.key) {
        case TUI_KEY_UP:
            if (*row > 0) {
                (*row)--;
                int new_line_len = w->state.textarea.lines[*row] ? 
                                   (int)strlen(w->state.textarea.lines[*row]) : 0;
                if (*col > new_line_len) *col = new_line_len;
                if (*row < *scroll_row) *scroll_row = *row;
            }
            return true;
            
        case TUI_KEY_DOWN:
            if (*row < line_count - 1) {
                (*row)++;
                int new_line_len = w->state.textarea.lines[*row] ? 
                                   (int)strlen(w->state.textarea.lines[*row]) : 0;
                if (*col > new_line_len) *col = new_line_len;
                if (*row >= *scroll_row + visible_rows) *scroll_row = *row - visible_rows + 1;
            }
            return true;
            
        case TUI_KEY_LEFT:
            if (*col > 0) {
                (*col)--;
            } else if (*row > 0) {
                (*row)--;
                *col = w->state.textarea.lines[*row] ? 
                       (int)strlen(w->state.textarea.lines[*row]) : 0;
                if (*row < *scroll_row) *scroll_row = *row;
            }
            return true;
            
        case TUI_KEY_RIGHT:
            if (*col < current_line_len) {
                (*col)++;
            } else if (*row < line_count - 1) {
                (*row)++;
                *col = 0;
                if (*row >= *scroll_row + visible_rows) *scroll_row = *row - visible_rows + 1;
            }
            return true;
            
        case TUI_KEY_HOME:
            if (e->base.ctrl) {
                *row = 0;
                *col = 0;
                *scroll_row = 0;
            } else {
                *col = 0;
            }
            return true;
            
        case TUI_KEY_END:
            if (e->base.ctrl) {
                *row = w->state.textarea.line_count - 1;
                *col = w->state.textarea.lines[*row] ? 
                       (int)strlen(w->state.textarea.lines[*row]) : 0;
                if (*row >= *scroll_row + visible_rows) *scroll_row = *row - visible_rows + 1;
            } else {
                *col = current_line_len;
            }
            return true;
            
        case TUI_KEY_PAGEUP:
            *row -= visible_rows;
            if (*row < 0) *row = 0;
            *scroll_row -= visible_rows;
            if (*scroll_row < 0) *scroll_row = 0;
            {
                int new_line_len = w->state.textarea.lines[*row] ? 
                                   (int)strlen(w->state.textarea.lines[*row]) : 0;
                if (*col > new_line_len) *col = new_line_len;
            }
            return true;
            
        case TUI_KEY_PAGEDOWN:
            *row += visible_rows;
            if (*row >= w->state.textarea.line_count) *row = w->state.textarea.line_count - 1;
            *scroll_row += visible_rows;
            {
                int max_scroll = w->state.textarea.line_count - visible_rows;
                if (max_scroll < 0) max_scroll = 0;
                if (*scroll_row > max_scroll) *scroll_row = max_scroll;
                int new_line_len = w->state.textarea.lines[*row] ? 
                                   (int)strlen(w->state.textarea.lines[*row]) : 0;
                if (*col > new_line_len) *col = new_line_len;
            }
            return true;
        
        default:
            break;
    }
    
    /* Editing keys (only if editable) */
    if (!editable) return false;
    
    switch (e->base.key) {
        case TUI_KEY_BACKSPACE:
            if (*col > 0 && current_line) {
                /* Delete char before cursor */
                memmove(current_line + *col - 1, current_line + *col, 
                        strlen(current_line + *col) + 1);
                (*col)--;
            } else if (*col == 0 && *row > 0) {
                /* Join with previous line */
                char* prev_line = w->state.textarea.lines[*row - 1];
                int prev_len = prev_line ? (int)strlen(prev_line) : 0;
                int curr_len = current_line ? (int)strlen(current_line) : 0;
                
                if (prev_len + curr_len < max_line_len) {
                    /* Append current line to previous */
                    if (current_line && prev_line) {
                        strcat(prev_line, current_line);
                    }
                    /* Free current line and shift remaining lines up */
                    free(current_line);
                    for (int i = *row; i < w->state.textarea.line_count - 1; i++) {
                        w->state.textarea.lines[i] = w->state.textarea.lines[i + 1];
                    }
                    w->state.textarea.line_count--;
                    (*row)--;
                    *col = prev_len;
                    if (*row < *scroll_row) *scroll_row = *row;
                }
            }
            return true;
            
        case TUI_KEY_DELETE:
            if (current_line && *col < current_line_len) {
                /* Delete char at cursor */
                memmove(current_line + *col, current_line + *col + 1, 
                        strlen(current_line + *col + 1) + 1);
            } else if (*row < w->state.textarea.line_count - 1) {
                /* Join with next line */
                char* next_line = w->state.textarea.lines[*row + 1];
                int curr_len = current_line ? (int)strlen(current_line) : 0;
                int next_len = next_line ? (int)strlen(next_line) : 0;
                
                if (curr_len + next_len < max_line_len && current_line) {
                    strcat(current_line, next_line ? next_line : "");
                    free(next_line);
                    for (int i = *row + 1; i < w->state.textarea.line_count - 1; i++) {
                        w->state.textarea.lines[i] = w->state.textarea.lines[i + 1];
                    }
                    w->state.textarea.line_count--;
                }
            }
            return true;
            
        case TUI_KEY_ENTER:
            /* Split line at cursor */
            if (w->state.textarea.line_count < w->state.textarea.line_capacity) {
                /* Make room for new line */
                for (int i = w->state.textarea.line_count; i > *row + 1; i--) {
                    w->state.textarea.lines[i] = w->state.textarea.lines[i - 1];
                }
                
                /* Create new line with content after cursor */
                const char* after_cursor = current_line ? current_line + *col : "";
                char* new_line = (char*)malloc(max_line_len);
                if (new_line) {
                    strncpy(new_line, after_cursor, max_line_len - 1);
                    new_line[max_line_len - 1] = '\0';
                    w->state.textarea.lines[*row + 1] = new_line;
                    
                    /* Truncate current line at cursor */
                    if (current_line) current_line[*col] = '\0';
                    
                    w->state.textarea.line_count++;
                    (*row)++;
                    *col = 0;
                    if (*row >= *scroll_row + visible_rows) *scroll_row = *row - visible_rows + 1;
                }
            }
            return true;
            
        case TUI_KEY_TAB:
            /* Insert spaces for tab */
            if (current_line && current_line_len + 4 < max_line_len) {
                memmove(current_line + *col + 4, current_line + *col, 
                        strlen(current_line + *col) + 1);
                memset(current_line + *col, ' ', 4);
                *col += 4;
            }
            return true;
            
        case TUI_KEY_SPACE:
            /* Insert space */
            if (current_line && current_line_len < max_line_len - 1) {
                memmove(current_line + *col + 1, current_line + *col, 
                        strlen(current_line + *col) + 1);
                current_line[*col] = ' ';
                (*col)++;
            }
            return true;
            
        case TUI_KEY_CHAR:
            /* Insert character */
            if (current_line && current_line_len < max_line_len - 1 && e->base.ch >= 32) {
                memmove(current_line + *col + 1, current_line + *col, 
                        strlen(current_line + *col) + 1);
                current_line[*col] = (char)e->base.ch;
                (*col)++;
            }
            return true;
            
        default:
            break;
    }
    
    return false;
}

/* Handle splitter input */
static bool tui_widget_handle_splitter_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    if (e->base.type == TUI_EVENT_MOUSE) {
        int ax, ay, aw, ah;
        tui_widget_get_absolute_bounds(w, &ax, &ay, &aw, &ah);
        
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            w->state.splitter.dragging = true;
            
            /* Calculate new ratio */
            if (w->state.splitter.vertical) {
                float new_ratio = (float)(e->base.mouse_y - ay) / (float)ah;
                if (new_ratio < 0.1f) new_ratio = 0.1f;
                if (new_ratio > 0.9f) new_ratio = 0.9f;
                w->state.splitter.ratio = new_ratio;
            } else {
                float new_ratio = (float)(e->base.mouse_x - ax) / (float)aw;
                if (new_ratio < 0.1f) new_ratio = 0.1f;
                if (new_ratio > 0.9f) new_ratio = 0.9f;
                w->state.splitter.ratio = new_ratio;
            }
            return true;
        } else if (e->base.mouse_button == TUI_MOUSE_MOVE && w->state.splitter.dragging) {
            if (w->state.splitter.vertical) {
                float new_ratio = (float)(e->base.mouse_y - ay) / (float)ah;
                if (new_ratio < 0.1f) new_ratio = 0.1f;
                if (new_ratio > 0.9f) new_ratio = 0.9f;
                w->state.splitter.ratio = new_ratio;
            } else {
                float new_ratio = (float)(e->base.mouse_x - ax) / (float)aw;
                if (new_ratio < 0.1f) new_ratio = 0.1f;
                if (new_ratio > 0.9f) new_ratio = 0.9f;
                w->state.splitter.ratio = new_ratio;
            }
            return true;
        } else if (e->base.mouse_button == TUI_MOUSE_RELEASE) {
            w->state.splitter.dragging = false;
            return true;
        }
    } else if (e->base.type == TUI_EVENT_KEY) {
        /* Allow keyboard adjustment with Ctrl+arrow */
        if (e->base.ctrl) {
            float step = 0.05f;
            if (w->state.splitter.vertical) {
                if (e->base.key == TUI_KEY_UP) {
                    w->state.splitter.ratio -= step;
                    if (w->state.splitter.ratio < 0.1f) w->state.splitter.ratio = 0.1f;
                    return true;
                } else if (e->base.key == TUI_KEY_DOWN) {
                    w->state.splitter.ratio += step;
                    if (w->state.splitter.ratio > 0.9f) w->state.splitter.ratio = 0.9f;
                    return true;
                }
            } else {
                if (e->base.key == TUI_KEY_LEFT) {
                    w->state.splitter.ratio -= step;
                    if (w->state.splitter.ratio < 0.1f) w->state.splitter.ratio = 0.1f;
                    return true;
                } else if (e->base.key == TUI_KEY_RIGHT) {
                    w->state.splitter.ratio += step;
                    if (w->state.splitter.ratio > 0.9f) w->state.splitter.ratio = 0.9f;
                    return true;
                }
            }
        }
    }
    
    return false;
}

/* Handle button input */
static bool tui_widget_handle_button_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e) return false;
    
    bool activate = false;
    if (e->base.type == TUI_EVENT_KEY) {
        if (e->base.key == TUI_KEY_ENTER || e->base.key == TUI_KEY_SPACE) {
            activate = true;
        }
    } else if (e->base.type == TUI_EVENT_MOUSE) {
        if (e->base.mouse_button == TUI_MOUSE_LEFT) {
            activate = true;
        }
    }
    
    if (activate) {
        w->state.button.pressed = true;
        return true;  /* Let user handlers see this event */
    }
    return false;
}

/* Default input handler for widget types */
static bool tui_widget_handle_default_input(tui_widget* w, tui_widget_event* e) {
    if (!w || !e || !w->enabled) return false;
    if (e->phase != TUI_PHASE_TARGET) return false;
    
    switch (w->type) {
        case TUI_WIDGET_BUTTON:
            return tui_widget_handle_button_input(w, e);
        case TUI_WIDGET_TEXTBOX:
            return tui_widget_handle_textbox_input(w, e);
        case TUI_WIDGET_CHECKBOX:
            return tui_widget_handle_checkbox_input(w, e);
        case TUI_WIDGET_RADIO:
            return tui_widget_handle_radio_input(w, e);
        case TUI_WIDGET_LIST:
            return tui_widget_handle_list_input(w, e);
        case TUI_WIDGET_SLIDER:
            return tui_widget_handle_slider_input(w, e);
        case TUI_WIDGET_SPINNER:
            return tui_widget_handle_spinner_input(w, e);
        case TUI_WIDGET_DROPDOWN:
            return tui_widget_handle_dropdown_input(w, e);
        case TUI_WIDGET_TABS:
            return tui_widget_handle_tabs_input(w, e);
        case TUI_WIDGET_SCROLLBAR:
            return tui_widget_handle_scrollbar_input(w, e);
        case TUI_WIDGET_TEXTAREA:
            return tui_widget_handle_textarea_input(w, e);
        case TUI_WIDGET_SPLITTER:
            return tui_widget_handle_splitter_input(w, e);
        default:
            break;
    }
    return false;
}

/* Route event through widget tree */
void tui_wm_route_event(tui_widget_manager* wm, tui_event* event) {
    if (!wm || !event) return;
    
    tui_widget_event we = {0};
    we.base = *event;
    we.stopped = false;
    we.prevented = false;
    we.consumed = false;
    
    /* Handle Tab for focus navigation */
    if (event->type == TUI_EVENT_KEY && event->key == TUI_KEY_TAB) {
        /* TODO: Check shift modifier for reverse */
        tui_wm_focus_next(wm);
        return;
    }
    
    /* Check hotkeys first */
    if (tui_wm_check_hotkeys(wm, &we)) {
        return;
    }
    
    /* Determine target */
    tui_widget* target = NULL;
    
    if (event->type == TUI_EVENT_MOUSE) {
        target = tui_wm_hit_test(wm, event->mouse_x, event->mouse_y);
        wm->hover = target;
        
        /* Click to focus */
        if (event->mouse_button == TUI_MOUSE_LEFT && target && target->focusable) {
            tui_wm_focus(wm, target);
        }
    } else {
        /* Keyboard events go to focused widget */
        target = wm->focus;
    }
    
    if (!target) {
        target = wm->root;
    }
    
    if (!target) return;
    
    we.target = target;
    
    /* Build path from root to target */
    tui_widget* path[64];
    int path_len = tui_widget_build_path(target, path, 64);
    
    /* CAPTURE PHASE: root → target */
    we.phase = TUI_PHASE_CAPTURE;
    for (int i = 0; i < path_len - 1 && !we.stopped; i++) {
        tui_widget_call_handlers(path[i], &we, true);
    }
    
    /* TARGET PHASE */
    if (!we.stopped) {
        we.phase = TUI_PHASE_TARGET;
        we.current = target;
        tui_widget_call_handlers(target, &we, true);  /* Capture handlers at target */
        
        /* Default widget input handling (before user bubble handlers) */
        if (!we.stopped && !we.prevented) {
            if (tui_widget_handle_default_input(target, &we)) {
                we.consumed = true;
            }
        }
        
        tui_widget_call_handlers(target, &we, false); /* Bubble handlers at target */
    }
    
    /* BUBBLE PHASE: target → root */
    we.phase = TUI_PHASE_BUBBLE;
    for (int i = path_len - 2; i >= 0 && !we.stopped; i--) {
        tui_widget_call_handlers(path[i], &we, false);
    }
}

/* Draw widget recursively */
static void tui_widget_draw_recursive(tui_widget* w, tui_context* ctx) {
    if (!w || !w->visible) return;
    
    int x, y, width, height;
    tui_widget_get_absolute_bounds(w, &x, &y, &width, &height);
    
    /* Set colors */
    uint32_t fg = w->fg_color != TUI_COLOR_DEFAULT ? w->fg_color : TUI_COLOR_WHITE;
    uint32_t bg = w->bg_color != TUI_COLOR_DEFAULT ? w->bg_color : TUI_COLOR_DEFAULT;
    
    /* Draw based on type */
    switch (w->type) {
        case TUI_WIDGET_PANEL:
            if (w->has_border) {
                tui_set_fg(ctx, fg);
                tui_set_bg(ctx, bg);
                tui_box(ctx, x, y, width, height, w->border_style);
            } else if (bg != TUI_COLOR_DEFAULT) {
                tui_set_bg(ctx, bg);
                tui_fill(ctx, x, y, width, height, ' ');
            }
            break;
            
        case TUI_WIDGET_LABEL:
            tui_set_fg(ctx, fg);
            tui_set_bg(ctx, bg);
            if (w->state.label.text) {
                tui_label_aligned(ctx, x, y, width, w->state.label.text, w->state.label.align);
            }
            break;
            
        case TUI_WIDGET_BUTTON: {
            bool focused = w->focused;
            bool pressed = w->state.button.pressed;
            if (pressed) {
                tui_set_fg(ctx, TUI_COLOR_BLACK);
                tui_set_bg(ctx, TUI_COLOR_CYAN);
            } else if (focused) {
                tui_set_fg(ctx, TUI_COLOR_BLACK);
                tui_set_bg(ctx, TUI_COLOR_WHITE);
            } else {
                tui_set_fg(ctx, fg);
                tui_set_bg(ctx, bg != TUI_COLOR_DEFAULT ? bg : TUI_RGB(60, 60, 60));
            }
            
            /* Draw button with brackets */
            char buf[256];
            snprintf(buf, sizeof(buf), "[ %s ]", w->state.button.text ? w->state.button.text : "");
            tui_label(ctx, x, y, buf);
            w->state.button.pressed = false;  /* Reset press state */
            break;
        }
        
        case TUI_WIDGET_CHECKBOX: {
            bool focused = w->focused;
            bool checked = w->state.checkbox.checked;
            tui_set_fg(ctx, focused ? TUI_COLOR_CYAN : fg);
            tui_set_bg(ctx, bg);
            
            char buf[256];
            snprintf(buf, sizeof(buf), "[%c] %s", checked ? 'x' : ' ', 
                     w->state.checkbox.text ? w->state.checkbox.text : "");
            tui_label(ctx, x, y, buf);
            break;
        }
        
        case TUI_WIDGET_RADIO: {
            bool focused = w->focused;
            bool selected = w->state.radio.group_value && 
                           (*w->state.radio.group_value == w->state.radio.value);
            tui_set_fg(ctx, focused ? TUI_COLOR_CYAN : fg);
            tui_set_bg(ctx, bg);
            
            char buf[256];
            snprintf(buf, sizeof(buf), "(%c) %s", selected ? '*' : ' ', 
                     w->state.radio.text ? w->state.radio.text : "");
            tui_label(ctx, x, y, buf);
            break;
        }
        
        case TUI_WIDGET_TEXTBOX: {
            bool focused = w->focused;
            tui_set_fg(ctx, focused ? TUI_COLOR_WHITE : fg);
            tui_set_bg(ctx, focused ? TUI_RGB(40, 40, 60) : TUI_RGB(30, 30, 30));
            tui_fill(ctx, x, y, width, 1, ' ');
            
            if (w->state.textbox.buffer) {
                int cursor = w->state.textbox.cursor;
                int scroll = w->state.textbox.scroll;
                
                /* Adjust scroll to keep cursor visible */
                if (cursor < scroll) scroll = cursor;
                if (cursor >= scroll + width) scroll = cursor - width + 1;
                w->state.textbox.scroll = scroll;
                
                /* Draw visible portion */
                const char* buf = w->state.textbox.buffer;
                int len = w->state.textbox.length;
                for (int i = 0; i < width && scroll + i < len; i++) {
                    tui_set_cell(ctx, x + i, y, (uint32_t)(uint8_t)buf[scroll + i]);
                }
                
                /* Draw cursor */
                if (focused) {
                    int cursor_x = x + (cursor - scroll);
                    if (cursor_x >= x && cursor_x < x + width) {
                        tui_set_bg(ctx, TUI_COLOR_WHITE);
                        tui_set_fg(ctx, TUI_COLOR_BLACK);
                        uint32_t ch = (cursor < len) ? (uint32_t)(uint8_t)buf[cursor] : ' ';
                        tui_set_cell(ctx, cursor_x, y, ch);
                    }
                }
            }
            break;
        }
        
        case TUI_WIDGET_LIST: {
            bool focused = w->focused;
            int sel = w->state.list.selected;
            int scr = w->state.list.scroll;
            int count = w->state.list.count;
            int visible = w->state.list.visible > 0 ? w->state.list.visible : height;
            const char** items = w->state.list.items;
            
            for (int i = 0; i < visible && scr + i < count; i++) {
                bool is_sel = (scr + i == sel);
                if (is_sel) {
                    tui_set_fg(ctx, focused ? TUI_COLOR_BLACK : TUI_COLOR_WHITE);
                    tui_set_bg(ctx, focused ? TUI_COLOR_CYAN : TUI_RGB(80, 80, 80));
                } else {
                    tui_set_fg(ctx, fg);
                    tui_set_bg(ctx, bg);
                }
                tui_fill(ctx, x, y + i, width, 1, ' ');
                if (items && items[scr + i]) {
                    tui_label(ctx, x + 1, y + i, items[scr + i]);
                }
            }
            break;
        }
        
        case TUI_WIDGET_PROGRESS: {
            float val = w->state.progress.value;
            float min = w->state.progress.min;
            float max = w->state.progress.max;
            float ratio = (max > min) ? (val - min) / (max - min) : 0;
            if (ratio < 0) ratio = 0;
            if (ratio > 1) ratio = 1;
            
            int filled = (int)(ratio * (float)(width - 2) + 0.5f);
            
            tui_set_fg(ctx, fg);
            tui_set_bg(ctx, bg);
            tui_set_cell(ctx, x, y, '[');
            tui_set_cell(ctx, x + width - 1, y, ']');
            
            for (int i = 0; i < width - 2; i++) {
                if (i < filled) {
                    tui_set_fg(ctx, TUI_COLOR_GREEN);
                    tui_set_cell(ctx, x + 1 + i, y, 0x2588); /* Full block */
                } else {
                    tui_set_fg(ctx, TUI_RGB(60, 60, 60));
                    tui_set_cell(ctx, x + 1 + i, y, 0x2591); /* Light shade */
                }
            }
            break;
        }
        
        case TUI_WIDGET_SLIDER: {
            bool focused = w->focused;
            float val = w->state.slider.value;
            float min = w->state.slider.min;
            float max = w->state.slider.max;
            float ratio = (max > min) ? (val - min) / (max - min) : 0;
            if (ratio < 0) ratio = 0;
            if (ratio > 1) ratio = 1;
            
            int pos = (int)(ratio * (float)(width - 1) + 0.5f);
            
            for (int i = 0; i < width; i++) {
                if (i == pos) {
                    tui_set_fg(ctx, focused ? TUI_COLOR_CYAN : TUI_COLOR_WHITE);
                    tui_set_cell(ctx, x + i, y, 0x25CF); /* Filled circle */
                } else {
                    tui_set_fg(ctx, TUI_RGB(60, 60, 60));
                    tui_set_cell(ctx, x + i, y, 0x2500); /* Horizontal line */
                }
            }
            break;
        }
        
        case TUI_WIDGET_SPINNER: {
            bool focused = w->focused;
            int val = w->state.spinner.value;
            
            /* [-] value [+] */
            tui_set_fg(ctx, focused ? TUI_COLOR_WHITE : TUI_RGB(150, 150, 150));
            tui_set_bg(ctx, TUI_RGB(50, 50, 50));
            tui_set_cell(ctx, x, y, '[');
            tui_set_cell(ctx, x + 1, y, '-');
            tui_set_cell(ctx, x + 2, y, ']');
            
            tui_set_fg(ctx, focused ? TUI_COLOR_BLACK : fg);
            tui_set_bg(ctx, focused ? TUI_COLOR_CYAN : TUI_RGB(30, 30, 30));
            int val_width = width - 6;
            for (int i = 0; i < val_width; i++) {
                tui_set_cell(ctx, x + 3 + i, y, ' ');
            }
            char vbuf[16];
            snprintf(vbuf, sizeof(vbuf), "%d", val);
            int vlen = (int)strlen(vbuf);
            int vx = x + 3 + (val_width - vlen) / 2;
            tui_label(ctx, vx, y, vbuf);
            
            tui_set_fg(ctx, focused ? TUI_COLOR_WHITE : TUI_RGB(150, 150, 150));
            tui_set_bg(ctx, TUI_RGB(50, 50, 50));
            tui_set_cell(ctx, x + width - 3, y, '[');
            tui_set_cell(ctx, x + width - 2, y, '+');
            tui_set_cell(ctx, x + width - 1, y, ']');
            break;
        }
        
        case TUI_WIDGET_DROPDOWN: {
            bool focused = w->focused;
            bool open = w->state.dropdown.open;
            int sel = w->state.dropdown.selected;
            const char** items = w->state.dropdown.items;
            int count = w->state.dropdown.count;
            
            /* Main button */
            tui_set_fg(ctx, focused ? TUI_COLOR_BLACK : fg);
            tui_set_bg(ctx, focused ? TUI_COLOR_CYAN : TUI_RGB(50, 50, 50));
            tui_fill(ctx, x, y, width, 1, ' ');
            
            if (items && sel >= 0 && sel < count && items[sel]) {
                tui_label(ctx, x + 1, y, items[sel]);
            }
            tui_set_cell(ctx, x + width - 2, y, 0x25BC); /* Down arrow */
            
            /* Dropdown list */
            if (open) {
                int list_height = count < 5 ? count : 5;
                tui_set_bg(ctx, TUI_RGB(40, 40, 40));
                for (int i = 0; i < list_height; i++) {
                    int item_idx = w->state.dropdown.scroll + i;
                    if (item_idx >= count) break;
                    
                    bool is_sel = (item_idx == sel);
                    if (is_sel) {
                        tui_set_fg(ctx, TUI_COLOR_BLACK);
                        tui_set_bg(ctx, TUI_COLOR_WHITE);
                    } else {
                        tui_set_fg(ctx, TUI_COLOR_WHITE);
                        tui_set_bg(ctx, TUI_RGB(40, 40, 40));
                    }
                    tui_fill(ctx, x, y + 1 + i, width, 1, ' ');
                    if (items && items[item_idx]) {
                        tui_label(ctx, x + 1, y + 1 + i, items[item_idx]);
                    }
                }
            }
            break;
        }
        
        case TUI_WIDGET_TABS: {
            bool focused = w->focused;
            int sel = w->state.tabs.selected;
            const char** labels = w->state.tabs.labels;
            int count = w->state.tabs.count;
            
            int cur_x = x;
            for (int i = 0; i < count && cur_x < x + width; i++) {
                bool is_sel = (i == sel);
                if (is_sel) {
                    tui_set_fg(ctx, focused ? TUI_COLOR_BLACK : TUI_COLOR_WHITE);
                    tui_set_bg(ctx, focused ? TUI_COLOR_CYAN : TUI_RGB(80, 80, 80));
                } else {
                    tui_set_fg(ctx, TUI_RGB(150, 150, 150));
                    tui_set_bg(ctx, TUI_RGB(40, 40, 40));
                }
                
                tui_set_cell(ctx, cur_x, y, ' ');
                cur_x++;
                
                const char* label = labels && labels[i] ? labels[i] : "";
                while (*label && cur_x < x + width - 1) {
                    tui_set_cell(ctx, cur_x, y, (uint32_t)(uint8_t)*label);
                    label++;
                    cur_x++;
                }
                
                tui_set_cell(ctx, cur_x, y, ' ');
                cur_x++;
                
                /* Separator */
                if (cur_x < x + width && i < count - 1) {
                    tui_set_fg(ctx, TUI_RGB(60, 60, 60));
                    tui_set_bg(ctx, TUI_COLOR_DEFAULT);
                    tui_set_cell(ctx, cur_x, y, '|');
                    cur_x++;
                }
            }
            
            /* Fill rest */
            tui_set_bg(ctx, TUI_COLOR_DEFAULT);
            while (cur_x < x + width) {
                tui_set_cell(ctx, cur_x, y, ' ');
                cur_x++;
            }
            break;
        }
        
        case TUI_WIDGET_SCROLLBAR: {
            int scr = w->state.scrollbar.scroll;
            int content = w->state.scrollbar.content_size;
            int view = w->state.scrollbar.view_size;
            bool vertical = w->state.scrollbar.vertical;
            
            int bar_len = vertical ? height : width;
            
            /* Draw track */
            tui_set_fg(ctx, TUI_RGB(60, 60, 60));
            for (int i = 0; i < bar_len; i++) {
                if (vertical) {
                    tui_set_cell(ctx, x, y + i, 0x2502); /* Vertical line */
                } else {
                    tui_set_cell(ctx, x + i, y, 0x2500); /* Horizontal line */
                }
            }
            
            /* Draw thumb */
            if (content > 0 && content > view) {
                int thumb_size = (view * bar_len) / content;
                if (thumb_size < 1) thumb_size = 1;
                if (thumb_size > bar_len) thumb_size = bar_len;
                
                int max_scroll = content - view;
                int thumb_pos = 0;
                if (max_scroll > 0) {
                    thumb_pos = (scr * (bar_len - thumb_size)) / max_scroll;
                }
                
                tui_set_fg(ctx, w->focused ? TUI_COLOR_CYAN : TUI_COLOR_WHITE);
                for (int i = 0; i < thumb_size; i++) {
                    if (vertical) {
                        tui_set_cell(ctx, x, y + thumb_pos + i, 0x2588); /* Full block */
                    } else {
                        tui_set_cell(ctx, x + thumb_pos + i, y, 0x2588);
                    }
                }
            }
            break;
        }
        
        case TUI_WIDGET_TEXTAREA: {
            bool focused = w->focused;
            char** lines = w->state.textarea.lines;
            int line_count = w->state.textarea.line_count;
            int scroll_row = w->state.textarea.scroll_row;
            int scroll_col = w->state.textarea.scroll_col;
            int cursor_row = w->state.textarea.cursor_row;
            int cursor_col = w->state.textarea.cursor_col;
            bool line_numbers = w->state.textarea.line_numbers;
            
            int gutter_width = line_numbers ? 5 : 0;
            int text_x = x + gutter_width;
            int text_width = width - gutter_width;
            
            for (int i = 0; i < height; i++) {
                int line_idx = scroll_row + i;
                
                /* Draw line number gutter */
                if (line_numbers) {
                    if (line_idx < line_count) {
                        tui_set_fg(ctx, TUI_RGB(100, 100, 100));
                        tui_set_bg(ctx, TUI_RGB(30, 30, 30));
                        char num_buf[8];
                        snprintf(num_buf, sizeof(num_buf), "%4d", line_idx + 1);
                        tui_label(ctx, x, y + i, num_buf);
                        tui_set_cell(ctx, x + 4, y + i, 0x2502);
                    } else {
                        tui_set_fg(ctx, TUI_RGB(60, 60, 60));
                        tui_set_bg(ctx, TUI_RGB(30, 30, 30));
                        tui_fill(ctx, x, y + i, gutter_width, 1, ' ');
                    }
                }
                
                /* Draw text content */
                tui_set_fg(ctx, fg);
                tui_set_bg(ctx, bg);
                tui_fill(ctx, text_x, y + i, text_width, 1, ' ');
                
                if (line_idx < line_count && lines && lines[line_idx]) {
                    const char* line = lines[line_idx];
                    int line_len = (int)strlen(line);
                    
                    for (int j = 0; j < text_width && scroll_col + j < line_len; j++) {
                        tui_set_cell(ctx, text_x + j, y + i, (uint32_t)(uint8_t)line[scroll_col + j]);
                    }
                }
                
                /* Draw cursor */
                if (focused && line_idx == cursor_row) {
                    int cursor_screen_x = text_x + cursor_col - scroll_col;
                    if (cursor_screen_x >= text_x && cursor_screen_x < text_x + text_width) {
                        tui_set_bg(ctx, TUI_COLOR_WHITE);
                        tui_set_fg(ctx, TUI_COLOR_BLACK);
                        const char* line = (lines && lines[line_idx]) ? lines[line_idx] : "";
                        int line_len = (int)strlen(line);
                        uint32_t ch = (cursor_col < line_len) ? (uint32_t)(uint8_t)line[cursor_col] : ' ';
                        tui_set_cell(ctx, cursor_screen_x, y + i, ch);
                    }
                }
            }
            break;
        }
        
        case TUI_WIDGET_SPLITTER: {
            bool vertical = w->state.splitter.vertical;
            float ratio = w->state.splitter.ratio;
            int min_size = w->state.splitter.min_size;
            
            /* Calculate split position */
            int split_pos;
            if (vertical) {
                split_pos = (int)(ratio * (float)height);
                if (split_pos < min_size) split_pos = min_size;
                if (split_pos > height - min_size) split_pos = height - min_size;
            } else {
                split_pos = (int)(ratio * (float)width);
                if (split_pos < min_size) split_pos = min_size;
                if (split_pos > width - min_size) split_pos = width - min_size;
            }
            
            /* Draw divider line */
            tui_set_fg(ctx, w->state.splitter.dragging ? TUI_COLOR_CYAN : TUI_RGB(100, 100, 100));
            tui_set_bg(ctx, bg);
            
            if (vertical) {
                for (int i = 0; i < width; i++) {
                    tui_set_cell(ctx, x + i, y + split_pos, 0x2500); /* Horizontal line */
                }
            } else {
                for (int i = 0; i < height; i++) {
                    tui_set_cell(ctx, x + split_pos, y + i, 0x2502); /* Vertical line */
                }
            }
            
            /* Update child positions based on split */
            if (w->child_count >= 2) {
                if (vertical) {
                    w->children[0]->x = 0;
                    w->children[0]->y = 0;
                    w->children[0]->width = width;
                    w->children[0]->height = split_pos;
                    
                    w->children[1]->x = 0;
                    w->children[1]->y = split_pos + 1;
                    w->children[1]->width = width;
                    w->children[1]->height = height - split_pos - 1;
                } else {
                    w->children[0]->x = 0;
                    w->children[0]->y = 0;
                    w->children[0]->width = split_pos;
                    w->children[0]->height = height;
                    
                    w->children[1]->x = split_pos + 1;
                    w->children[1]->y = 0;
                    w->children[1]->width = width - split_pos - 1;
                    w->children[1]->height = height;
                }
            }
            break;
        }
        
        case TUI_WIDGET_CONTAINER:
        case TUI_WIDGET_CUSTOM:
        default:
            break;
    }
    
    /* Custom draw function */
    if (w->draw_fn) {
        w->draw_fn(w, ctx);
    }
    
    /* Draw children */
    for (int i = 0; i < w->child_count; i++) {
        tui_widget_draw_recursive(w->children[i], ctx);
    }
}

/* Draw all widgets */
void tui_wm_draw(tui_widget_manager* wm, tui_context* ctx) {
    if (!wm || !wm->root || !ctx) return;
    tui_widget_draw_recursive(wm->root, ctx);
}

#endif /* TUI_IMPLEMENTATION */
