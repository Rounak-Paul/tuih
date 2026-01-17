/*
 * input.c - TUI Keyboard Handling Demo
 *
 * Demonstrates keyboard input handling and event processing.
 * Shows all key presses and their types in real-time.
 * Press ESC three times in a row to exit.
 */

#define TUI_IMPLEMENTATION
#include "../tui.h"

#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #define sleep_ms(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
#endif

#define MAX_LOG_LINES 20
#define MAX_LINE_LEN  64

typedef struct {
    char lines[MAX_LOG_LINES][MAX_LINE_LEN];
    int count;
    int start;
} log_buffer;

static void log_add(log_buffer* log, const char* text) {
    int idx = (log->start + log->count) % MAX_LOG_LINES;
    strncpy(log->lines[idx], text, MAX_LINE_LEN - 1);
    log->lines[idx][MAX_LINE_LEN - 1] = '\0';
    
    if (log->count < MAX_LOG_LINES) {
        log->count++;
    } else {
        log->start = (log->start + 1) % MAX_LOG_LINES;
    }
}

static const char* log_get(log_buffer* log, int i) {
    if (i < 0 || i >= log->count) return "";
    return log->lines[(log->start + i) % MAX_LOG_LINES];
}

static const char* key_name(tui_key key) {
    switch (key) {
        case TUI_KEY_NONE:      return "NONE";
        case TUI_KEY_UP:        return "UP";
        case TUI_KEY_DOWN:      return "DOWN";
        case TUI_KEY_LEFT:      return "LEFT";
        case TUI_KEY_RIGHT:     return "RIGHT";
        case TUI_KEY_ENTER:     return "ENTER";
        case TUI_KEY_ESC:       return "ESC";
        case TUI_KEY_BACKSPACE: return "BACKSPACE";
        case TUI_KEY_TAB:       return "TAB";
        case TUI_KEY_CHAR:      return "CHAR";
        default:                return "UNKNOWN";
    }
}

