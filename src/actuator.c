#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include "message.h"

int main() {
    // 1. CPU AFFINITY: Pin this process to Core 3 ONLY (Bitmask: 0x08)
    uint32_t runmask = (1 << 2);
    if (ThreadCtl(_NTO_TCTL_RUNMASK, (void *)(uintptr_t)runmask) == -1) {
        perror("[ACTUATOR] Failed to set CPU runmask");
    }

    // 2. Set strict real-time priority
    struct sched_param param;
    sched_getparam(0, &param);
    param.sched_priority = 60;
    sched_setscheduler(0, SCHED_RR, &param);

    name_attach_t *attach = name_attach(NULL, ATTACH_POINT_ACTUATOR, 0);
    shutdown_msg_t msg;
    int rcvid;

    printf("[ACTUATOR] Online. Pinned to CORE 3. Priority: 60.\n");

    while (1) {
        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

        if (rcvid > 0 && msg.type == MSG_TYPE_SHUTDOWN) {
            if (msg.state == 2) {
                printf("\n[ACTUATOR] 🚨 CRITICAL BEARING FAILURE PREDICTED! 🚨\n");
                printf("[ACTUATOR] TRIGGERING GPIO HALT! SHUTTING DOWN LINE!\n\n");
            }
            MsgReply(rcvid, EOK, NULL, 0);
        }
    }
    return 0;
}
