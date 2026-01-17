/*
 * hello.c - TUI Label Demo
 *
 * Demonstrates basic label rendering with the TUI library.
 * Press ESC or 'q' to exit.
 */

#define TUI_IMPLEMENTATION
#include "../tui.h"

#include <stdbool.h>

#ifdef _WIN32
    #include <windows.h>
    #define sleep_ms(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
#endif

int main(void) {
    /* Create TUI context */
    tui_context* ctx = tui_create();
    if (!ctx) {
        return 1;
    }
    
    bool running = true;
    int frame = 0;
    
    while (running) {
        /* Poll for input events */
        tui_event event;
        while (tui_poll_event(ctx, &event)) {
            if (event.key == TUI_KEY_ESC) {
                running = false;
            }
            if (event.key == TUI_KEY_CHAR && event.ch == 'q') {
                running = false;
            }
        }
        
        /* Begin frame */
        tui_begin_frame(ctx);
        
        int width = tui_get_width(ctx);
        int height = tui_get_height(ctx);
        
        /* Draw border */
        tui_set_fg(ctx, TUI_COLOR_CYAN);
        for (int x = 0; x < width; x++) {
            tui_set_cell(ctx, x, 0, '-');
            tui_set_cell(ctx, x, height - 1, '-');
        }
        for (int y = 0; y < height; y++) {
            tui_set_cell(ctx, 0, y, '|');
            tui_set_cell(ctx, width - 1, y, '|');
        }
        tui_set_cell(ctx, 0, 0, '+');
        tui_set_cell(ctx, width - 1, 0, '+');
        tui_set_cell(ctx, 0, height - 1, '+');
        tui_set_cell(ctx, width - 1, height - 1, '+');
        
        /* Draw title */
        tui_set_fg(ctx, TUI_COLOR_YELLOW);
        tui_set_style(ctx, TUI_STYLE_BOLD);
        tui_label(ctx, 2, 2, "TUI Library - Hello World Demo");
        
        /* Draw info labels */
        tui_set_fg(ctx, TUI_COLOR_WHITE);
        tui_set_style(ctx, TUI_STYLE_NONE);
        tui_label(ctx, 2, 4, "This is a simple demonstration of the TUI library.");
        tui_label(ctx, 2, 5, "It shows how to render labels on the terminal.");
        
        /* Draw terminal info */
        tui_set_fg(ctx, TUI_COLOR_GREEN);
        char info[64];
        snprintf(info, sizeof(info), "Terminal size: %d x %d", width, height);
        tui_label(ctx, 2, 7, info);
        
        snprintf(info, sizeof(info), "Frame: %d", frame);
        tui_label(ctx, 2, 8, info);
        
        /* Draw animated spinner */
        tui_set_fg(ctx, TUI_COLOR_MAGENTA);
        const char* spinner = "|/-\\";
        char spin_str[2] = { spinner[frame % 4], '\0' };
        tui_label(ctx, 2, 10, "Processing: ");
        tui_set_fg(ctx, TUI_COLOR_YELLOW);
        tui_label(ctx, 14, 10, spin_str);
        
        /* Draw colored boxes */
        tui_set_style(ctx, TUI_STYLE_NONE);
        
        tui_set_fg(ctx, TUI_COLOR_RED);
        tui_label(ctx, 2, 12, "RED");
        
        tui_set_fg(ctx, TUI_COLOR_GREEN);
        tui_label(ctx, 8, 12, "GREEN");
        
        tui_set_fg(ctx, TUI_COLOR_BLUE);
        tui_label(ctx, 16, 12, "BLUE");
        
        tui_set_fg(ctx, TUI_COLOR_YELLOW);
        tui_label(ctx, 23, 12, "YELLOW");
        
        tui_set_fg(ctx, TUI_COLOR_MAGENTA);
        tui_label(ctx, 32, 12, "MAGENTA");
        
        tui_set_fg(ctx, TUI_COLOR_CYAN);
        tui_label(ctx, 42, 12, "CYAN");
        
        /* Draw RGB gradient */
        tui_set_style(ctx, TUI_STYLE_NONE);
        for (int i = 0; i < 32 && 2 + i < width - 1; i++) {
            uint32_t color = TUI_RGB(i * 8, 0, 255 - i * 8);
            tui_set_fg(ctx, color);
            tui_set_cell(ctx, 2 + i, 14, '#');
        }
        
        /* Draw instructions */
        tui_set_fg(ctx, TUI_RGB(150, 150, 150));
        tui_label(ctx, 2, height - 3, "Press 'q' or ESC to exit");
        
        /* End frame */
        tui_end_frame(ctx);
        
        frame++;
        
        /* Sleep to limit frame rate (~30 FPS) */
        sleep_ms(33);
    }
    
    /* Cleanup */
    tui_destroy(ctx);
    
    return 0;
}
