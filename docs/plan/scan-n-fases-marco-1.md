# SCAN N-fases — primeiras implementações da atividade 2

**Objetivo:** Entregar um núcleo C de simulação de disco com N configurável, fila de chegada, lote congelado e varredura bidirecional demonstrável. Este documento planeja o primeiro marco; não registra implementação concluída.

**Fontes:** Pedido e análise da conversa; enunciado `/home/samuel/Segunda_Atividade_Prática-bbb77248253049e081073f59aae68d9b.pdf`, página única e Figura 1; fontes locais `main.c`, `pcb.[ch]`, `fila.[ch]`, `scheduler.[ch]` e `Makefile`. Caminhos de código neste documento são relativos à raiz do projeto.

**Estado inicial (antes da etapa 2):** Branch `trabalho-2`. O programa implementa quatro filas de prioridade fixa, com Round Robin e quantums 2, 4, 6 e 8. Não há cilindros, posição da cabeça, lotes congelados nem chegadas durante execução. A lista circular permite aproveitar os algoritmos de encadeamento; sua circularidade não implementa SCAN. `CircularList_creat` e `creatNode` acessam alocações sem verificar NULL; a inserção não informa falha ao chamador. `README.md` já existe sem rastreamento e não será sobrescrito por este plano.

**Requisitos do enunciado:** C/C++; N definido pelo usuário; atendimento nos dois sentidos; critério semelhante à varredura congelada; primeiras N requisições; N=1 com ordem FCFS; N alto com comportamento SCAN. A figura exige deslocamento aos batentes, mesmo sem pedido no extremo.

**Interpretação adotada na etapa 1:** Selecionar por chegada até N pedidos para cada lote; atender somente esse lote em ordem espacial, preservando a direção ao trocar lotes. Novas chegadas aguardam o próximo lote. O texto diz “em cada sentido”, mas a primeira fase da figura atravessa uma inversão. A etapa 1 resolveu essa diferença com base nos grupos da figura; as regras e convenções estão em `scan-n-fases-contrato.md`.

**Arquitetura atualizada pelo usuário na etapa 2:** Adaptar diretamente `fila.[ch]` para `DiskRequest`, com modelo em `disk_request.[ch]` e consumidores em `scheduler.[ch]` e `main.c`. Não criar `disk_queue`. O alvo `scheduler` passa a demonstrar pendentes; a política de CPU deixa de integrar o executável. `pcb.[ch]` permanece como fonte histórica sem ligação ao alvo. A varredura será acrescentada nas próximas etapas.

**Fora deste marco:** Interface gráfica, concorrência real, acesso a dispositivo físico, simulação de latência rotacional, métricas de tempo sem modelo definido e entrega acadêmica final. Chegadas serão injetadas por eventos determinísticos entre atendimentos, sem threads nem relógio real.

<a id="step-1"></a>
## Step 1 — Definir o contrato das fases e dos limites

**Intenção:** Resolver a semântica necessária à varredura a partir do enunciado e da figura. Produzir uma decisão explícita, distinguindo evidência de convenções propostas.
**Tags:** design
**Dependências:** Nenhuma.
**Escopo:** Registrar contrato em `docs/plan/scan-n-fases-contrato.md`: N por lote ou por sentido, instante de troca do lote, continuidade da direção, inversões nos batentes, atendimento no cilindro atual e parada após o último pedido. Propor desempate por chegada para pedidos no mesmo cilindro. Separar equivalência de ordem FCFS de equivalência de deslocamento.
**Acceptance:**
- O contrato explica como o primeiro lote 33,72,47, partindo de 63 para baixo com N=3, produz atendimento 47,33,72; registra a passagem pelo limite inferior.
- Ficam explícitos os limites configuráveis, a regra de parada e os eventos de chegada; a comparação com FCFS para N=1 usa ordem de atendimento.
- A ambiguidade “em cada sentido” versus lote é resolvida com evidência ou apresentada para decisão do usuário; sem resolução, etapa 4 permanece pendente.

**Validação:** Traçar manualmente os dois primeiros lotes da figura e documentar a decisão. Não inferir datas ou critérios adicionais do professor.
**Out of scope:** Implementação da varredura.

