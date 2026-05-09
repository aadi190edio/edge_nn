CC = qcc -Vgcc_ntoaarch64le
CFLAGS = -Wall -O2
LDFLAGS_NN = -lm
OUT_DIR = build/aarch64le

all: setup actuator edge_nn sensor_sim factory_hmi

setup:
	mkdir -p $(OUT_DIR)

actuator: src/actuator.c src/message.h
	$(CC) $(CFLAGS) src/actuator.c -o $(OUT_DIR)/actuator -lm

edge_nn: src/edge_nn.c src/nn.h src/model.h src/message.h
	$(CC) $(CFLAGS) src/edge_nn.c -o $(OUT_DIR)/edge_nn $(LDFLAGS_NN)

sensor_sim: src/sensor_sim.c src/message.h
	$(CC) $(CFLAGS) src/sensor_sim.c -o $(OUT_DIR)/sensor_sim

factory_hmi: src/factory_hmi.c
	$(CC) $(CFLAGS) src/factory_hmi.c -o build/aarch64le/factory_hmi -lscreen -lm
	
clean:
	rm -rf build/