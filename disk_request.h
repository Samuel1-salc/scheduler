#ifndef DISK_REQUEST_H
#define DISK_REQUEST_H
#include <stddef.h>

typedef struct DiskRequest {
    int id;
    int cilindro;
    size_t sequencia_chegada;
} DiskRequest;

/**
 * @brief Cria uma requisição de acesso a um cilindro do disco.
 *
 * @param id Identificador da requisição.
 * @param cilindro Cilindro solicitado; deve ser maior ou igual a zero.
 * @param sequencia_chegada Sequência que representa a ordem de chegada.
 * @return Ponteiro para a requisição criada, ou NULL se o cilindro for
 *         negativo ou a alocação de memória falhar.
 *
 * @note O chamador possui a requisição até transferi-la para uma fila.
 *       O limite superior do cilindro não é validado nesta função.
 */
DiskRequest *DiskRequest_create(int id, int cilindro, size_t sequencia_chegada);

/**
 * @brief Libera a memória de uma requisição.
 *
 * @param requisicao Requisição pertencente ao chamador a ser liberada.
 *                   Se for NULL, nenhuma operação será realizada.
 * @return Nenhum valor (void).
 *
 * @note Após a liberação, o ponteiro não deve ser acessado novamente.
 */
void DiskRequest_destroy(DiskRequest *requisicao);
/* Exibe os dados de um pedido; aceita NULL sem produzir saída. */
void REQUESTS_print(const DiskRequest *pedido);
#endif
