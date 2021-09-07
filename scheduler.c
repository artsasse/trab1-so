#include <stdio.h>
#include <stdlib.h>

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

//Inicializa as informações de cada processo
void init_process(Process* p, int pid, int status, int priority, int time_cpu, int arrival, int time_io[]) {
    p->pid = pid;
    p->status = status;
    p->priority = priority;
    p->time_cpu = time_cpu;
    p->arrival = arrival;

    for (int i = 0; i <= 5; i++) {
        p->time_io[i] = time_io[i];
    }
}

//Gera 5 processos com valores definidos e retorna uma array de ponteiros de processos
Process** generate_processes() {
    Process* p1;
    Process* p2;
    Process* p3;
    Process* p4;
    Process* p5;
    Process* process_list[5];

    p1 = (Process*) malloc(sizeof(Process));
    p2 = (Process*) malloc(sizeof(Process));
    p3 = (Process*) malloc(sizeof(Process));
    p4 = (Process*) malloc(sizeof(Process));
    p5 = (Process*) malloc(sizeof(Process));
    
    init_process(p1, 1, 0, 3, 13, 0, NULL);
    init_process(p2, 2, 0, 4, 11, 4, NULL);
    init_process(p3, 3, 0, 1, 7, 5, NULL);
    init_process(p4, 4, 0, 2, 8, 7, NULL);
    init_process(p5, 5, 0, 5, 16, 10, NULL);

    process_list[0] = p1;
    process_list[1] = p2;
    process_list[2] = p3;
    process_list[3] = p4;
    process_list[4] = p5;

    return process_list;

}

int main(int argc, char **argv){
    /* TODO: funcao pra criar uma array de processos */
    Process** p;
    p = generate_processes();

    for (int i = 0; i < 5; i++) {
        printf("%d\n", (*(p + i))->pid);
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