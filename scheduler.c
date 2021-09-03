#include <stdio.h>

typedef struct process {
    // PCB
    int pid;
    int status;
    int priority;

    // Infos de execucao
    int time_cpu;
    int arrival;
    int time_io[6]; /* [tempo_disco, inicio_disco, tempo_fita, inicio_fita, tempo_impressora, inicio_impressora] */

    // Ponteiro de fila
    Process* next;
} Process;

// Adicionar processo em um fila (ALEXANDRE)
    // Ex: Processo vai fazer IO de impressora
        // add(p1, printer_queue)
    // Ex: Processo foi preemptado
        // add(p1, low_priority_queue)
    // Ex: Processo novo
        // add(p1, low_priority_queue)
void add(Process* p, Process** queue);

// Retirar o primeiro processo de uma fila (ALEXANDRE)
    // Ex: preempcao
        //  remove(high_priority_queue) 
    // Ex: fim de IO
        //  remove(printer_queue) 
Process* remove(Process** queue);

// Filas
Process** high_priority_queue;
Process** low_priority_queue;
Process** printer_queue;
Process** disk_queue;
Process** magnetic_tape_queue;

// Gerar processos de exemplo (LUCAS)
    // Retornar array de ponteiros de processos
Process** generate_processes();

int main(int argc, char **argv){
    /* TODO: funcao pra criar uma array de processos */
    int t = 0;
    while(1){ // (SASSE)

        /* ---------- INICIO - ADICIONAR NAS FILAS ---------- */

        /* ---------- INICIO - ALTA PRIORIDADE ---------- */
        /* TODO: verifica se ha processos NOVOS no instante de tempo t*/
            /* TODO: adiciona processos NOVOS na fila de ALTA prioridade*/

        /* TODO: verifica se ha processos voltando de FITA MAGNETICA no instante de tempo t*/

            /* TODO: adiciona processos voltando de FITA MAGNETICA na fila de ALTA prioridade*/

        /* TODO: verifica se ha processos voltando de IMPRESSORA no instante de tempo t*/

            /* TODO: adiciona processos voltando de IMPRESSORA na fila de ALTA prioridade*/

        /* ---------- FIM - ALTA PRIORIDADE ---------- */

        /* ---------- INICIO - BAIXA PRIORIDADE ---------- */
        /* TODO: verifica se ha processos voltando de DISCO no instante de tempo t*/

            /* TODO: adiciona processos voltando de DISCO na fila de BAIXA prioridade*/

        /* TODO: verifica se ha processos que sofreram preempcao no instante de tempo t*/

            /* TODO: adiciona processos que sofreram preempcao na fila de BAIXA prioridade*/

        /* ---------- FIM - BAIXA PRIORIDADE ---------- */

        /* ---------- FIM - ADICIONAR NAS FILAS ---------- */

        /* ---------- INICIO - EXECUTAR ---------- */

        /* TODO: seleciona o proximo processo das filas de CPU ou mantém o mesmo processo, se nao houve preempcao/bloqueio */

            /* TODO: decrementa o tempo_cpu do processo */
            /* TODO: decrementa o time slice */

            /* Verifica se ha alguma operacao de I/O no instante t para o processo atual */
                /* Se sim, entao:
                    Tira o processo da fila de CPU
                    Adiciona o processo na fila de I/O correspondente 
                    Muda o time_slice para zero */
                
                /* Senao, verifica se o time slice é zero:
                     Se sim, entao:
                        Move o processo para o fim da fila de baixa prioridade (faz a preempcao)
                     */

                

        /* ---------- FIM - EXECUTAR ---------- */



    }
    printf("Hello World\n");
}