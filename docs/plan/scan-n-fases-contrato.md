# Contrato SCAN N-fases — atividade 2

**Origem:** Etapa 1 de `scan-n-fases-marco-1.md`; enunciado e Figura 1 de `/home/samuel/Segunda_Atividade_Prática-bbb77248253049e081073f59aae68d9b.pdf`, analisados nesta conversa.

**Status:** Contrato definido para o primeiro marco. A interpretação de N por lote é sustentada pelos grupos da figura; regras não especificadas pelo enunciado estão identificadas como convenções de simulação. Não constitui confirmação do professor nem implementação do algoritmo.

## Decisão sobre N

N limita a quantidade de requisições selecionadas por ordem de chegada para um lote congelado. Não é uma cota independente renovada a cada inversão de direção. Um lote pode atravessar uma inversão; termina quando seus pedidos forem atendidos.

Evidência: a figura apresenta N=3 e f(1)={47,33,72}, partindo de 63 em direção decrescente. Os pedidos 47 e 33 são atendidos antes da inversão, e 72 depois. São três pedidos no mesmo grupo, nos dois sentidos. O grupo seguinte é f(2)={74,99,8}, novamente atravessando uma inversão. Interpretar “em cada sentido” como formação de um novo lote em toda inversão não reproduz esses grupos.

Assim, para este projeto, “em cada sentido” indica que o atendimento ocorre nos dois sentidos; o limite N se aplica ao grupo selecionado. Essa é uma resolução operacional baseada no exemplo fornecido, não uma afirmação de que o texto não admite outra leitura. Caso uma orientação posterior do professor contradiga a figura, revisar este contrato antes de alterar o motor.

## Configuração e estado

- N é inteiro positivo e permanece fixo durante uma execução. Alterá-lo requer uma nova simulação neste marco.
- Limites de cilindro são inteiros inclusivos, com 0 <= mínimo <= máximo. Cabeça inicial e pedidos devem estar nesse intervalo. O caso mínimo=máximo é válido e não exige movimento.
- Direção inicial é crescente ou decrescente; não é inferida do primeiro pedido.
- Cada pedido tem identidade própria, cilindro e sequência de chegada. Pedidos no mesmo cilindro não são fundidos.
- Estado mantém pendentes em FIFO, lote ativo, cabeça, direção e deslocamento acumulado. Cada pedido pertence a exatamente um conjunto: pendente, ativo ou atendido.

Limites, direção inicial e N são parâmetros do núcleo. A demonstração usa 0..99, cabeça 63, direção decrescente e N informado pelo usuário. A validação numérica e a identidade dos pedidos são convenções de implementação.

## Formação e congelamento

Quando não houver lote ativo, retirar os primeiros min(N, quantidade de pendentes) pedidos da FIFO e formar o próximo lote. Não ordenar a FIFO inteira antes dessa seleção. Não aguardar completar N quando houver menos pedidos disponíveis.

Durante o processamento, a composição do lote só diminui por atendimento. Toda chegada entra no fim dos pendentes, mesmo se seu cilindro estiver no caminho da cabeça ou for igual à posição atual. Não repor vagas do lote com pedidos novos.

Ao terminar o lote, processar as chegadas associadas ao último atendimento e só então formar o próximo. Preservar cabeça e direção entre lotes. Não ir ao batente apenas porque o lote acabou; o próximo lote é selecionado imediatamente, na posição atual. A continuidade da direção é consistente com a passagem de 72 para 74 no exemplo.

## Seleção espacial e batentes

Dentro do lote ativo:

- Atender primeiro pedidos no cilindro atual, sem deslocamento e sem inverter direção; desempatar pela sequência de chegada.
- Na direção crescente, escolher o menor cilindro maior que a cabeça. Na decrescente, escolher o maior cilindro menor que a cabeça. Desempatar também por chegada.
- Se restarem pedidos ativos, mas nenhum na direção atual, mover até o batente correspondente e inverter. O batente é obrigatório mesmo sem pedido nele; não inverter no último pedido intermediário como faria uma política LOOK.
- Se a cabeça já estiver no batente e precisar inverter, inverter sem deslocamento adicional. Um pedido no próprio batente é atendido antes de decidir a próxima inversão.

