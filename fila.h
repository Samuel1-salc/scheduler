#ifndef LINKED_CIRC_LIST_H
#define LINKED_CIRC_LIST_H
#include <stdbool.h>
#include <stddef.h>
#include "disk_request.h"

typedef struct Node {
    DiskRequest *requisicao;
    struct Node *next;
} Node;

/* Lista simplesmente encadeada NÃO circular: o último next é NULL.
 * O nome FIFO foi mantido por compatibilidade com a API existente. */
typedef struct FIFO {
    Node *begin;
    size_t size;
} FIFO;

FIFO *FIFO_creat(void);
/* Assume propriedade somente no sucesso. Pedido não pode pertencer a outra fila.
 * Rejeita NULL e o mesmo ponteiro já inserido nesta fila. */
bool FIFO_Insert(FIFO *list, DiskRequest *requisicao);
/* Retiram por identidade e transferem propriedade ao chamador; NULL se ausente. */
DiskRequest *FIFO_Remove(FIFO *list, DiskRequest *requisicao);
DiskRequest *FIFO_PopFront(FIFO *list);
bool FIFO_Empty(const FIFO *list);
size_t FIFO_Size(const FIFO *list);
void FIFO_Print(const FIFO *list);
/* Libera nós, pedidos restantes e a própria fila. Aceita NULL. */
void FIFO_Destroy(FIFO *list);
/**
 * @brief Transfere um prefixo FIFO para uma fila vazia, sem alocar memória.
 * @param origem Fila proprietária dos pedidos a transferir.
 * @param destino Fila vazia que receberá os nós e a propriedade dos pedidos.
 * @param limite Quantidade máxima positiva de pedidos.
 * @return Número transferido; zero para argumentos inválidos ou origem vazia.
 * @note Em caso de recusa, nenhuma fila é alterada.
 */
size_t FIFO_MovePrefix(FIFO *origem, FIFO *destino,
                              size_t limite);
#endif
