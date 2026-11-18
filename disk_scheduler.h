#ifndef DISK_SCHEDULER_H
#define DISK_SCHEDULER_H
#include "fila.h"

typedef enum {
    DIRECTION_DOWN = -1,
    DIRECTION_UP = 1
} DiskDirection;

typedef enum {
    DISK_STEP_ERROR = -1,
    DISK_STEP_IDLE = 0,
    DISK_STEP_MOVE,
    DISK_STEP_REVERSE,
    DISK_STEP_SERVICE
} DiskStep;

typedef struct DiskEvent {
    DiskStep tipo;
    int origem;
    int destino;
    DiskDirection direcao; /* Direção após o evento. */
    int deslocamento;
    int acumulado;
    DiskRequest *requisicao; /* Apenas SERVICE: propriedade do chamador. */
} DiskEvent;

/* Configuração fixa após criação; filas devem ser geridas pelas operações da API. */
typedef struct DiskScheduler {
    int n;
    int cilindro_min;
    int cilindro_max;
    int cabeca;
    DiskDirection direcao;
    int deslocamento_total;
    FIFO *pendentes;
    FIFO *lote_ativo;
} DiskScheduler;

/**
 * @brief Cria um escalonador com duas filas vazias.
 * @param n Limite positivo de pedidos por lote.
 * @param cilindro_min Menor cilindro permitido, não negativo.
 * @param cilindro_max Maior cilindro permitido, inclusive; >= cilindro_min.
 * @param cabeca Posição inicial dentro dos limites inclusivos.
 * @param direcao DIRECTION_DOWN ou DIRECTION_UP.
 * @return Escalonador pertencente ao chamador ou NULL por configuração inválida
 *         ou falha de alocação. Alocações parciais são liberadas.
 */
DiskScheduler *DiskScheduler_create(int n, int cilindro_min, int cilindro_max,
                            int cabeca, DiskDirection direcao);
/**
 * @brief Libera o escalonador e todos os pedidos ainda nas duas filas.
 * @param scheduler Escalonador a destruir; aceita NULL.
 * @return Nenhum valor (void).
 */
void DiskScheduler_destroy(DiskScheduler *scheduler);
/**
 * @brief Acrescenta um pedido válido ao fim dos pendentes, sem alterar o lote.
 * @param scheduler Escalonador criado por DiskScheduler_create.
 * @param requisicao Pedido do chamador, não pertencente a outra fila.
 * @return true se inserido e a propriedade transferida; false sem mutação
 *         para NULL, cilindro inválido, ponteiro já nas filas ou falta de memória.
 * @note FIFO segue a ordem das chamadas bem-sucedidas, não ordena por cilindro
 *       nem por sequencia_chegada. Não modificar um pedido após transferi-lo.
 */
bool DiskScheduler_addRequest(DiskScheduler *scheduler, DiskRequest *requisicao);
/**
 * @brief Forma lote congelado com os primeiros até N pendentes, sem alocação.
 * @param scheduler Escalonador cujo lote ativo deve estar vazio.
 * @return Quantidade transferida; zero se NULL, lote ocupado ou sem pendentes.
 * @note Não move a cabeça, não troca direção nem repõe vagas de lote não vazio.
 */
size_t DiskScheduler_formBatch(DiskScheduler *scheduler);
/**
 * @brief Executa um evento SCAN, formando lote apenas no início da chamada.
 * @return Tipo do evento; ERROR para argumentos NULL ou overflow (sem movimento).
 * @note MOVE não atende pedidos; REVERSE ocorre no batente, sem movimento.
 * SERVICE retira um pedido e transfere sua propriedade ao chamador, que deve
 * destruí-lo. Não forma o próximo lote nessa chamada. IDLE conserva o estado.
 * Desempate por sequencia_chegada; sequências iguais preservam a ordem FIFO.
 * O evento é sobrescrito a cada chamada; não mantém histórico interno.
 */
DiskStep DiskScheduler_step(DiskScheduler *scheduler, DiskEvent *evento);
/**
 * @brief Exibe os pedidos pendentes na ordem FIFO.
 * @param scheduler Escalonador a consultar; NULL não produz saída.
 * @return Nenhum valor (void).
 */
void DiskScheduler_print(const DiskScheduler *scheduler);
#endif
