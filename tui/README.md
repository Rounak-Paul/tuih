# TUI Library

A **single-header**, **zero-dependency**, **cross-platform** Terminal UI library written in **pure C11**.

![C11](https://img.shields.io/badge/C-11-blue)
![License](https://img.shields.io/badge/License-MIT-green)
![Platforms](https://img.shields.io/badge/Platforms-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)

## Features

- **Single Header** - Just include `tui.h` and define `TUI_IMPLEMENTATION`
- **Zero Dependencies** - No external libraries required
- **Cross-Platform** - Linux, macOS, Windows 10+
- **Double Buffered** - Flicker-free rendering with diff-based updates
- **Synchronized Output** - Tear-free rendering on supported terminals
- **Mouse Support** - SGR extended mouse tracking
- **Hierarchical Widget System** - Event bubbling, capture phase, focus management
- **Theming** - 5 built-in themes with 24-bit RGB colors
- **Unicode Support** - UTF-8 encoding, wide characters (CJK), box drawing
- **Cursor Shapes** - Block, beam, underline (blinking/steady)
- **Bracketed Paste** - Detect paste vs typed input
- **Focus Events** - Terminal focus in/out detection
- **Clipboard** - OSC 52 clipboard integration
- **Hyperlinks** - OSC 8 clickable URLs
- **Colored Underlines** - Undercurl with custom colors (for spelling errors)

## Quick Start

```c
#define TUI_IMPLEMENTATION
#include "tui.h"

int main(void) {
    tui_context* ctx = tui_create();
    if (!ctx) return 1;
    
    bool running = true;
    while (running) {
        tui_event event;
        while (tui_poll_event(ctx, &event)) {
            if (event.type == TUI_EVENT_KEY && event.key == TUI_KEY_ESC)
                running = false;
        }
        
        tui_begin_frame(ctx);
        
        tui_set_fg(ctx, TUI_COLOR_GREEN);
        tui_label(ctx, 5, 3, "Hello, TUI!");
        
        tui_end_frame(ctx);
    }
    
    tui_destroy(ctx);
    return 0;
}
```

## Building

### Linux / macOS

```bash
cd tui
chmod +x build.sh
./build.sh
./bin/widgets_demo
```

### Windows (MSVC)

```batch
cd tui
build.bat
bin\widgets_demo.exe
```

### Manual Compilation

```bash
# Linux/macOS
cc -std=c11 -O2 -o widgets_demo examples/widgets_demo.c

# Windows (MSVC)
cl /std:c11 /O2 /Fe:widgets_demo.exe examples\widgets_demo.c
```

## Widget Reference

### Core Drawing

```c
// Labels with styling
tui_set_fg(ctx, TUI_COLOR_CYAN);
tui_set_style(ctx, TUI_STYLE_BOLD);
tui_label(ctx, x, y, "Text");

// Aligned text
tui_label_aligned(ctx, x, y, width, "Center", TUI_ALIGN_CENTER);

// Boxes and borders
tui_box(ctx, x, y, width, height, TUI_BORDER_DOUBLE);
tui_fill(ctx, x, y, width, height, ' ');

// Popup box with title
tui_popup_box(ctx, x, y, w, h, "Title", TUI_BORDER_ROUNDED);

// Text wrapping
int lines = tui_wrap_text(ctx, x, y, width, max_lines, long_text);
```

### Buttons

```c
// Simple button (checks for click in event)
if (tui_button(ctx, x, y, "Click Me", &event)) {
    // Button was clicked!
}
```

### Hierarchical Widget System

The library provides a powerful hierarchical widget system with proper event bubbling,
capture phases, focus management, and hit testing - similar to desktop UI frameworks.

```c
// Create widget manager
tui_widget_manager* wm = tui_wm_create();

// Create a root container
tui_widget* root = tui_widget_create(TUI_WIDGET_PANEL);
tui_widget_set_bounds(root, 0, 0, 80, 24);
tui_wm_set_root(wm, root);

// Add child widgets
tui_widget* button = tui_widget_create(TUI_WIDGET_BUTTON);
tui_widget_set_bounds(button, 5, 10, 15, 1);
button->label = "Click Me";
button->on_click = button_clicked;
tui_widget_add_child(root, button);

tui_widget* textbox = tui_widget_create(TUI_WIDGET_TEXTBOX);
tui_widget_set_bounds(textbox, 5, 12, 20, 1);
textbox->buffer = text_buffer;
textbox->buffer_capacity = sizeof(text_buffer);
tui_widget_add_child(root, textbox);

// Process events with full bubbling
while (tui_poll_event(ctx, &event)) {
    tui_wm_dispatch_event(wm, &event);  // Capture → Target → Bubble
}

// Render all widgets
tui_wm_render(wm, ctx);

// Focus navigation
tui_wm_focus_next(wm);      // Tab navigation
tui_wm_focus_prev(wm);      // Shift+Tab
tui_wm_set_focus(wm, widget);  // Focus specific widget

// Hotkey registration
tui_wm_register_hotkey(wm, 'n', TUI_KEY_MOD_CTRL, new_file_handler);
tui_wm_register_hotkey(wm, 's', TUI_KEY_MOD_CTRL, save_file_handler);

// Cleanup
tui_wm_destroy(wm);
```

#### Widget Types

```c
TUI_WIDGET_PANEL      // Container with optional border
TUI_WIDGET_LABEL      // Static text
TUI_WIDGET_BUTTON     // Clickable button
TUI_WIDGET_TEXTBOX    // Text input field
TUI_WIDGET_CHECKBOX   // Boolean toggle
TUI_WIDGET_RADIO      // Radio button group
TUI_WIDGET_LIST       // Scrollable list
TUI_WIDGET_SCROLLBAR  // Scroll indicator
TUI_WIDGET_CUSTOM     // User-defined widget
```

#### Event Phases

Events flow through three phases (like DOM events):
1. **Capture Phase**: From root down to target (for global interception)
2. **Target Phase**: On the target widget itself
3. **Bubble Phase**: From target back up to root (for parent handling)

```c
// Custom event handler
bool my_handler(tui_widget* w, tui_widget_event* e) {
    if (e->phase == TUI_EVENT_PHASE_TARGET) {
        // Handle the event
        return true;  // Stop propagation
    }
    return false;  // Continue bubbling
}

widget->on_event = my_handler;
```

### Theming

```c
// Set theme
tui_set_theme(ctx, &TUI_THEME_DARK);

// Available themes
TUI_THEME_DEFAULT
TUI_THEME_DARK
TUI_THEME_LIGHT
TUI_THEME_BLUE
TUI_THEME_GREEN

// Access theme colors
const tui_theme* t = tui_get_theme(ctx);
tui_set_fg(ctx, t->accent);

// Color interpolation for animations
tui_color lerped = tui_lerp_color(color1, color2, 0.5f);
```

## Editor Features

### Cursor Shapes

```c
tui_set_cursor_shape(ctx, TUI_CURSOR_BAR);        // Beam cursor for insert mode
tui_set_cursor_shape(ctx, TUI_CURSOR_BLOCK);      // Block cursor for normal mode
tui_set_cursor_shape(ctx, TUI_CURSOR_UNDERLINE);  // Underline cursor

// All shapes: DEFAULT, BLOCK_BLINK, BLOCK, UNDERLINE_BLINK, UNDERLINE, BAR_BLINK, BAR
```

### Bracketed Paste

```c
tui_enable_bracketed_paste(ctx);

while (tui_poll_event(ctx, &event)) {
    if (event.type == TUI_EVENT_PASTE_START) {
        // User started pasting - disable auto-indent, etc.
    }
    if (event.type == TUI_EVENT_PASTE_END) {
        // Paste complete
    }
    if (tui_is_pasting(ctx)) {
        // Currently receiving pasted text
    }
}
```

### Focus Events

```c
tui_enable_focus_events(ctx);

if (event.type == TUI_EVENT_FOCUS_IN) {
    // Terminal gained focus - refresh display
}
if (event.type == TUI_EVENT_FOCUS_OUT) {
    // Terminal lost focus - save state
}
```

### Clipboard (OSC 52)

```c
tui_clipboard_set(ctx, "Text to copy");  // Copy to system clipboard
```

### Hyperlinks

```c
tui_hyperlink_start(ctx, "https://example.com");
tui_label(ctx, x, y, "Click me!");
tui_hyperlink_end(ctx);
```

### Colored Underlines / Undercurl

```c
// Undercurl with red color (like vim spelling errors)
tui_set_style(ctx, TUI_STYLE_UNDERCURL);
tui_set_underline_color(ctx, TUI_COLOR_RED);
tui_label(ctx, x, y, "Misspelled");
tui_reset_underline_color(ctx);

// Available styles: TUI_STYLE_STRIKETHROUGH, TUI_STYLE_UNDERCURL
```

### Wide Characters (CJK)

```c
// Automatically handled in tui_label
tui_label(ctx, x, y, "Hello 世界! 🎉");

// Manual wide char placement
int width = tui_char_width(codepoint);  // Returns 0, 1, or 2
tui_set_cell_wide(ctx, x, y, codepoint);  // For double-width chars
```

## Keyboard Input

| Key | Constant |
|-----|----------|
| Escape | `TUI_KEY_ESC` |
| Enter | `TUI_KEY_ENTER` |
| Tab | `TUI_KEY_TAB` |
| Backspace | `TUI_KEY_BACKSPACE` |
| Arrow Up/Down/Left/Right | `TUI_KEY_UP/DOWN/LEFT/RIGHT` |
| Home / End | `TUI_KEY_HOME` / `TUI_KEY_END` |
| Page Up / Down | `TUI_KEY_PGUP` / `TUI_KEY_PGDN` |
| F1 - F12 | `TUI_KEY_F1` - `TUI_KEY_F12` |
| Insert / Delete | `TUI_KEY_INSERT` / `TUI_KEY_DELETE` |

## Mouse Input

```c
tui_enable_mouse(ctx);

while (tui_poll_event(ctx, &event)) {
    if (event.type == TUI_EVENT_MOUSE) {
        int x = event.mouse_x;
        int y = event.mouse_y;
        
        if (event.mouse_button == TUI_MOUSE_LEFT) { }
        if (event.mouse_button == TUI_MOUSE_RIGHT) { }
        if (event.mouse_button == TUI_MOUSE_WHEEL_UP) { }
    }
}

tui_disable_mouse(ctx);
```

## Colors

```c
// Standard colors
TUI_COLOR_BLACK, TUI_COLOR_RED, TUI_COLOR_GREEN,
TUI_COLOR_YELLOW, TUI_COLOR_BLUE, TUI_COLOR_MAGENTA,
TUI_COLOR_CYAN, TUI_COLOR_WHITE, TUI_COLOR_DEFAULT

// 24-bit RGB
tui_color custom = TUI_RGB(255, 128, 64);
tui_set_fg(ctx, custom);
tui_set_bg(ctx, custom);
```

## Platform Notes

- **Windows**: Requires Windows 10+ with VT processing enabled
- **SSH/tmux**: Works correctly over remote connections
- **Terminal Size**: Automatically handles `SIGWINCH` on POSIX systems
- **Synchronized Output**: Uses DEC mode 2026 for tear-free rendering

## License

MIT License - see [LICENSE](LICENSE) for details.