int main(void) {
    /* Create TUI context */
    tui_context* ctx = tui_create();
    if (!ctx) {
        return 1;
    }
    
    bool running = true;
    int esc_count = 0;
    int event_count = 0;
    
    /* Input text buffer for demonstration */
    char input_text[128] = "";
    int input_pos = 0;
    
    /* Event log */
    log_buffer event_log = {{{0}}, 0, 0};
    log_add(&event_log, "Event log started...");
    
    while (running) {
        /* Poll for input events */
        tui_event event;
        while (tui_poll_event(ctx, &event)) {
            event_count++;
            
            /* Format event for log */
            char log_entry[MAX_LINE_LEN];
            
            if (event.key == TUI_KEY_CHAR) {
                if (event.ch >= 32 && event.ch < 127) {
                    snprintf(log_entry, sizeof(log_entry), 
                             "#%d: CHAR '%c' (0x%02X)", 
                             event_count, (char)event.ch, event.ch);
                } else {
                    snprintf(log_entry, sizeof(log_entry), 
                             "#%d: CHAR U+%04X", 
                             event_count, event.ch);
                }
            } else {
                snprintf(log_entry, sizeof(log_entry), 
                         "#%d: %s", 
                         event_count, key_name(event.key));
            }
            log_add(&event_log, log_entry);
            
            /* Handle ESC to exit (3 times in a row) */
            if (event.key == TUI_KEY_ESC) {
                esc_count++;
                if (esc_count >= 3) {
                    running = false;
                }
            } else {
                esc_count = 0;
            }
            
            /* Handle text input */
            if (event.key == TUI_KEY_CHAR && input_pos < 126) {
                if (event.ch >= 32 && event.ch < 127) {
                    input_text[input_pos++] = (char)event.ch;
                    input_text[input_pos] = '\0';
                }
            }
            
            if (event.key == TUI_KEY_BACKSPACE && input_pos > 0) {
                input_pos--;
                input_text[input_pos] = '\0';
            }
            
            if (event.key == TUI_KEY_ENTER) {
                /* Clear input on Enter */
                input_text[0] = '\0';
                input_pos = 0;
            }
        }
        
        /* Begin frame */
        tui_begin_frame(ctx);
        
        int width = tui_get_width(ctx);
        int height = tui_get_height(ctx);
        
        /* Draw header */
        tui_set_fg(ctx, TUI_COLOR_CYAN);
        tui_set_style(ctx, TUI_STYLE_BOLD);
        tui_label(ctx, 2, 1, "TUI Library - Keyboard Input Demo");
        tui_set_style(ctx, TUI_STYLE_NONE);
        
        /* Draw separator */
        tui_set_fg(ctx, TUI_RGB(60, 60, 60));
        for (int x = 0; x < width; x++) {
            tui_set_cell(ctx, x, 2, '-');
        }
        
        /* Draw instructions panel */
        tui_set_fg(ctx, TUI_COLOR_YELLOW);
        tui_label(ctx, 2, 4, "Instructions:");
        tui_set_fg(ctx, TUI_COLOR_WHITE);
        tui_label(ctx, 2, 5, "- Type any character to see it logged");
        tui_label(ctx, 2, 6, "- Use arrow keys to test navigation");
        tui_label(ctx, 2, 7, "- Press ESC three times to exit");
        tui_label(ctx, 2, 8, "- Backspace removes last character");
        tui_label(ctx, 2, 9, "- Enter clears input buffer");
        
        /* Draw input box */
        tui_set_fg(ctx, TUI_COLOR_GREEN);
        tui_label(ctx, 2, 11, "Input Buffer:");
        
        /* Draw input field background */
        tui_set_fg(ctx, TUI_COLOR_WHITE);
        tui_set_bg(ctx, TUI_RGB(40, 40, 40));
        tui_set_cell(ctx, 2, 12, '[');
        for (int i = 0; i < 40; i++) {
            if (i < input_pos) {
                tui_set_cell(ctx, 3 + i, 12, (uint32_t)input_text[i]);
            } else if (i == input_pos) {
                tui_set_bg(ctx, TUI_RGB(100, 100, 100));
                tui_set_cell(ctx, 3 + i, 12, ' ');
                tui_set_bg(ctx, TUI_RGB(40, 40, 40));
            } else {
                tui_set_cell(ctx, 3 + i, 12, ' ');
            }
        }
        tui_set_cell(ctx, 43, 12, ']');
        tui_set_bg(ctx, TUI_COLOR_DEFAULT);
        
        /* Draw event log panel */
        int log_x = 50;
        int log_y = 4;
        
        tui_set_fg(ctx, TUI_COLOR_MAGENTA);
        tui_label(ctx, log_x, log_y, "Event Log:");
        
        tui_set_fg(ctx, TUI_RGB(60, 60, 60));
        for (int y = log_y + 1; y < log_y + MAX_LOG_LINES + 2; y++) {
            tui_set_cell(ctx, log_x - 1, y, '|');
        }
        
        tui_set_fg(ctx, TUI_RGB(200, 200, 200));
        for (int i = 0; i < event_log.count && i < MAX_LOG_LINES; i++) {
            int display_idx = event_log.count - 1 - i;
            if (display_idx >= 0) {
                tui_label(ctx, log_x, log_y + 1 + i, log_get(&event_log, display_idx));
            }
        }
        
        /* Draw statistics */
        tui_set_fg(ctx, TUI_COLOR_GREEN);
        char stats[64];
        snprintf(stats, sizeof(stats), "Total events: %d", event_count);
        tui_label(ctx, 2, 14, stats);
        
        snprintf(stats, sizeof(stats), "Input length: %d", input_pos);
        tui_label(ctx, 2, 15, stats);
        
        if (esc_count > 0) {
            tui_set_fg(ctx, TUI_COLOR_YELLOW);
            snprintf(stats, sizeof(stats), "ESC pressed %d/3 times", esc_count);
            tui_label(ctx, 2, 17, stats);
        }
        
        /* Draw key legend */
        tui_set_fg(ctx, TUI_RGB(80, 80, 80));
        tui_label(ctx, 2, height - 2, "Arrow keys | Enter | Backspace | Tab | ESC (x3 to quit)");
        
        /* End frame */
        tui_end_frame(ctx);
        
        /* Sleep to limit frame rate */
        sleep_ms(16);
    }
    
    /* Cleanup */
    tui_destroy(ctx);
    
    return 0;
}
