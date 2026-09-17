#ifndef PCB_H
#define PCB_H
#include <stdbool.h>

typedef enum {
    READY,
    RUNNING,
    FINISHED
} ProcessState;


typedef struct PCB {
    int pid;
    int prioridade;
    int tempo_execucao;
    int tempo_restante;
    ProcessState estado;
} PCB;


PCB *PCB_create(
    int pid,
    int prioridade,
    int tempo_execucao
);

void PCB_destroy(PCB *processo);

#endif // LINKED_PCB_STRUCT_H
