# QNX Autonomous Factory Grid Controller

A real-time autonomous factory monitoring and predictive maintenance system built on **QNX SDP 8.0** for the **Raspberry Pi 4**.

The project demonstrates:
- Real-time multi-core process isolation
- QNX native message passing
- Edge analytics using DSP + Neural Networks
- Predictive maintenance
- Safety-critical shutdown handling
- Industrial HMI visualization using QNX Screen

---

# System Overview

The system continuously monitors simulated thermal and vibration telemetry from factory machinery.  
A lightweight neural network performs real-time inference to predict abnormal machine behavior and potential bearing failures.

When a critical condition is detected:
- The HMI instantly changes to a critical state
- The safety actuator triggers an emergency halt

---

# Architecture

The application uses a **region-based multi-core architecture** where each process is pinned to a dedicated CPU core for deterministic execution and reduced contention.

| Process | Role | CPU Core | Description |
|---|---|---|---|
| `factory_hmi` | Visualization | Core 0 | Displays real-time machine status using QNX Screen |
| `sensor_sim` | Data Source | Core 1 | Simulates thermal and vibration sensor telemetry |
| `edge_nn` | AI Inference | Core 2 | Runs neural network inference and predictive analytics |
| `actuator` | Safety Relay | Core 3 | Executes emergency shutdown logic |

---

# Features

- QNX Neutrino RTOS
- Multi-core CPU affinity
- Native QNX IPC
- Deterministic real-time scheduling
- FFT-based vibration analytics
- RMS feature extraction
- Neural network inference engine
- Predictive bearing fault detection
- Safety shutdown simulation
- Real-time HMI dashboard
- Edge AI / TinyML-style inference pipeline

---

# Repository Structure

```text
.
├── src/
│   ├── actuator.c
│   ├── edge_nn.c
│   ├── factory_hmi.c
│   ├── sensor_sim.c
│   ├── message.h
│   ├── nn.h
│   └── model.h
│
├── build/
├── Makefile
└── README.md
```

---

# File Descriptions

## `src/edge_nn.c`

Central inference engine responsible for:
- Receiving sensor telemetry
- Running neural network forward propagation
- Predicting machine state
- Sending status updates to HMI and actuator

---

## `src/actuator.c`

High-priority real-time safety process.

Responsibilities:
- Receives shutdown requests
- Simulates emergency safety relay behavior
- Executes deterministic halt handling

Configured with:
- `SCHED_FIFO`
- Real-time priority 60

---

## `src/sensor_sim.c`

Sensor telemetry simulator.

Generates:
- Temperature signals
- Vibration waveforms
- Progressive bearing degradation patterns

---

## `src/factory_hmi.c`

Industrial visualization dashboard built using:
- QNX Screen Graphics Subsystem

Displays:
- Green → SAFE
- Amber → WARNING
- Red → CRITICAL

---

## `src/message.h`

Defines:
- IPC message structures
- Channel names
- Shared protocol definitions

Used for:
- QNX native message passing

---

## `src/nn.h` and `src/model.h`

Contains:
- Neural network architecture
- Activation functions
- Pre-trained weights and biases
- Inference utilities

---

## `Makefile`

Handles:
- Cross-compilation for `aarch64le`
- Math library linking (`-lm`)
- QNX Screen linking (`-lscreen`)

---

# Real-Time Design

The project utilizes several QNX RTOS capabilities:

- `ThreadCtl()` for CPU affinity
- `SCHED_FIFO` scheduling
- `mlockall()` memory locking
- Native QNX message passing
- Pulse/timer driven execution
- Deterministic process isolation

---

# Edge Analytics Pipeline

```text
Sensor Stream
      ↓
Signal Acquisition
      ↓
FFT / RMS Extraction
      ↓
Feature Generation
      ↓
Neural Network Inference
      ↓
Fault Prediction
      ↓
Safety Actuation + HMI Update
```

---

# Building the Project

## 1. Initialize QNX Environment

Ensure the QNX SDP 8.0 environment is configured.

Example:

```bash
source qnxsdp-env.sh
```

---

## 2. Build

```bash
make clean
make
```

Compiled binaries will be generated inside:

```text
build/aarch64le/
```

---

# Deployment to Raspberry Pi 4

## 1. Transfer binaries

```bash
scp build/aarch64le/* qnxuser@<RPi4_IP>:/tmp/
```

---

## 2. Set executable permissions

On the Raspberry Pi 4:

```bash
chmod +x /tmp/actuator
chmod +x /tmp/edge_nn
chmod +x /tmp/sensor_sim
chmod +x /tmp/factory_hmi
```

---

# Execution Order

Start processes in the following order:

## 1. Safety Actuator

```bash
/tmp/actuator &
```

---

## 2. HMI Dashboard

```bash
/tmp/factory_hmi &
```

---

## 3. Inference Engine

```bash
/tmp/edge_nn &
```

---

## 4. Sensor Simulator

```bash
/tmp/sensor_sim
```

---

# Monitoring and Results

## Process Logs

Each process outputs:
- timestamps
- CPU core usage
- system state transitions
- prediction results

---

## HMI Dashboard

If a monitor is connected to the Raspberry Pi HDMI output:

| Color | State |
|---|---|
| Green | SAFE |
| Amber | WARNING |
| Red | CRITICAL |

---

## Safety Trigger

When a critical failure is predicted:

```text
CRITICAL BEARING FAILURE PREDICTED
```

The actuator process immediately:
- logs the fault
- simulates a GPIO safety halt
- transitions the system into shutdown state

---

# Technologies Used

- QNX SDP 8.0
- C
- POSIX RT APIs
- QNX Neutrino IPC
- FFT Signal Processing
- TinyML-style inference
- Raspberry Pi 4
- QNX Screen Graphics

---

# Future Improvements

- Real sensor integration (I2C/SPI/CAN)
- Shared-memory telemetry transport
- Distributed Qnet nodes
- TinyML accelerator integration
- Hardware GPIO shutdown
- Persistent telemetry logging
- Web-based remote monitoring
- Adaptive anomaly detection

---

# License

MIT License

---

# Author

Adithya Y  
Embedded Systems | RTOS | Edge AI | Robotics
