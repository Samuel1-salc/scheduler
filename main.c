#include <stdio.h>
#include <stdlib.h>

#include "pcb.h"
#include "scheduler.h"

int main(void)
{
        Scheduler *scheduler = Scheduler_create();

        if (scheduler == NULL) {
                fprintf(stderr, "Erro ao criar o scheduler.\n");
                return EXIT_FAILURE;
        }

        PCB *processos[] = {
                PCB_create(1, 0, 5),
                PCB_create(2, 1, 7),
                PCB_create(3, 2, 3),
                PCB_create(4, 0, 4)
        };
        const size_t quantidade = sizeof(processos) / sizeof(processos[0]);

        for (size_t i = 0; i < quantidade; i++) {
            if (processos[i] == NULL ||
                !Scheduler_addProcess(scheduler, processos[i])) {
                fprintf(stderr, "Erro ao adicionar o processo %zu.\n", i + 1);

                for (size_t j = i; j < quantidade; j++) {
                    PCB_destroy(processos[j]);
                }

                Scheduler_destroy(scheduler);
                return EXIT_FAILURE;
            }
        }

        printf("\nExecutando scheduler...\n");
        Scheduler_run(scheduler);

        Scheduler_destroy(scheduler);
        return EXIT_SUCCESS;
}


