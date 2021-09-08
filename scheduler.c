#include <stdio.h>
#include <stdlib.h>

// Controles de estados
#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3
#define TERMINATED 4

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
    struct process* next;
} Process;

// Variável auxiliar para contar o número de processos
static int process_number = 0;

// Adicionar processo em um fila (ALEXANDRE)
    // Ex: Processo vai fazer IO de impressora
        // add(p1, printer_queue)
    // Ex: Processo foi preemptado
        // add(p1, low_priority_queue)
    // Ex: Processo novo
        // add(p1, low_priority_queue)
void add_process(Process* p, Process** queue);

// Retirar o primeiro processo de uma fila (ALEXANDRE)
    // Ex: preempcao
        //  remove(high_priority_queue) 
    // Ex: fim de IO
        //  remove(printer_queue) 
Process* remove_process(Process** queue);

// Filas
Process** high_priority_queue;
Process** low_priority_queue;
Process** printer_queue;
Process** disk_queue;
Process** magnetic_tape_queue;

// Gerar processos de exemplo (LUCAS)
    // Retornar array de ponteiros de processos

// Cria um processo com as informações passadas
Process* init_process(int priority, int time_cpu, int arrival) {
    Process* p = (Process*) malloc(sizeof(Process));

    p->pid = process_number++;
    p->status = NEW;
    p->priority = priority;
    p->time_cpu = time_cpu;
    p->arrival = arrival;

    return p;

}

//Cria uma lista de 5 processos
    // São os processos da questão 2 da lista 2
    // Ainda vou colocar a opção de criar uma certa quantia de processos e retornar valores aleatórios
Process** generate_processes() {
    Process** processes_list = (Process**) malloc(5 * sizeof(Process*));

    processes_list[0] = init_process(3, 13, 0);
    processes_list[1] = init_process(4, 11, 4);
    processes_list[2] = init_process(1, 7, 5);
    processes_list[3] = init_process(2, 8, 7);
    processes_list[4] = init_process(5, 16, 10);
    
    return processes_list;

}

int main(int argc, char **argv){
    /* TODO: funcao pra criar uma array de processos */
    Process** processes_list = generate_processes();
    
    for (int i = 0; i < 5; i++) {
        printf("%d\n", processes_list[i]->pid);
    }

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