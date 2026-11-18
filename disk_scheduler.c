#include "disk_scheduler.h"

#include <stdlib.h>
#include <limits.h>

DiskScheduler *DiskScheduler_create(int n, int cilindro_min, int cilindro_max,
                                   int cabeca, DiskDirection direcao)
{
    if (n <= 0 || cilindro_min < 0 || cilindro_max < cilindro_min ||
        cabeca < cilindro_min || cabeca > cilindro_max ||
        (direcao != DIRECTION_DOWN && direcao != DIRECTION_UP)) return NULL;

    DiskScheduler *scheduler = malloc(sizeof(*scheduler));
    if (scheduler == NULL) return NULL;
    *scheduler = (DiskScheduler){
        .n = n, .cilindro_min = cilindro_min, .cilindro_max = cilindro_max,
        .cabeca = cabeca, .direcao = direcao
    };
    scheduler->pendentes = FIFO_creat();
    if (scheduler->pendentes == NULL) {
        DiskScheduler_destroy(scheduler);
        return NULL;
    }
    scheduler->lote_ativo = FIFO_creat();
    if (scheduler->lote_ativo == NULL) {
        DiskScheduler_destroy(scheduler);
        return NULL;
    }
    return scheduler;
}

void DiskScheduler_destroy(DiskScheduler *scheduler)
{
    if (scheduler == NULL) return;
    FIFO_Destroy(scheduler->pendentes);
    FIFO_Destroy(scheduler->lote_ativo);
    free(scheduler);
}

bool DiskScheduler_addRequest(DiskScheduler *scheduler, DiskRequest *requisicao)
{
    if (scheduler == NULL || requisicao == NULL ||
        requisicao->cilindro < scheduler->cilindro_min ||
        requisicao->cilindro > scheduler->cilindro_max) return false;
    for (const Node *node = scheduler->lote_ativo->begin;
         node != NULL; node = node->next) {
        if (node->requisicao == requisicao) return false;
    }
    return FIFO_Insert(scheduler->pendentes, requisicao);
}

size_t DiskScheduler_formBatch(DiskScheduler *scheduler)
{
    if (scheduler == NULL) return 0;
    return FIFO_MovePrefix(scheduler->pendentes, scheduler->lote_ativo,
                           (size_t)scheduler->n);
}

void DiskScheduler_print(const DiskScheduler *scheduler)
{
    if (scheduler != NULL) FIFO_Print(scheduler->pendentes);
}

DiskStep DiskScheduler_step(DiskScheduler *scheduler, DiskEvent *evento)
{
    if (evento == NULL) return DISK_STEP_ERROR;
    *evento = (DiskEvent){.tipo = DISK_STEP_ERROR};
    if (scheduler == NULL) return DISK_STEP_ERROR;
    *evento = (DiskEvent){
        .tipo = DISK_STEP_IDLE, .origem = scheduler->cabeca,
        .destino = scheduler->cabeca, .direcao = scheduler->direcao,
        .acumulado = scheduler->deslocamento_total
    };
    if (FIFO_Empty(scheduler->lote_ativo)) DiskScheduler_formBatch(scheduler);
    if (FIFO_Empty(scheduler->lote_ativo)) return evento->tipo;

    DiskRequest *proximo = NULL;
    for (const Node *node = scheduler->lote_ativo->begin;
         node != NULL; node = node->next) {
        DiskRequest *r = node->requisicao;
        // Filtro que descarta a possibilidade de ciclindros fora da direção atual
        if ((scheduler->direcao == DIRECTION_UP && r->cilindro < scheduler->cabeca) ||
            (scheduler->direcao == DIRECTION_DOWN && r->cilindro > scheduler->cabeca))
            continue;
            // Filtro que pretente escolher o menor  entre os canditados na direção UP
            // Filtro para direção DOW que escolhe o menor canditado para a opsição Down
        if (proximo == NULL ||
            (scheduler->direcao == DIRECTION_UP && r->cilindro < proximo->cilindro) ||
            (scheduler->direcao == DIRECTION_DOWN && r->cilindro > proximo->cilindro) ||
            (r->cilindro == proximo->cilindro &&
             r->sequencia_chegada < proximo->sequencia_chegada)) proximo = r;
    }
    if (proximo != NULL && proximo->cilindro == scheduler->cabeca) {
        evento->requisicao = FIFO_Remove(scheduler->lote_ativo, proximo);
        evento->tipo = DISK_STEP_SERVICE;
        return evento->tipo;
    }
    int destino = proximo != NULL ? proximo->cilindro :
        (scheduler->direcao == DIRECTION_UP ? scheduler->cilindro_max : scheduler->cilindro_min);
    if (destino == scheduler->cabeca) {
        if (scheduler->direcao == DIRECTION_UP) {
            scheduler->direcao = DIRECTION_DOWN;
        } else {
            scheduler->direcao = DIRECTION_UP;
        }
        evento->direcao = scheduler->direcao;
        evento->tipo = DISK_STEP_REVERSE;
        return evento->tipo;
    }
    
    int distancia = destino > scheduler->cabeca ?
        destino - scheduler->cabeca :
        scheduler->cabeca - destino;
    if (distancia > INT_MAX - scheduler->deslocamento_total) {
        evento->tipo = DISK_STEP_ERROR;
        return evento->tipo;
    }
    scheduler->cabeca = destino;
    scheduler->deslocamento_total += distancia;
    evento->tipo = DISK_STEP_MOVE;
    evento->destino = destino;
    evento->deslocamento = distancia;
    evento->acumulado = scheduler->deslocamento_total;
    return evento->tipo;
}
