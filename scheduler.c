#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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


// Variável auxiliar para contar o número de processos
int process_number = 0;

// Variável auxiliar para contar o número de processos que terminaram
int terminated = 0;

typedef struct process {
    // PCB
    int pid;
    int status;
    int priority;

    // Infos de execucao
    unsigned int time_cpu;
    unsigned int arrival;
    int start_io[IO_TYPES]; /* [inicio_disco, inicio_fita, inicio_impressora] */
    int duration_io[IO_TYPES]; /* [tempo_disco, tempo_fita, tempo_impressora]  */

    // Ponteiro de fila
    struct process* next;
} Process;

// Prototypes

Process* init_process(unsigned int time_cpu, unsigned int arrival, int* start_io, int* duration_io);
Process** generate_processes();
Process** generate_random_processes(int amount);
void add_process(Process* p, Process** queue);
Process* remove_process(Process** queue);
void print_queue(Process** queue);
Process* get_running_process(void);
void run_process(Process* running_process, int* time_slice);
void print_process(Process* process);
void print_all_processes(Process** processes_list);

// Filas
Process* high_priority_queue = NULL;
Process* low_priority_queue = NULL;
Process* printer_queue = NULL;
Process* disk_queue = NULL;
Process* magnetic_tape_queue = NULL;


int main(int argc, char **argv){

    int i;
    int t = 0;
    int time_slice = 0;
    Process* running_process = NULL;
    Process** processes_list = NULL;

    processes_list = generate_processes();
    
    // Teste para saber se os processos estão sendo corretamente criados
    print_all_processes(processes_list);

    while(t >= 0){
            
        /* ---------- INICIO - ADICIONA PROCESSOS NOVOS NA FILA ---------- */

        /* Verifica se ha processos NOVOS no instante t*/
        for (i = 0; i < process_number; i++){
            // Verifica se tem alguma chegada no instante t
            if (processes_list[i]->arrival == t){
                processes_list[i]->status = NEW;
                // Adiciona imediatamente na fila de alta prioridade
                processes_list[i]->status = READY;
                add_process(processes_list[i], &high_priority_queue);
                printf("Adicionou %d\n", processes_list[i]->pid);
            }
            /*  TODO: verificar se algum processo novo pode entrar na fila de alta prioridade
                levando em conta tamanho das filas. Vamos precisar de uma constante para o tamanho maximo
                das filas e uma variavel para guardar o quanto elas estao preenchidas. */
        }

        /* ---------- FIM - ADICIONA PROCESSOS NOVOS NA FILA ---------- */

        /* ---------- INICIO - SELECIONA PROCESSOS ---------- */

        // Seleciona o processo a ser executado na CPU.
        // Quando time_slice == 0, houve preempcao, bloqueio ou término. Logo, precisamos de um novo processo.
        if(time_slice == 0){

            // Pega o proximo processo a ser executado
            running_process = get_running_process();

            // So restaura o time slice se tiver pego um processo
            if(running_process != NULL)
                time_slice = QUANTUM;

        // TODO: Selecionar processos das filas de I/O.
        
        }

        /* ---------- FIM - SELECIONA PROCESSOS ---------- */


        /* ---------- INICIO - EXECUCAO CPU ---------- */

        /* Se houver um processo a ser executado, realiza as operacoes da CPU */
        if(running_process != NULL){

            printf("t = %d. Executando %d\n", t, running_process->pid);
            /* Realiza execucao de CPU */
            run_process(running_process, &time_slice);
        }
        else{
            //printf("Processador ocioso.\n");
        }

        /* ---------- FIM - EXECUCAO CPU ---------- */

        /* ---------- INICIO - EXECUCAO I/O ---------- */

        /* ---------- FIM - EXECUCAO I/O ---------- */

        // Verifica se todos os processos terminaram
        if (terminated == process_number){
            printf("Todos terminaram.\n");
            printf("%d instantes de tempo\n", t+1);
            // TODO: liberar memoria alocada dos processos
            return 0;
        }

        // Senao, incrementa o tempo
        t++;
    }
    printf("Hello World\n");
}

