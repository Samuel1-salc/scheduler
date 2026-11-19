# Trabalho 2 — Escalonador de disco

Simulador em C do algoritmo **SCAN N-fases**, que organiza as requisições de acesso ao disco em lotes de até **N pedidos**.

## Funcionamento

As requisições chegam a uma fila de pendentes em ordem FIFO. Quando o lote ativo fica vazio, os primeiros N pedidos (ou os restantes, se houver menos) formam o próximo lote. Novas requisições aguardam nos pendentes, sem alterar o lote em processamento.

Dentro de cada lote, a cabeça atende os cilindros na direção atual. Quando ainda há pedidos, mas nenhum nessa direção, ela avança até o limite do disco e inverte o sentido. A posição e a direção são preservadas entre lotes, e o simulador acumula a distância percorrida.

## Estrutura

- `disk_request.c` e `.h`: representam cada requisição por ID, cilindro e sequência de chegada.
- `fila.c` e `.h`: implementam as filas de pendentes e do lote ativo usando listas simplesmente encadeadas.
- `disk_scheduler.c` e `.h`: controlam os lotes, a posição da cabeça, a direção e os eventos de movimento, inversão e atendimento.
- `disk_main.c`: configura e executa a simulação, exibindo os eventos e o deslocamento total.
- `Makefile`: define a compilação do executável `disk-scheduler`.

O exemplo utiliza cilindros de **0 a 99**, cabeça inicialmente em **63**, sentido decrescente e pedidos em **33, 72, 47, 8, 99, 74, 52 e 75**. Durante a execução, chegam pedidos para **37** após o atendimento de **47** e para **80** após o de **72**.

## Como rodar

Com **GCC** e **Make** instalados, execute na raiz do projeto. O argumento `4` define o limite de pedidos por lote e pode ser substituído por outro inteiro positivo:

```bash
make disk-scheduler && ./disk-scheduler 4
```
