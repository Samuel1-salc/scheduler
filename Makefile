CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = scheduler

OBJ = main.o pcb.o fila.o scheduler.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

main.o: main.c pcb.h fila.h scheduler.h
	$(CC) $(CFLAGS) -c main.c

pcb.o: pcb.c pcb.h
	$(CC) $(CFLAGS) -c pcb.c

fila.o: fila.c fila.h pcb.h
	$(CC) $(CFLAGS) -c fila.c

scheduler.o: scheduler.c scheduler.h fila.h pcb.h
	$(CC) $(CFLAGS) -c scheduler.c

clean:
	rm -f $(OBJ) $(TARGET)