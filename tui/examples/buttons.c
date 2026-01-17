/*
 * buttons.c - TUI Interactive Buttons Demo
 *
 * Demonstrates interactive button widgets with keyboard navigation.
 * Use Tab/Arrow keys to navigate, Enter to press, ESC to exit.
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

#define NUM_BUTTONS 5

typedef struct {
    int x;
    int y;
    const char* label;
} button_info;

int main(void) {
    /* Create TUI context */
    tui_context* ctx = tui_create();
    if (!ctx) {
        return 1;
    }
    
    bool running = true;
    int selected_button = 0;
    int press_count = 0;
    const char* last_pressed = "(none)";
    
    /* Define buttons */
    button_info buttons[NUM_BUTTONS] = {
        { 4, 6,  "Button 1" },
        { 4, 8,  "Button 2" },
        { 4, 10, "Button 3" },
        { 4, 12, "Save" },
        { 4, 14, "Exit" }
    };
    
    while (running) {
        /* Poll for input events */
        tui_event event;
        while (tui_poll_event(ctx, &event)) {
            if (event.key == TUI_KEY_ESC) {
                running = false;
            }
            
            /* Navigate buttons with arrow keys or tab */
            if (event.key == TUI_KEY_DOWN || event.key == TUI_KEY_TAB) {
                selected_button = (selected_button + 1) % NUM_BUTTONS;
            }
            if (event.key == TUI_KEY_UP) {
                selected_button = (selected_button - 1 + NUM_BUTTONS) % NUM_BUTTONS;
            }
            
            /* Press button with Enter */
            if (event.key == TUI_KEY_ENTER) {
                press_count++;
                last_pressed = buttons[selected_button].label;
                
                /* Handle Exit button */
                if (selected_button == 4) {
                    running = false;
                }
            }
            
            /* Keyboard shortcuts */
            if (event.key == TUI_KEY_CHAR) {
                if (event.ch == 'q' || event.ch == 'Q') {
                    running = false;
                }
                /* Number keys select buttons directly */
                if (event.ch >= '1' && event.ch <= '5') {
                    selected_button = (int)(event.ch - '1');
                }
            }
        }
        
        /* Begin frame */
        tui_begin_frame(ctx);
        
        int width = tui_get_width(ctx);
        int height = tui_get_height(ctx);
        
        /* Draw background border */
        tui_set_fg(ctx, TUI_RGB(80, 80, 80));
        for (int x = 0; x < width; x++) {
            tui_set_cell(ctx, x, 0, '=');
            tui_set_cell(ctx, x, height - 1, '=');
        }
        for (int y = 1; y < height - 1; y++) {
            tui_set_cell(ctx, 0, y, '|');
            tui_set_cell(ctx, width - 1, y, '|');
        }
        
        /* Draw title */
        tui_set_fg(ctx, TUI_COLOR_CYAN);
        tui_set_style(ctx, TUI_STYLE_BOLD);
        tui_label(ctx, 2, 2, "TUI Library - Button Demo");
        tui_set_style(ctx, TUI_STYLE_NONE);
        
        /* Draw instructions */
        tui_set_fg(ctx, TUI_RGB(180, 180, 180));
        tui_label(ctx, 2, 4, "Use UP/DOWN or TAB to navigate, ENTER to press");
        
        /* Draw buttons */
        for (int i = 0; i < NUM_BUTTONS; i++) {
            /* Set hot button for the selected one */
            if (i == selected_button) {
                /* Temporarily set hot button coordinates */
                /* This is a workaround since we need internal access */
                tui_set_fg(ctx, TUI_COLOR_BLACK);
                tui_set_bg(ctx, TUI_COLOR_WHITE);
                tui_set_style(ctx, TUI_STYLE_BOLD);
            } else {
                tui_set_fg(ctx, TUI_COLOR_WHITE);
                tui_set_bg(ctx, TUI_RGB(60, 60, 60));
                tui_set_style(ctx, TUI_STYLE_NONE);
            }
            
            /* Draw button manually */
            int bx = buttons[i].x;
            int by = buttons[i].y;
            const char* label = buttons[i].label;
            int len = (int)strlen(label);
            
            tui_set_cell(ctx, bx, by, '[');
            tui_set_cell(ctx, bx + 1, by, ' ');
            for (int j = 0; j < len; j++) {
                tui_set_cell(ctx, bx + 2 + j, by, (uint32_t)label[j]);
            }
            tui_set_cell(ctx, bx + 2 + len, by, ' ');
            tui_set_cell(ctx, bx + 3 + len, by, ']');
            
            /* Draw selection indicator */
            tui_set_bg(ctx, TUI_COLOR_DEFAULT);
            if (i == selected_button) {
                tui_set_fg(ctx, TUI_COLOR_YELLOW);
                tui_set_cell(ctx, bx - 2, by, '>');
            }
        }
        
        /* Reset colors for status area */
        tui_set_fg(ctx, TUI_COLOR_DEFAULT);
        tui_set_bg(ctx, TUI_COLOR_DEFAULT);
        tui_set_style(ctx, TUI_STYLE_NONE);
        
        /* Draw status panel */
        int panel_x = 30;
        int panel_y = 6;
        
        tui_set_fg(ctx, TUI_COLOR_GREEN);
        tui_label(ctx, panel_x, panel_y, "Status:");
        
        tui_set_fg(ctx, TUI_COLOR_WHITE);
        char status[64];
        snprintf(status, sizeof(status), "Selected: %s", buttons[selected_button].label);
        tui_label(ctx, panel_x, panel_y + 2, status);
        
        snprintf(status, sizeof(status), "Press count: %d", press_count);
        tui_label(ctx, panel_x, panel_y + 3, status);
        
        snprintf(status, sizeof(status), "Last pressed: %s", last_pressed);
        tui_label(ctx, panel_x, panel_y + 4, status);
        
        /* Draw key hints */
        tui_set_fg(ctx, TUI_RGB(100, 100, 100));
        tui_label(ctx, 2, height - 3, "[1-5] Select  [Enter] Press  [ESC/q] Exit");
        
        /* End frame */
        tui_end_frame(ctx);
        
        /* Sleep to limit frame rate */
        sleep_ms(33);
    }
    
    /* Cleanup */
    tui_destroy(ctx);
    
    return 0;
}