<a id="step-2"></a>
## Step 2 — Implementar requisições e fila de disco

**Intenção:** Criar o modelo de requisição e a fila FIFO tipada, reaproveitando os casos de inserção e remoção da lista existente. As novas rotinas devem tratar falhas de alocação desde sua criação.
**Tags:** change
**Dependências:** Nenhuma; pode avançar enquanto a semântica da varredura é discutida.
**Escopo:** `disk_request.[ch]` e `fila.[ch]`. Requisição com ID, cilindro e sequência de chegada. Operações de criar, inserir no fim, retirar do início, retirar pedido específico, consultar tamanho e destruir. Documentar propriedade: a fila assume o pedido apenas após inserção bem-sucedida; retirar transfere a propriedade ao chamador. Não generalizar a lista de PCB com void*.
**Acceptance:**
- Inserir três pedidos e retirá-los preserva chegada; remover primeiro, intermediário e último mantém tamanho e encadeamento corretos, inclusive em fila unitária e vazia.
- Falha de alocação retorna erro sem acessar NULL, alterar a fila ou liberar um pedido ainda pertencente ao chamador.
- Retirada não destrói o pedido; destruição da fila libera seus pedidos restantes uma única vez, inclusive para cilindros repetidos com IDs distintos.

**Validação:** Casos focados de operações e falha de alocação com mecanismo controlado de teste; verificar memória com sanitizadores quando disponíveis.
**Out of scope:** Implementação da varredura.

<a id="step-3"></a>
## Step 3 — Implementar configuração e lotes congelados

**Implementação:** `disk_scheduler.[ch]`, com API `DiskScheduler_*`, utiliza a FIFO de `fila.[ch]`. `main.c` e o Makefile consomem o novo módulo; `scheduler.[ch]` não integra o alvo. Lotes são transferidos por `FIFO_MovePrefix`, sem alocação.

**Intenção:** Criar o estado do escalonador de disco e separar pedidos pendentes dos pedidos já selecionados para atendimento. Esta entrega não decide ainda a ordem espacial.
**Tags:** impl
**Dependências:** Step 2.
**Escopo:** `disk_scheduler.[ch]` com N, limites inclusivos, cabeça, direção, pendentes e lote ativo. API para submeter pedidos e formar lote com até N entradas FIFO. Transferir a propriedade sem perda ou duplicação; recusar substituição de lote não vazio. Validar N positivo, intervalo de cilindros e posição inicial dentro do intervalo.
**Acceptance:**
- Para fila 33,72,47,8 e N=3, o lote contém 33,72,47 e pendentes contém 8; uma chegada 37 altera apenas pendentes, para 8,37.
- N=1 seleciona um pedido; N maior que a fila seleciona todos; fila vazia não forma lote e configuração ou pedido inválido retorna erro sem mutação parcial.
- Lote ativo não vazio permanece congelado; após esvaziá-lo, o próximo lote recebe os pedidos pendentes na ordem de chegada, com cada pedido pertencendo a um único conjunto.

**Validação:** Exercitar a API com sequência fixa e chegada entre formações, incluindo lote final incompleto e descarte completo do estado.
**Out of scope:** Movimento da cabeça e interface de linha de comando.

<a id="step-4"></a>
## Step 4 — Implementar a varredura SCAN sobre o lote ativo

**Implementação:** `DiskScheduler_step` retorna um evento por chamada (movimento, inversão, atendimento ou ociosidade), forma lote no início quando necessário e transfere ao chamador a propriedade do pedido atendido. Acumulador `int` com detecção de overflow. Casos determinísticos em `tests/test_scheduler.c` validam os percursos de 135 e 341, congelamento, N=1, duplicatas, extremos e parada.

