#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

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


// Label status
char statuses[5][10] = {
    {"NEW"},
    {"READY"},
    {"RUNNING"},
    {"WAITING"},
    {"TERMINATED"},
};

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

Process* init_process(int priority, unsigned int time_cpu, unsigned int arrival, int* start_io, int* duration_io);
Process** generate_processes();
Process** generate_random_processes(int amount);
void add_process(Process* p, Process** queue);
Process* remove_process(Process** queue);
void get_queue_str(Process** queue, char* str);
Process* get_running_process(void);
void run_process(Process* running_process, int* time_slice);

// Filas
Process* high_priority_queue = NULL;
Process* low_priority_queue = NULL;
Process* printer_queue = NULL;
Process* disk_queue = NULL;
Process* magnetic_tape_queue = NULL;

// Representação em string para processos nas filas
char high_priority_queue_str[15] = "";
char low_priority_queue_str[15] = "";
char printer_queue_str[15] = "";
char disk_queue_str[15] = "";
char magnetic_tape_queue_str[15] = "";

int main(int argc, char **argv){

    int i;
    int t = 0;
    int time_slice = 0;
    Process* running_process = NULL;
    Process** processes_list = NULL;

    processes_list = generate_processes();

    // Teste para saber se os processos estão sendo corretamente criados
    for (i = 0; i < process_number; i++) {
        printf("\nPID = %d\n", processes_list[i]->pid);
        if (processes_list[i]->status == NEW)
            printf("Status = NEW\n");
        printf("Priority = %d\n", processes_list[i]->priority);
        printf("CPU Time = %d\n", processes_list[i]->time_cpu);
        printf("Arrival = %d\n", processes_list[i]->arrival);
        printf("start_io = %d, %d\n\n", processes_list[i]->start_io[0], processes_list[i]->duration_io[0]);
    }

    printf("\n\nPressione enter para continuar...");
    gets();


    // Inicializando tabelas no ncurses    
    initscr();
    noecho();
    cbreak();

    WINDOW* pid = newwin(2 + 6, 5, 0, 0);// tam_y, tam_x, pos_y, pos_x
    WINDOW* cpu_time = newwin(2 + 6, 10, 0, pid->_begx + pid->_maxx);
    WINDOW* status = newwin(2 + 6, 14, 0, cpu_time->_begx + cpu_time->_maxx);
    WINDOW* io = newwin(2 + 6, 10, 0, status->_begx + status->_maxx);
    WINDOW* queues = newwin(2 + 6, io->_begx + io->_maxx + 1, 8, 0);
    box(pid, 0, 0);
    box(cpu_time, 0, 0);
    box(status, 0, 0);
    box(io, 0, 0);
    box(queues, 0, 0);


    mvwprintw(pid, 1, 1, "PID");
    mvwprintw(cpu_time, 1, 1, "Cpu Time");
    mvwprintw(status, 1, 1, "Status");
    mvwprintw(io, 1, 1, "IO");
    mvwprintw(queues, 1, 1, "Filas");
    refresh();
    wrefresh(pid);
    wrefresh(cpu_time);
    wrefresh(status);
    wrefresh(io);
    wrefresh(queues);

    while(t >= 0){
        // Preenche novamente as tabelas a cada passo do scheduler
        for (i = 0; i < process_number; i++) {
            mvwprintw(pid, i + 2, 1, " %d ", processes_list[i]->pid);
            mvwprintw(status, i + 2, 1, "%s", statuses[processes_list[i]->status]);
            mvwprintw(cpu_time, i + 2, 1, " %d ", processes_list[i]->time_cpu);
        }
        
        get_queue_str(&high_priority_queue, &high_priority_queue_str);
        get_queue_str(&low_priority_queue, &low_priority_queue_str);

        mvwprintw(queues, 0 + 2, 1, "Alta Prioridade  %s", high_priority_queue_str);
        mvwprintw(queues, 1 + 2, 1, "Baixa Prioridade %s", low_priority_queue_str);

        refresh();
        wrefresh(pid);
        wrefresh(cpu_time);
        wrefresh(status);
        wrefresh(io);
        wrefresh(queues);

        // Sleep para facilitar visualização de cada passo do scheduler
        sleep(1);

        /* ---------- INICIO - ADICIONA PROCESSOS NOVOS NA FILA ---------- */

        /* Verifica se ha processos NOVOS no instante t*/
        for (i = 0; i < process_number; i++){
            // Verifica se tem alguma chegada no instante t
            if (processes_list[i]->arrival == t){
                processes_list[i]->status = NEW;
                // Adiciona imediatamente na fila de alta prioridade
                processes_list[i]->status = READY;
                add_process(processes_list[i], &high_priority_queue);
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

            /* Realiza execucao de CPU */
            run_process(running_process, &time_slice);
        }
        else{
            // TODO: Alterar console para mostrar processo que está 
            // sendo executado ou se está ocioso
            // printf("Processador ocioso.\n");
        }

        /* ---------- FIM - EXECUCAO CPU ---------- */

        /* ---------- INICIO - EXECUCAO I/O ---------- */

        /* ---------- FIM - EXECUCAO I/O ---------- */

        // Verifica se todos os processos terminaram
        if (terminated == process_number){
            // TODO: liberar memoria alocada dos processos
            return 0;
        }

        // Senao, incrementa o tempo
        t++;
    }
    endwin();
}

/* Funcoes */

// Cria um processo com as informações passadas
Process* init_process(int priority, unsigned int time_cpu, unsigned int arrival, int* start_io, int* duration_io) {

    // Inicializa um processo
    Process* p = (Process*) malloc(sizeof(Process));

    // O processo recebe suas características
    p->pid = process_number++;
    p->status = NEW;
    p->priority = priority;
    p->time_cpu = time_cpu;
    p->arrival = arrival;

    // Inicializa as arrays de I/O
    // Caso 1: Processo nao tem I/O
    if(start_io == NULL){
        for(int i = 0; i < IO_TYPES; i++){
            p->start_io[i] = -1;
            p->duration_io[i] = -1;
        }
    }
    // Caso 2: Processo tem I/O
    else{
        for(int i = 0; i < IO_TYPES; i++){
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
    // int start_io[3] = {4, -1, -1};
    // int duration_io[3] = {2, -1, -1};

    processes_list[0] = init_process(HIGH, 8, 1, NULL, NULL);
    processes_list[1] = init_process(HIGH, 3, 2, NULL, NULL);
    processes_list[2] = init_process(HIGH, 10, 4, NULL, NULL);
    processes_list[3] = init_process(HIGH, 1, 4, NULL, NULL);
    processes_list[4] = init_process(HIGH, 2, 11, NULL, NULL);
    
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
        running_process->status = TERMINATED;
        terminated++;
        // zera o time slice para indicar que houve término
        *time_slice = 0;
    }
    // Se o time slice acabou, faz a preempcao
    else if(*time_slice == 0){
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
void get_queue_str(Process** queue, char* str) {
    if(queue == NULL) return;
    Process* head = *queue;
    strcpy(str, "");
    while (head != NULL) {
        strcat(str, " ");
        char pid[] = { head->pid + '0' };
        printf("%s", pid);
        strcat(str, pid);
        head = head->next;
    }
    for (int i = strlen(str); i < 10; i++){
        str[i] = ' ';
    }
    return;
}
