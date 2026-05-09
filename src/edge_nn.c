#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include "nn.h"
#include "model.h"
#include "message.h"

// Helper for timestamped logging
void print_log_header(const char* region) {
    time_t now;
    struct tm *t;
    time(&now);
    t = localtime(&now);
    printf("[%02d/%02d/%d %02d:%02d:%02d][CORE_%d][%s] ",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec,
            SYSPAGE_ENTRY(cpuinfo)->cpu, region);
}

int main() {
    // 1. CPU Affinity: Dual-core mode (Cores 0 & 2)
    uint32_t runmask = (1 << 0);
    ThreadCtl(_NTO_TCTL_RUNMASK, (void *)(uintptr_t)runmask);

    // 2. Connect to Safety Actuator
    int actuator_coid = -1;
    while ((actuator_coid = name_open(ATTACH_POINT_ACTUATOR, 0)) == -1) {
        sleep(1);
    }

    // 3. NEW: Connect to the HMI Service
    int hmi_coid = -1;
    printf("[EDGE_NN] Searching for HMI Dashboard...\n");
    while ((hmi_coid = name_open("factory_hmi_service", 0)) == -1) {
        sleep(1); // Wait for factory_hmi to start
    }
    printf("[EDGE_NN] HMI Connected.\n");

    // 4. Setup Server for Sensor Data
    name_attach_t *attach = name_attach(NULL, ATTACH_POINT_INFERENCE, 0);
    sensor_msg_t msg;
    int rcvid;

    while (1) {
        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
        if (rcvid > 0 && msg.type == MSG_TYPE_SENSOR_DATA) {
            MsgReply(rcvid, EOK, NULL, 0);

            // 5. Neural Network Inference
            float in[2] = {msg.temp, msg.vibration};
            float h[HIDDEN], out[OUTPUT];
            forward(in, W1, b1, W2, b2, h, out);
            int p = argmax(out, OUTPUT);

            print_log_header("INFERENCE");
            printf("Temp: %.1f -> Result: %s\n",
                    msg.temp, (p==2) ? "CRITICAL" : "NORMAL");

            // 6. Update the HMI (Every cycle)
            // We send the current sensor data so the HMI can update its color
            MsgSend(hmi_coid, &msg, sizeof(msg), NULL, 0);

            // 7. Trigger Safety Actuator (Only if Critical)
            if (p == 2) {
                shutdown_msg_t alert = {MSG_TYPE_SHUTDOWN, p};
                MsgSend(actuator_coid, &alert, sizeof(alert), NULL, 0);
            }
        }
    }
    return 0;
}
