CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = scheduler disk-scheduler

DISK_OBJ = disk_main.o disk_request.o fila.o disk_scheduler.o
CPU_OBJ = cpu/main.o cpu/scheduler.o cpu/fila.o pcb.o
OBJ = $(CPU_OBJ) $(DISK_OBJ)

all: $(TARGET)

scheduler: $(CPU_OBJ)
	$(CC) $(CFLAGS) $(CPU_OBJ) -o $@

disk-scheduler: $(DISK_OBJ)
	$(CC) $(CFLAGS) $(DISK_OBJ) -o $@

cpu/%.o: cpu/%.c cpu/scheduler.h cpu/fila.h pcb.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

pcb.o: pcb.c pcb.h
	$(CC) $(CFLAGS) -c pcb.c

disk_main.o: disk_main.c disk_scheduler.h fila.h disk_request.h
	$(CC) $(CFLAGS) -c disk_main.c

main.o: main.c disk_request.h fila.h disk_scheduler.h
	$(CC) $(CFLAGS) -c main.c

disk_request.o: disk_request.c disk_request.h
	$(CC) $(CFLAGS) -c disk_request.c

fila.o: fila.c fila.h disk_request.h
	$(CC) $(CFLAGS) -c fila.c

disk_scheduler.o: disk_scheduler.c disk_scheduler.h fila.h disk_request.h
	$(CC) $(CFLAGS) -c disk_scheduler.c

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_BIN)

.PHONY: all test clean
TEST_BIN = tests/test_fila tests/test_scheduler

test: $(TEST_BIN) $(TARGET)
	./tests/test_fila
	./tests/test_scheduler
	python3 tests/test_disk_cli.py

tests/test_fila: tests/test_fila.c fila.c fila.h disk_request.c disk_request.h
	$(CC) $(CFLAGS) -I. tests/test_fila.c fila.c disk_request.c -Wl,--wrap=malloc -o $@

tests/test_scheduler: tests/test_scheduler.c fila.c fila.h disk_request.c disk_request.h disk_scheduler.c disk_scheduler.h
	$(CC) $(CFLAGS) -I. tests/test_scheduler.c fila.c disk_request.c disk_scheduler.c -Wl,--wrap=malloc -o $@