Movimentos e atendimentos são eventos distintos. Somar |destino-origem| em cada movimento, incluindo trechos sem atendimento; atendimento na posição atual soma zero. Usar aritmética que evite overflow tanto na diferença quanto na soma, retornando erro se a capacidade for excedida.

## Chegadas, ociosidade e parada

Convenção do primeiro marco: execução discreta, sem relógio físico ou threads. O chamador pode submeter pedidos entre passos. Chegadas descritas na figura como ocorrendo “durante” um atendimento são injetadas logo após esse atendimento e antes de eventual troca de lote. Uma chamada de atendimento não deve formar automaticamente o lote seguinte antes dessa oportunidade.

No cenário de referência, injetar 37 depois do atendimento de 47 e 80 depois do atendimento de 72, uma única vez cada. A sequência de chegada inicial é 33,72,47,8,99,74,52,75; os dois pedidos novos entram depois dela.

Convenção de parada: quando lote e pendentes estiverem vazios após a injeção dos eventos correspondentes, a execução finita termina no último cilindro atendido. Não acrescentar uma viagem final sem trabalho até o batente. O enunciado não esclarece esse término; a regra dos batentes aplica-se às inversões necessárias para atender pedidos restantes.

Se uma aplicação futura mantiver entradas abertas, o mesmo estado vazio significa ociosidade: conservar posição e direção, sem girar em laço. Essa aplicação decidirá quando encerrar a entrada. O marco atual não inclui agenda de chegadas temporizadas.

## Traço de referência

N=3, disco 0..99, cabeça 63, direção decrescente. As colunas de distância incluem todas as viagens necessárias dentro de cada lote, sem viagem final após esvaziar o trabalho.

| Lote | Seleção FIFO | Ordem atendida | Percurso da cabeça | Distância | Acumulado |
|---|---|---|---|---:|---:|
| 1 | 33,72,47 | 47,33,72 | 63→47→33→0→72 | 135 | 135 |
| 2 | 8,99,74 | 74,99,8 | 72→74→99→8 | 118 | 253 |
| 3 | 52,75,37 | 37,52,75 | 8→0→37→52→75 | 83 | 336 |
| 4 | 80 | 80 | 75→80 | 5 | 341 |

No primeiro lote, 16+14+33+72=135. A chegada 37 após 47 não interrompe o congelamento. Depois de 72 e da chegada 80, pendentes são 8,99,74,52,75,37,80.

No segundo lote, preservar a direção crescente produz 74 e 99; como 99 já é o batente superior, inverter ali permite atender 8. Distância: 2+25+91=118. Ao encerrar, pendentes são 52,75,37,80 e a direção é decrescente.

O terceiro lote começa em 8 descendo, sem pedido elegível nesse sentido; deve visitar 0 antes de subir. Esse deslocamento adicional diferencia SCAN de LOOK. O total 341 depende da convenção de parada escolhida e não é uma métrica explicitamente fornecida pelo enunciado.

## Casos de aceitação do contrato

- N=1: cada lote contém o próximo pedido por chegada; a ordem de atendimento é FCFS, inclusive com novas chegadas. O percurso pode ir aos batentes, logo não se exige igualdade de deslocamento com FCFS direto. Exemplo: 63 descendo e pedido único 72 percorre 63→0→72.
- N maior ou igual à quantidade inicial: uma carga estática inteira forma um lote e segue SCAN. Em carga dinâmica, pedidos posteriores continuam congelados fora do lote; não se afirma equivalência com SCAN que admite chegadas durante a varredura.
- Entrada vazia: zero atendimentos e zero movimento. Lote incompleto é processado sem espera. Pedidos repetidos são atendidos individualmente por chegada, sem deslocamento entre cilindros iguais.
- Cabeça no batente, intervalo de um único cilindro e ausência de pedidos no sentido atual não podem causar inversões infinitas.

## Efeito sobre o plano

A decisão N por lote está resolvida para o primeiro marco com base na figura. A etapa 4 deixa de depender de esclarecimento semântico e continua dependendo da implementação da etapa 3. Regras de parada, desempate e chegadas discretas são convenções explícitas deste contrato. Nenhuma varredura foi implementada nesta etapa.
