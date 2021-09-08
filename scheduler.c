#include <stdio.h>
#include <stdlib.h>

// Controles de estados
#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3
#define TERMINATED 4

// Prioridade
#define HIGH 1
#define LOW 0

// Fatia de tempo do Round-Robin
#define QUANTUM 4

// Tipos de I/O diferentes
#define IO_TYPES 3
#define DISK 0
#define MAGNETIC_TAPE 1
#define PRINTER 2

typedef struct process {
    // PCB
    int pid;
    int status;
    int priority;

    // Infos de execucao
    int time_cpu;
    int arrival;
    int start_io[IO_TYPES]; /* [inicio_disco, inicio_fita, inicio_impressora] */
    int duration_io[IO_TYPES]; /* [tempo_disco, tempo_fita, tempo_impressora]  */

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

    static int time_slice = 0;
    int start_io, i, io_type;
    Process* running_process;
    Process** queue;


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

        /* ---------- FIM - BAIXA PRIORIDADE ---------- */

        /* ---------- FIM - ADICIONAR NAS FILAS ---------- */

        /* ---------- INICIO - EXECUCAO CPU ---------- */

        // TODO: encapsular em funcao
        /* Seleciona o processo a ser executado.
        Mantém o mesmo processo ou, se houve preempcao/bloqueio/término, seleciona o primeiro processo das filas de pronto. */

        /* Quando time_slice == 0, houve preempcao, bloqueio ou término. Logo, precisamos de um novo processo. */
        if(time_slice == 0){

            // Restaura time slice
            time_slice = QUANTUM;

            // Busca processo na fila de alta prioridade
            running_process = remove_process(high_priority_queue);

            // Se estiver vazia, busca na fila de baixa prioridade
            if (running_process == NULL){
                running_process = remove_process(low_priority_queue);

                // Se nao houver processo em nenhuma fila para CPU, pula o ciclo
                if (running_process == NULL)
                    continue;
            }

            // Altera status do processo
            running_process->status = RUNNING;
        }
        
        /* TODO: decrementa o tempo_cpu do processo */
        running_process->time_cpu--;
        /* TODO: decrementa o time slice */
        time_slice--;

        /* verifica se algum dos inicios de I/O batem com o time_cpu atual */
        io_type = -1;
        for (i = 0; i < IO_TYPES; i++){
            start_io = running_process->start_io[i];
            // se existe um I/O que começa agora, pegamos o seu numero
            if (running_process->time_cpu == start_io)
                io_type = i;
        }
        
        // Se pegou um numero de I/O, pega a fila de I/O correspondente
        if (io_type >= 0){
            switch (io_type){
                case DISK:
                    queue = disk_queue;
                    break;
                case MAGNETIC_TAPE:
                    queue = magnetic_tape_queue;
                    break;
                case PRINTER:
                    queue = printer_queue;
                    break;
            }
            // adiciona o processo na fila
            if (queue != NULL){
                add_process(running_process, queue);
                running_process->status = WAITING;
            }
            // zera o time slice para indicar que houve bloqueio
            time_slice = 0;
        }
        // Se o tempo de serviço do processo acabou, termina o processo
        else if(running_process->time_cpu == 0){
            running_process->status == TERMINATED;
            // zera o time slice para indicar que houve término
            time_slice = 0;
        }
        // Se o time slice acabou, faz a preempcao
        else if(time_slice == 0){
            // Move o processo para a fila de baixa prioridade
            add_process(running_process, low_priority_queue);
            // Muda o status
            running_process->status = READY;
        }

        /* ---------- FIM - EXECUCAO CPU ---------- */

        /* ---------- INICIO - EXECUCAO I/O ---------- */

        /* ---------- FIM - EXECUCAO I/O ---------- */



    }
    printf("Hello World\n");
}