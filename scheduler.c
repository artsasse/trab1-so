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

// Adicionar processo em um fila
    // Ex: Processo vai fazer IO de impressora
        // add_process(p1, &printer_queue)
    // Ex: Processo foi preemptado
        // add_process(p1, &low_priority_queue)
    // Ex: Processo novo
        // add_process(p1, &low_priority_queue)
void add_process(Process* p, Process** queue);

// Retirar o primeiro processo de uma fila
    // Ex: preempcao
        //  remove_process(&high_priority_queue) 
    // Ex: fim de IO
        //  remove_process(&printer_queue) 
Process* remove_process(Process** queue);

// Mostra os processos em um fila
    // Ex: 
        // print_queue(&printer_queue)
void print_queue(Process** queue);

Process* get_running_process(void);

void run_process(Process* running_process, int* time_slice);

// Filas
Process* high_priority_queue;
Process* low_priority_queue;
Process* printer_queue;
Process* disk_queue;
Process* magnetic_tape_queue;

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

        /* ---------- INICIO - ADICIONA PROCESSOS NOVOS NA FILA ---------- */
        /* TODO: verifica se ha processos NOVOS no instante de tempo t*/
            /* TODO: adiciona processos NOVOS na fila de ALTA prioridade*/

        /* ---------- FIM - ADICIONA PROCESSOS NOVOS NA FILA ---------- */

        /* ---------- INICIO - SELECIONA PROCESSOS ---------- */

        // Seleciona o processo a ser executado na CPU.
        // Quando time_slice == 0, houve preempcao, bloqueio ou término. Logo, precisamos de um novo processo.
        if(time_slice == 0){

            // Restaura time slice
            time_slice = QUANTUM;
            
            // Pega o proximo processo a ser executado
            running_process = get_running_process();

        // TODO: Selecionar processos das filas de I/O.
        
        }

        /* ---------- FIM - SELECIONA PROCESSOS ---------- */


        /* ---------- INICIO - EXECUCAO CPU ---------- */

        /* Se houver um processo a ser executado, realiza as operacoes da CPU */
        if(running_process != NULL){

            /* Realiza execucao de CPU */
            run_process(running_process, &time_slice);
        }

        /* ---------- FIM - EXECUCAO CPU ---------- */

        /* ---------- INICIO - EXECUCAO I/O ---------- */

        /* ---------- FIM - EXECUCAO I/O ---------- */

    }
    printf("Hello World\n");
}

/* Funcoes */

/*  Retorna o ponteiro do proximo processo a ser executado. 
    Retorna NULL se nao houver processos nas filas. */
Process* get_running_process(void){

    Process* running_process = NULL;

    // Busca processo na fila de alta prioridade
    running_process = remove_process(high_priority_queue);

    // Se estiver vazia, busca na fila de baixa prioridade
    if (running_process == NULL){
        running_process = remove_process(low_priority_queue);
    }

    // Altera status do processo
    if (running_process != NULL)
        running_process->status = RUNNING;

    return running_process;
}

/*  Realiza a execução do processo na CPU. */
void run_process(Process* running_process, int* time_slice){

    int io_type, i;
    Process** queue = NULL;

    /* Decrementa o time slice */
    *(time_slice)--;

    /* Decrementa o time_cpu do processo */
    running_process->time_cpu--;

    /* verifica se algum I/O vai começar no proximo instante de tempo */
    io_type = -1;
    for (i = 0; i < IO_TYPES; i++){
        // se existe um I/O que começa agora, seleciona o seu numero
        if (running_process->time_cpu == running_process->start_io[i]){
            io_type = i;
            break;
        }
    }
    
    // Se selecionou um numero de I/O, escolhe a fila de I/O correspondente
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
        *(time_slice) = 0;
    }
    // Se o tempo de serviço do processo acabou, termina o processo
    else if(running_process->time_cpu == 0){
        running_process->status == TERMINATED;
        // zera o time slice para indicar que houve término
        *(time_slice) = 0;
    }
    // Se o time slice acabou, faz a preempcao
    else if(time_slice == 0){
        // Move o processo para a fila de baixa prioridade
        add_process(running_process, low_priority_queue);
        // Muda o status
        running_process->status = READY;
    }
}

Process* remove_process(Process** queue) {
    if(queue == NULL) return NULL;
    Process* head = *queue;
    Process* parent = NULL;

    while (head->next) {
        parent = head;
        head = head->next;
    }
    parent->next = NULL;
    return head;
}

void add_process(Process* p, Process** queue) {
    if(queue != NULL) p->next = *queue;
    *queue = p;
}

void print_queue(Process** queue) {
    printf("\nProcessos na fila:\n");
    if(queue == NULL) return;
    Process* head = *queue;
    while (head != NULL) {
        printf(" %d ", head->pid);
        head = head->next;
    }
}
