#include "disk_request.h"
#include <stdio.h>
#include <stdlib.h>

DiskRequest *DiskRequest_create(int id, int cilindro, size_t sequencia_chegada)
{
    if (cilindro < 0) return NULL;
    DiskRequest *pedido = malloc(sizeof(*pedido));
    if (pedido != NULL) {
        pedido->id = id;
        pedido->cilindro = cilindro; // posição em que o pedido quer acessar atualmente
        pedido->sequencia_chegada = sequencia_chegada;
    }
    return pedido;
}

void DiskRequest_destroy(DiskRequest *requisicao)
{
    free(requisicao);
}

void REQUESTS_print(const DiskRequest *pedido)
{
    if (pedido == NULL) return;

    printf(
        "ID: %d | Cilindro: %d | Sequencia: %zu\n",
        pedido->id,
        pedido->cilindro,
        pedido->sequencia_chegada
    );
}