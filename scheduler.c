#include "scheduler.h"

#include <stdio.h>
#include <stdlib.h>


Scheduler *Scheduler_create(void){

    //alocando espaço para o scheduler
    Scheduler *scheduler = malloc(sizeof(Scheduler));
    
    if (scheduler != NULL){

        // criando filas, Caso alguma não seja criada corretamente todas as outras serão excluidas e retornará NULL.
        for (int i = 0 ; i < NUM_FILAS ; i++){
            scheduler->filas[i] = CircularList_creat();
            if (scheduler->filas[i] == NULL){
                for (int j = i; j -- ;){
                    free(scheduler->filas[j]);
                }
                free (scheduler);
                return NULL;
            }
        }
        // como a verificação de filas já foi feita, posso criar os quantums.
        scheduler->quantums[0] = 2;
        scheduler->quantums[1] = 4;
        scheduler->quantums[2] = 6;
        scheduler->quantums[3] = 8;

        
        return scheduler;
        

    }else{
        return NULL;
    }
}

/* Adicionando um processo na fila correspondente à sua prioridade */
bool Scheduler_addProcess(
    Scheduler *scheduler,
    PCB *processo
)
{
        if (scheduler != NULL && processo != NULL){
            if (processo->prioridade < NUM_FILAS && processo->prioridade >= 0 ){
                CircularList_Insert(scheduler->filas[processo->prioridade], processo);
                return true;
            }else{
                printf("prioridade errada");
                return false;
            }
        }else{
            printf("scheduler ou processo nulos");
            return false;
        }

}

/* Executa o escalonamento */

void Scheduler_run(
Scheduler* scheduler
){

    if (scheduler == NULL){printf("scheduler NULL"); return;} 

    int passo = 1;
    printf("Passo | PID | Quantum usado | Restante | Estado\n");
    printf("------+-----+---------------+----------+--------\n");

    for ( int prioridade = 0 ; prioridade < NUM_FILAS ; prioridade ++){
        CircularList* fila = scheduler->filas[prioridade];
        int quantum = scheduler->quantums[prioridade];
        if (CircularList_Empty(fila)){
            printf("current lista vazia");
            continue;
        }else{
            do{
                Node* temp = fila->begin;
                PCB* processo = temp->processo;
                int quantum_usado = quantum;

                if (processo->tempo_restante < quantum_usado) {
                    quantum_usado = processo->tempo_restante;
                }

                if (processo->estado == READY){
                    processo->estado = RUNNING;
                    for (int j = 0 ; j < quantum_usado; j ++){
                        processo->tempo_restante--;
                    }
                }

                if (processo->tempo_restante == 0) {
                    processo->estado = FINISHED;
                }

                Scheduler_print(
                    passo,
                    processo->pid,
                    quantum_usado,
                    processo->tempo_restante,
                    processo->estado
                );
                passo++;

                if (processo->estado == FINISHED) {
                    CircularList_Remove(fila, processo);
                    PCB_destroy(processo);
                } else {
                    processo->estado = READY;
                    //mover o primeiro processo para o final da fila circular para que seja reutilizável depois;
                    fila->begin = fila->begin->next;
                }
            }while(fila->size > 0);

        }

    }

}

void Scheduler_print(
    int passo,
    int pid,
    int quantum_usado,
    int restante,
    ProcessState estado
)
{
    const char *nome_estado;

    switch (estado) {
        case READY:
            nome_estado = "READY";
            break;
        case RUNNING:
            nome_estado = "RUNNING";
            break;
        case FINISHED:
            nome_estado = "FINISHED";
            break;
        default:
            nome_estado = "UNKNOWN";
            break;
    }

    printf(
        "%5d | %3d | %13d | %8d | %s\n",
        passo,
        pid,
        quantum_usado,
        restante,
        nome_estado
    );
}

void Scheduler_destroy(Scheduler *scheduler)
{
    if (scheduler == NULL) {
        return;
    }

    for (int prioridade = 0; prioridade < NUM_FILAS; prioridade++) {
        CircularList *fila = scheduler->filas[prioridade];

        while (!CircularList_Empty(fila)) {
            PCB *processo = fila->begin->processo;
            CircularList_Remove(fila, processo);
            PCB_destroy(processo);
        }

        free(fila);
    }

    free(scheduler);
}
