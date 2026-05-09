# QNX Autonomous Factory Grid Controller

A real-time autonomous factory monitoring and safety control system built on **QNX SDP 8.0** for the **Raspberry Pi 4**.

The project simulates industrial thermal and vibration telemetry, performs **edge-side neural network inference**, predicts mechanical failures, and triggers real-time safety actions with a visual HMI dashboard.

---

# System Overview

The application follows a **region-based multicore architecture**, where each critical process is pinned to a dedicated CPU core to ensure deterministic execution and reduced resource contention.

| Process | Role | CPU Core | Functionality |
|---|---|---|---|
| `factory_hmi` | Visualization | Core 0 | Displays real-time factory state using QNX Screen |
| `sensor_sim` | Sensor Simulation | Core 1 | Generates thermal and vibration telemetry |
| `edge_nn` | AI Inference Engine | Core 2 | Runs neural network inference for fault prediction |
| `actuator` | Safety Controller | Core 3 | Executes emergency shutdown actions |

---

# Features

- Real-time deterministic execution using QNX Neutrino RTOS
- Multi-core process isolation
- Neural network based predictive maintenance
- Simulated industrial sensor telemetry
- Real-time safety shutdown logic
- Native QNX message passing IPC
- CPU affinity and real-time scheduling
- Live HMI dashboard using QNX Screen
- Raspberry Pi 4 deployment support

---

# Project Architecture

```text
+------------------+
|   sensor_sim     |
|  (Core 1)        |
| Thermal + Vib    |
+--------+---------+
         |
         v
+------------------+
|     edge_nn      |
|    (Core 2)      |
| Neural Network   |
| Inference Engine |
+----+--------+----+
     |        |
     v        v
+---------+  +----------------+
|actuator |  |  factory_hmi   |
|(Core 3) |  |   (Core 0)     |
|Shutdown |  | Visual Display |
+---------+  +----------------+
