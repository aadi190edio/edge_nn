#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <screen/screen.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include "message.h"

// Industrial Colors (A-R-G-B)
#define COLOR_SAFE  0xFF005500 // Deep Green
#define COLOR_WARN  0xFF888800 // Amber
#define COLOR_CRIT  0xFF880000 // Deep Red
#define COLOR_BLACK 0xFF000000

int main() {
    // 1. PIN TO CORE 0
    uint32_t runmask = (1 << 3);
    ThreadCtl(_NTO_TCTL_RUNMASK, (void *)(uintptr_t)runmask);

    screen_context_t screen_ctx;
    screen_window_t screen_win;
    screen_buffer_t screen_buf;
    int rect[4] = { 0, 0, 800, 480 }; // Standard HMI Resolution
    int usage = SCREEN_USAGE_WRITE | SCREEN_USAGE_NATIVE;
    int format = SCREEN_FORMAT_RGBA8888;
    int transparency = SCREEN_TRANSPARENCY_NONE;

    // 2. INITIALIZE QNX SCREEN
    if (screen_create_context(&screen_ctx, SCREEN_APPLICATION_CONTEXT) != 0) {
        perror("Could not create screen context. Is 'screen' service running?");
        return EXIT_FAILURE;
    }

    screen_create_window(&screen_win, screen_ctx);
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, &usage);
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_FORMAT, &format);
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SIZE, rect + 2);
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_TRANSPARENCY, &transparency);

    // 3. SETUP BUFFERS
    screen_create_window_buffers(screen_win, 1);
    screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&screen_buf);

    // 4. ATTACH IPC CHANNEL
    name_attach_t *attach = name_attach(NULL, "factory_hmi_service", 0);
    if (attach == NULL) {
        perror("Failed to create HMI IPC channel");
        return EXIT_FAILURE;
    }

    printf("[09/05/2026][CORE_0][HMI] Dashboard Ready on HDMI-1.\n");

    while (1) {
        sensor_msg_t msg;
        int rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

        if (rcvid > 0) {
            MsgReply(rcvid, EOK, NULL, 0);

            // 5. GET BUFFER POINTER
            void *ptr = NULL;
            screen_get_buffer_property_pv(screen_buf, SCREEN_PROPERTY_POINTER, &ptr);
            int stride = 0;
            screen_get_buffer_property_iv(screen_buf, SCREEN_PROPERTY_STRIDE, &stride);

            // 6. DETERMINE STATUS COLOR
            uint32_t current_color = COLOR_SAFE;
            if (msg.temp > 85.0) current_color = COLOR_CRIT;
            else if (msg.temp > 75.0) current_color = COLOR_WARN;

            // 7. DRAW PIXELS (Industrial Background)
            for (int y = 0; y < rect[3]; y++) {
                uint32_t *line = (uint32_t *)((char *)ptr + y * stride);
                for (int x = 0; x < rect[2]; x++) {
                    line[x] = current_color;
                }
            }

            // 8. FLUSH TO DISPLAY
            screen_post_window(screen_win, screen_buf, 1, rect, 0);
        }
    }

    // Cleanup
    name_detach(attach, 0);
    screen_destroy_window(screen_win);
    screen_destroy_context(screen_ctx);
    return 0;
}
