#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "disk_scheduler.h"

static const char *direction(DiskDirection value)
{
    return value == DIRECTION_DOWN ? "descendo" : "subindo";
}

static bool submit(DiskScheduler *scheduler, int id, int cylinder)
{
    DiskRequest *request = DiskRequest_create(id, cylinder, id - 1);
    if (request == NULL || !DiskScheduler_addRequest(scheduler, request)) {
        DiskRequest_destroy(request);
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    int n = 0;
    if (argc != 2 || argv[1][0] == '\0') goto usage;
    for (int i = 0; argv[1][i] != '\0'; i++) {
        int digit = argv[1][i] - '0';
        if (digit < 0 || digit > 9 || n > (INT_MAX - digit) / 10) goto usage;
        n = n * 10 + digit;
    }
    if (n == 0) goto usage;

    DiskScheduler *scheduler = DiskScheduler_create(n, 0, 99, 63, DIRECTION_DOWN);
    if (scheduler == NULL) {
        fputs("Erro ao criar o escalonador de disco.\n", stderr);
        return EXIT_FAILURE;
    }
    const int cilindros[] = {33, 72, 47, 8, 99, 74, 52, 75};
    int next_id = 1;
    for (int i = 0; i < 8; i++) {
        if (!submit(scheduler, next_id++, cilindros[i])) goto error;
    }
    
    printf("SCAN N-fases: N=%d; disco=0..99; cabeca=63; direcao=descendo\n", n);
    puts("Chegadas discretas: 37 apos atender 47 e 80 apos atender 72; aguardam o proximo lote.");
    int batch = 0, serviced = 0;
    for (;;) {
        if (FIFO_Empty(scheduler->lote_ativo)) {
            int count = DiskScheduler_formBatch(scheduler);
            if (count == 0) break;
            printf("Lote %d: %d pedidos; direcao=%s; chegada:",
                   ++batch, count, direction(scheduler->direcao));
            for (const Node *node = scheduler->lote_ativo->begin; node; node = node->next)
                printf(" %d", node->requisicao->cilindro);
            putchar('\n');
        }
        DiskEvent event;
        DiskStep step = DiskScheduler_step(scheduler, &event);
        if (step == DISK_STEP_ERROR) goto error;
        printf("  Lote %d; direcao=%s; acumulado=%d; ",
               batch, direction(event.direcao), event.acumulado);
        if (step == DISK_STEP_MOVE) {
            printf("movimento %d -> %d; deslocamento=%d%s\n",
                   event.origem, event.destino, event.deslocamento,
                   event.destino == 0 || event.destino == 99 ? " (batente)" : "");
        } else if (step == DISK_STEP_REVERSE) {
            printf("inversao no batente %d\n", event.destino);
        } else {
            int cylinder = event.requisicao->cilindro;
            printf("atendimento id=%d cilindro=%d\n", event.requisicao->id, cylinder);
            DiskRequest_destroy(event.requisicao);
            serviced++;
            if (cylinder == 47 || cylinder == 72) {
                int arrival = cylinder == 47 ? 37 : 80;
                if (!submit(scheduler, next_id++, arrival)) goto error;
                printf("  Chegada cilindro=%d para pendentes\n", arrival);
            }
        }
    }
    printf("Fim: lotes=%d; atendimentos=%d; deslocamento total=%d\n",
           batch, serviced, scheduler->deslocamento_total);
    DiskScheduler_destroy(scheduler);
    return EXIT_SUCCESS;

error:
    fputs("Erro durante a simulacao de disco.\n", stderr);
    DiskScheduler_destroy(scheduler);
    return EXIT_FAILURE;

usage:
    fprintf(stderr, "Uso: %s N (inteiro de 1 a %d)\n", argv[0], INT_MAX);
    return EXIT_FAILURE;
}
