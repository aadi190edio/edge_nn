#ifndef MESSAGES_H
#define MESSAGES_H

#include <sys/iomsg.h>
#include <stdint.h>

// QNX Channel names
#define ATTACH_POINT_INFERENCE "factory_inference"
#define ATTACH_POINT_ACTUATOR  "factory_actuator"

// Custom message types
#define MSG_TYPE_SENSOR_DATA (_IO_MAX + 1)
#define MSG_TYPE_SHUTDOWN    (_IO_MAX + 2)

// Payload from Sensor -> edge_nn
typedef struct {
    uint16_t type;
    float temp;
    float vibration;
} sensor_msg_t;

// Payload from edge_nn -> actuator
typedef struct {
    uint16_t type;
    int state;
} shutdown_msg_t;

#endif
