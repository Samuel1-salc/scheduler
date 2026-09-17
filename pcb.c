#include "pcb.h"

#include <stdio.h>
#include <stdlib.h>

PCB *PCB_create(int pid,int prioridade,int tempo_execucao)
{
    PCB *processo = malloc(sizeof(PCB));

    if (processo == NULL) {
        return NULL;
    }

    processo->pid = pid;
    processo->prioridade = prioridade;
    processo->tempo_execucao = tempo_execucao;
    processo->tempo_restante = tempo_execucao;
    processo->estado = READY;

    return processo;
}

void PCB_destroy(PCB *processo)
{
    free(processo);
}

void PCB_print(PCB *processo)
{
    printf(
        "PID: %d | Prioridade: %d | Execucao: %d | Restante: %d\n",
        processo->pid,
        processo->prioridade,
        processo->tempo_execucao,
        processo->tempo_restante
    );
}