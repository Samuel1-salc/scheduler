#include "fila.h"
#include <stdio.h>
#include <stdlib.h>

FIFO *FIFO_creat(void)
{
    FIFO *list = malloc(sizeof(*list));
    if (list != NULL) {
        list->begin = NULL;
        list->size = 0;
    }
    return list;
}

bool FIFO_Insert(FIFO *list, DiskRequest *requisicao)
{
    if (list == NULL || requisicao == NULL) return false;
    Node *last = list->begin;
    while (last != NULL) {
        if (last->requisicao == requisicao) return false;
        if (last->next == NULL) break;
        last = last->next;
    }
    Node *node = malloc(sizeof(*node));
    if (node == NULL) return false;
    node->requisicao = requisicao;
    node->next = NULL;
    if (last == NULL) {
        list->begin = node;
    } else {
        last->next = node;
    }
    list->size++;
    return true;
}

DiskRequest *FIFO_Remove(FIFO *list, DiskRequest *requisicao)
{
    if (FIFO_Empty(list) || requisicao == NULL) return NULL;
    Node *prev = NULL;
    Node *node = list->begin;
    while (node != NULL) {
        if (node->requisicao == requisicao) {
            if (prev == NULL) list->begin = node->next;
            else prev->next = node->next;
            free(node);
            list->size--;
            return requisicao;
        }
        prev = node;
        node = node->next;
    }
    return NULL;
}

DiskRequest *FIFO_PopFront(FIFO *list)
{
    return FIFO_Empty(list) ? NULL :
        FIFO_Remove(list, list->begin->requisicao);
}

bool FIFO_Empty(const FIFO *list)
{
    return list == NULL || list->begin == NULL;
}

size_t FIFO_Size(const FIFO *list)
{
    return list == NULL ? 0 : list->size;
}

void FIFO_Print(const FIFO *list)
{
    if (FIFO_Empty(list)) {
        puts("Lista vazia.");
        return;
    }
    const Node *node = list->begin;
    while (node != NULL) {
        const DiskRequest *pedido = node->requisicao;
        printf("ID: %d | Cilindro: %d | Chegada: %zu\n",
               pedido->id, pedido->cilindro, pedido->sequencia_chegada);
        node = node->next;
    }
}

void FIFO_Destroy(FIFO *list)
{
    if (list == NULL) return;
    Node *node = list->begin;
    while (node != NULL) {
        Node *next = node->next;
        DiskRequest_destroy(node->requisicao);
        free(node);
        node = next;
    }
    free(list);
}

size_t FIFO_MovePrefix(FIFO *origem, FIFO *destino,
                              size_t limite)
{
    if (origem == NULL || destino == NULL || origem == destino || limite == 0 ||
        FIFO_Empty(origem) || !FIFO_Empty(destino)) return 0;
    size_t quantidade = origem->size < limite ? origem->size : limite;
    Node *primeiro = origem->begin;
    Node *ultimo = primeiro;
    for (size_t i = 1; i < quantidade; i++) ultimo = ultimo->next;
    origem->begin = ultimo->next;
    ultimo->next = NULL;
    origem->size -= quantidade;
    destino->begin = primeiro;
    destino->size = quantidade;
    return quantidade;
}
