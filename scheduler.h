#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "fila.h"

#define NUM_FILAS 4

#include<stdbool.h>

typedef struct Scheduler {
    CircularList *filas[NUM_FILAS];
    int quantums[NUM_FILAS];
} Scheduler;


/**
 * Cria uma nova scrut para scheduler
 * @return Um ponteiro para o scheduler Paradigma criado
 */
Scheduler *Scheduler_create(void);
void Scheduler_destroy(Scheduler *scheduler);


/* Adiciona um processo na fila correspondente à sua prioridade */
/**
 * @param 
 */
bool Scheduler_addProcess(
    Scheduler *scheduler,
    PCB *processo
);

void Schedular_Destroy(Scheduler *schedular);


/* Executa o escalonamento */

void Scheduler_run(
    Scheduler *scheduler
);


/* Exibe uma linha do historico de execucao */
void Scheduler_print(
    int passo,
    int pid,
    int quantum_usado,
    int restante,
    ProcessState estado
);

#endif