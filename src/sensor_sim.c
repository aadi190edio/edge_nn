#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include "message.h" // Ensure this matches your filename

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
    uint32_t runmask = (1 << 1);
    ThreadCtl(_NTO_TCTL_RUNMASK, (void *)(uintptr_t)runmask);

    int inference_coid = -1;
    while ((inference_coid = name_open(ATTACH_POINT_INFERENCE, 0)) == -1) {
        printf("[SENSOR] Waiting for Inference Engine...\n");
        sleep(1);
    }

    float temp = 60.0; float vib = 1.0;

    for (int i = 1; i <= 20; i++) {
        sensor_msg_t msg = {MSG_TYPE_SENSOR_DATA, temp, vib};
        MsgSend(inference_coid, &msg, sizeof(msg), NULL, 0);

        print_log_header("SENSOR");
        printf("Sent Data -> Temp: %.1f, Vib: %.1f\n", temp, vib);

        if (i > 8) { temp += 8.5; vib += 1.2; }
        else { temp += 0.5; vib += 0.1; }
        delay(500);
    }
    return 0;
}