**Intenção:** Acrescentar a seleção espacial e a movimentação da cabeça seguindo o contrato resolvido na etapa 1. Oferecer execução incremental para permitir chegadas entre atendimentos.
**Tags:** impl
**Dependências:** Steps 1 e 3.
**Contrato:** Resolvido em `scan-n-fases-contrato.md`, incluindo limites de fase e parada. A implementação da etapa 3 continua sendo condição de início; o roteador não impõe essa dependência automaticamente.
**Escopo:** Motor em `disk_scheduler.[ch]` que atende o próximo pedido elegível na direção atual, vai ao batente para inverter quando necessário e registra movimentos separadamente de atendimentos. Requisições no mesmo cilindro permanecem distintas; deslocamento acumulado inclui movimentos sem atendimento. Adotar tipo numérico que comporte a soma e evitar overflow na subtração de posições.
**Acceptance:**
- No primeiro lote da figura, cabeça 63 e direção decrescente produzem 47,33,72; para limites 0..99, o percurso 63→47→33→0→72 acumula 135 até atender 72.
- N=1 mantém a ordem de chegada; todos os pedidos em um lote são atendidos antes dos pendentes, inclusive os que chegam no cilindro da cabeça durante a fase.
- Casos de mesmo cilindro, uma direção sem pedidos, cabeça no batente e entrada vazia terminam sem duplicação ou laço infinito; as inversões e a parada seguem o contrato registrado.

**Validação:** Conferir eventos de movimento e atendimento em cenários determinísticos; comparar N grande com uma varredura SCAN manual da mesma entrada estática, sob as mesmas regras de limites e parada.
**Out of scope:** Interface gráfica e modelo de tempo físico.

<a id="step-5"></a>
## Step 5 — Implementar demonstração configurável do primeiro marco

**Intenção:** Expor o núcleo em um programa executável com N configurável e rastreamento suficiente para comparar o comportamento ao enunciado.
**Tags:** impl
**Dependências:** Step 4.
**Escopo:** `main.c` e alvo existente `scheduler` no `Makefile`. Receber N por argumento e documentar o uso no próprio programa. Usar cenário da figura: limites 0..99, cabeça 63, direção decrescente e entrada 33,72,47,8,99,74,52,75. Injetar 37 após atendimento de 47 e 80 após atendimento de 72, antes de formar o lote seguinte. Informar que isso modela discretamente as chegadas descritas como “durante” na figura.
**Acceptance:**
- Com N=3, o cenário reproduz os grupos da figura 47,33,72 / 74,99,8 / 37,52,75 / 80 sob o contrato decidido; divergência exige revisar o contrato, não alterar silenciosamente a referência.
- A saída identifica lote, pedido, cilindro, direção, movimentos aos limites e deslocamento acumulado; N inválido ou ausente apresenta uso e termina com erro.
- O alvo `scheduler` compila com os avisos existentes e executa o cenário. Exercitar N=1 e N maior que a quantidade inicial, respeitando que chegadas posteriores não entram em lote já congelado.

**Validação:** Executar o alvo e conferir ordem, quantidade de atendimentos e movimentos com a referência manual. Os casos de aceitação acompanham a funcionalidade; não constituem uma etapa isolada de cobertura.
**Out of scope:** Leitura de arquivos de carga e entrega acadêmica final.

---

**Compatibilidade com plan-orchestrate:** Avaliada contra `/home/samuel/.codex/skills/plan-orchestrate/SKILL.md`. As tags refletem a intenção principal; testes são critérios das funcionalidades, não entregas separadas. Os comandos abaixo são mapeamentos do catálogo, não comandos executados nem confirmação de disponibilidade no cliente.

| ID | Tag principal | Comando previsto | Transporte de escopo | Prontidão |
|---|---|---|---|---|
| 1 | design | `/plan` | Roteável | Contrato registrado; etapa concluída |
| 2 | change | `/orch-change-feature` | Roteável | Implementada e validada |
| 3 | impl | `/orch-add-feature` | Roteável | Implementada e validada |
| 4 | impl | `/orch-add-feature` | Roteável | Implementada e validada |
| 5 | impl | `/orch-add-feature` | Roteável | Depende de 4 |

**Sequenciamento:** A ordem numérica é o caminho principal. Etapas 2 e 3 não dependem da escolha espacial da etapa 1. Contrato da etapa 1 registrado em `scan-n-fases-contrato.md`; executar 4 somente após 3, e 5 somente após 4. Nenhuma etapa exige um comando sem transporte de escopo; prontidão técnica é distinta dessa compatibilidade.