/* Funcoes */

// Cria um processo com as informações passadas
Process* init_process(unsigned int time_cpu, unsigned int arrival, int* start_io, int* duration_io) {

    int i;
    // Inicializa um processo
    Process* p = (Process*) malloc(sizeof(Process));

    // O processo recebe suas características
    p->pid = process_number++;
    p->status = NEW;
    p->priority = HIGH;
    p->time_cpu = time_cpu;
    p->arrival = arrival;

    // Inicializa as arrays de I/O
    // Caso 1: Processo nao tem I/O
    if(start_io == NULL){
        for (i = 0; i < IO_TYPES; i++) {
            p->start_io[i] = -1;
            p->duration_io[i] = -1;
        }
    }
    // Caso 2: Processo tem I/O
    else{
        for(i = 0; i < IO_TYPES; i++) {
            p->start_io[i] = start_io[i];
            p->duration_io[i] = duration_io[i];
        }
    }
    
    
    // Processo começa fora de qualquer fila
    p->next = NULL;

    return p;
}

//Cria uma lista de 5 processos
    // São os processos da questão 2 da lista 2
Process** generate_processes() {

    // Cria a lista de processos
    Process** processes_list = (Process**) malloc(5 * sizeof(Process*));
    int start_io0[3] = {4, -1, -1};
    int duration_io0[3] = {2, -1, -1};
    int start_io4[3] = {-1, 2, 5};
    int duration_io4[3] = {-1, 2, 3};

    processes_list[0] = init_process(8, 1, start_io0, duration_io0);
    processes_list[1] = init_process(3, 2, NULL, NULL);
    processes_list[2] = init_process(10, 4, NULL, NULL);
    processes_list[3] = init_process(1, 4, NULL, NULL);
    processes_list[4] = init_process(2, 11, start_io4, duration_io4);
    
    return processes_list;
}

// Gera uma lista com processos criados aleatoriamente
// Process** generate_random_processes(int amount) {

//     int i;

//     // Cria a lista de processos
//     Process** processes_list = (Process**) malloc(amount * sizeof(Process*));

//     srand(time(NULL));

//     // Cria um processo de cada vez aleatoriamente
//     for (i = 0; i < amount; i++) {
//         /* Na criação, a priority está no intervalo [1,10], 
//            time_cpu no intervalo [1,20] e arrival no intervalo [0,10] */
//         processes_list[i] = init_process((rand() % 9)+1, (rand() % 19)+1, rand() % 11);
//     }

//     return processes_list;
// }

/*  Retorna o ponteiro do proximo processo a ser executado. 
    Retorna NULL se nao houver processos nas filas. */
Process* get_running_process(void){

    Process* running_process = NULL;

    // Busca processo na fila de alta prioridade
    running_process = remove_process(&high_priority_queue);

    // Se estiver vazia, busca na fila de baixa prioridade
    if (running_process == NULL){
        running_process = remove_process(&low_priority_queue);
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
    (*time_slice)--;

    /* Decrementa o time_cpu do processo */
    running_process->time_cpu--;
    // if(running_process->time_cpu >= -10)
    //     printf("time_cpu do p%d = %d\n", running_process->pid, running_process->time_cpu);

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
                printf("Entrou no I/O de DISCO.\n");
                printf("%d, %d, %d\n", running_process->start_io[0], running_process->start_io[1], running_process->start_io[2]);
                printf("time_cpu = %d\n", running_process->time_cpu);
                printf("pid de quem entrou no I/O = %d\n", running_process->pid);
                queue = &disk_queue;
                break;
            case MAGNETIC_TAPE:
                queue = &magnetic_tape_queue;
                break;
            case PRINTER:
                queue = &printer_queue;
                break;
        }
        // adiciona o processo na fila
        if (queue != NULL){
            add_process(running_process, queue);
            running_process->status = WAITING;
        }
        // zera o time slice para indicar que houve bloqueio
        *time_slice = 0;
    }
    // Se o tempo de serviço do processo acabou, termina o processo
    else if(running_process->time_cpu == 0){
        printf("Terminou o %d\n", running_process->pid);
        running_process->status = TERMINATED;
        terminated++;
        // zera o time slice para indicar que houve término
        *time_slice = 0;
    }
    // Se o time slice acabou, faz a preempcao
    else if(*time_slice == 0){
        printf("Rolou preempcao.\n");
        // Move o processo para a fila de baixa prioridade
        add_process(running_process, &low_priority_queue);
        // Muda o status
        running_process->status = READY;
    }
}

