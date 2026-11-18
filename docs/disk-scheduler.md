# Demonstração SCAN N-fases

Compile os dois executáveis com `make`. Execute `./scheduler` para CPU ou
`./disk-scheduler N` para disco; por exemplo, `./disk-scheduler 3`.
N é um inteiro decimal positivo até INT_MAX. Argumento ausente, inválido ou
excedente imprime uso em stderr e termina com erro.

O disco usa limites 0..99, cabeça 63 descendo e fila inicial
33,72,47,8,99,74,52,75. As chegadas são eventos discretos: 37 após atender
47 e 80 após atender 72. São inseridas antes da formação do próximo lote,
sem entrar no lote congelado atual. A direção continua entre lotes.

Com N=3, os atendimentos são 47,33,72 / 74,99,8 / 37,52,75 / 80,
com deslocamento total 341. Com N=1, a ordem é FCFS (incluindo a ordem
efetiva das chegadas), com deslocamento 620. Com N=100, o primeiro lote
contém os oito pedidos iniciais e o segundo contém as duas chegadas;
o deslocamento total é 224.

A saída registra lotes, IDs, cilindros, direção, movimentos, batentes,
inversões e deslocamento acumulado. `make test` verifica o núcleo e a CLI
(Python 3), incluindo entradas inválidas e execução da CPU.

Para preservar a CPU após a conversão anterior dos arquivos da raiz para
disco, `cpu/` contém os fontes históricos de HEAD, sem mudanças de política.
O alvo `scheduler` usa esses fontes e `pcb.c`; `disk-scheduler` usa
`disk_main.c`, `disk_scheduler.c`, `disk_request.c` e `fila.c`.
O antigo `main.c` da raiz permanece preservado, fora dos dois alvos.