// Retirar o primeiro processo de uma fila
    // Ex: preempcao
        //  remove_process(&high_priority_queue) 
    // Ex: fim de IO
        //  remove_process(&printer_queue) 
Process* remove_process(Process** queue) {
    if(*queue == NULL) return NULL;
    Process* head = *queue;
    Process* parent = head;

    // Se houver apenas 1 processo na fila
    if(head->next == NULL){
        *queue = NULL;
        return head;
    }
    
    // Se tem mais de 1 processo na fila
    while (head->next) {
        parent = head;
        head = head->next;
    }

    parent->next = NULL;
    return head;
}

// Adicionar processo em um fila
    // Ex: Processo vai fazer IO de impressora
        // add_process(p1, &printer_queue)
    // Ex: Processo foi preemptado
        // add_process(p1, &low_priority_queue)
    // Ex: Processo novo
        // add_process(p1, &low_priority_queue)
void add_process(Process* p, Process** queue) {
    if(queue != NULL) p->next = *queue;
    *queue = p;
}

// Mostra os processos em um fila
    // Ex: 
        // print_queue(&printer_queue)
void print_queue(Process** queue) {
    printf("\nProcessos na fila:\n");
    if(queue == NULL) return;
    Process* head = *queue;
    while (head != NULL) {
        printf(" %d ", head->pid);
        head = head->next;
    }
}

// Imprime as características de um processo
void print_process(Process* process) {
    printf("\nProcesso %d:\n", process->pid);
    printf("\tStatus = ");
    switch (process->status) {
        case 0:
            printf("NEW\n");
            break;
        case 1:
            printf("READY\n");
            break;
        case 2:
            printf("RUNNING\n");
            break;
        case 3:
            printf("WAITING\n");
            break;
        case 4:
            printf("TERMINATED\n");
            break;
    }
    printf("\tPriority = ");
    switch (process->priority) {
        case 0:
            printf("LOW\n");
            break;
        case 1:
            printf("HIGH\n");
            break;
    }
    printf("\tCPU Time = %d\n", process->time_cpu);
    printf("\tArrival = %d\n", process->arrival);
    for (int i = 0; i < IO_TYPES; i++) {
        if (process->start_io[i] >= 0) {
            switch (i) {
                case 0:
                    printf("Disk:\n");
                    printf("\tStart = %d\n", process->start_io[i]);
                    printf("\tDuration = %d\n", process->duration_io[i]);
                    break;
                case 1:
                    printf("Magnetic Tape:\n");
                    printf("\tStart = %d\n", process->start_io[i]);
                    printf("\tDuration = %d\n", process->duration_io[i]);
                    break;
                case 2:
                    printf("Printer:\n");
                    printf("\tStart = %d\n", process->start_io[i]);
                    printf("\tDuration = %d\n", process->duration_io[i]);
                    break;
            }
        }
    }
}

// Imprime todos os processos
void print_all_processes(Process** processes_list) {
    for (int i = 0; i < process_number; i++) {
        print_process(processes_list[i]);
    }
}