#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

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

// Cores
#define GREEN "\e[0;32m"
#define RESET "\e[0m"
#define RED "\e[0;31m"
#define YELLOW "\e[0;33m"

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

Process* init_process(unsigned int time_cpu, unsigned int arrival, int* start_io, int* duration_io);
Process** generate_processes();
Process** generate_random_processes(int amount);
void add_process(Process* p, Process** queue);
Process* remove_process(Process** queue);
void get_queue_str(Process** queue, char* str);
Process* get_running_process(void);
void run_process(Process* running_process, int* time_slice);
void terminate_process(Process* process);
Process* run_io(Process* io_process, unsigned int io_index);
void print_queue(Process** queue, char* name);
void print_process(Process* process);
void print_all_processes(Process** processes_list);

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

    // Flag para sinalizar se o programa deve utilizar o ncurses
    int use_ncurses = FALSE;
    if(argc>=2)
        for(i = 1; i < argc; i++)
            if(strcmp(argv[i], "-nc") == 0) use_ncurses = TRUE;

    int t = 0;
    int time_slice = 0;
    Process* running_process = NULL;
    Process* disk_process = NULL;
    Process* magnetic_tape_process = NULL;
    Process* printer_process = NULL;
    Process** processes_list = NULL;

    processes_list = generate_processes();
    
    // Teste para saber se os processos estão sendo corretamente criados
    print_all_processes(processes_list);
    
    /*
    printf("\n\nPressione enter para continuar...");
    char aux[2];
    fgets(aux, 2, stdin);

    // Inicializando tabelas no ncurses    
    initscr();
    noecho();
    cbreak();

    WINDOW* pid = newwin(2 + 6, 5, 0, 0);// tam_y, tam_x, pos_y, pos_x
    WINDOW* cpu_time = newwin(2 + 6, 10, 0, pid->_begx + pid->_maxx);
    WINDOW* status = newwin(2 + 6, 14, 0, cpu_time->_begx + cpu_time->_maxx);
    WINDOW* io = newwin(2 + 6, 10, 0, status->_begx + status->_maxx);
    WINDOW* queues = newwin(2 + 6, io->_begx + io->_maxx + 1, 8, 0);

    if(use_ncurses){
        // Inicializando tabelas no ncurses
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
    } */

    while(t >= 0){
        /*
        // Preenche novamente as tabelas a cada passo do scheduler
        if(use_ncurses){
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
        } */

        printf("\n---------- Instante %d ----------\n\n", t);

        /* ---------- INICIO - ADICIONA PROCESSOS NOVOS NA FILA ---------- */

        printf("NOVOS:\n");
        /* Verifica se ha processos NOVOS no instante t*/
        for (i = 0; i < process_number; i++){

            // Verifica se tem alguma chegada no instante t
            if (processes_list[i]->arrival == t){
                processes_list[i]->status = NEW;
                // Adiciona imediatamente na fila de alta prioridade
                processes_list[i]->status = READY;
                add_process(processes_list[i], &high_priority_queue);
                printf("- ");
                printf("%d ", processes_list[i]->pid);
                printf("\n");
            }
            /*  TODO: verificar se algum processo novo pode entrar na fila de alta prioridade
                levando em conta tamanho das filas. Vamos precisar de uma constante para o tamanho maximo
                das filas e uma variavel para guardar o quanto elas estao preenchidas. */
        }
        printf("\n");

        // print das filas
        printf("FILAS: \n");
        print_queue(&high_priority_queue, "CPU_H");
        print_queue(&low_priority_queue, "CPU_L");
        print_queue(&disk_queue, "DISK ");
        print_queue(&magnetic_tape_queue, "TAPE ");
        print_queue(&printer_queue, "PRINT");
        printf("\n");

        /* ---------- FIM - ADICIONA PROCESSOS NOVOS NA FILA ---------- */

        /* ---------- INICIO - SELECIONA PROCESSOS ---------- */

        // print da selecao de processos
        printf("SELEÇÃO:\n");

        // Seleciona o processo a ser executado na CPU.
        // Quando time_slice == 0, houve preempcao, bloqueio ou término. Logo, precisamos de um novo processo.
        if(time_slice == 0){

            // Pega o proximo processo a ser executado
            running_process = get_running_process();

            // So restaura o time slice se tiver pego um processo
            if(running_process != NULL){
                time_slice = QUANTUM;
                printf("- CPU selecionou %d\n", running_process->pid);
            }
        }

        // Seleciona os processos das filas de I/O.
        if (disk_process == NULL){
            disk_process = remove_process(&disk_queue);
            if(disk_process != NULL)
                printf("- DISCO selecionou %d\n", disk_process->pid);
        }
            
        if (magnetic_tape_process == NULL){
            magnetic_tape_process = remove_process(&magnetic_tape_queue);
            if(magnetic_tape_process != NULL)
                printf("- FITA MAGNÉTICA selecionou %d\n", magnetic_tape_process->pid);
        }
            
        if (printer_process == NULL){
            printer_process = remove_process(&printer_queue);
            if(printer_process != NULL)
                printf("- IMPRESSORA selecionou %d\n", printer_process->pid);
        }

        printf("\n");

        /* ---------- FIM - SELECIONA PROCESSOS ---------- */


        /* ---------- INICIO - EXECUCAO CPU ---------- */

        printf("EXECUÇÃO:\n");

        /* Se houver um processo a ser executado, realiza as operacoes da CPU */
        if(running_process != NULL){
            printf("- CPU executando %d (time slice = %d)\n", running_process->pid, time_slice - 1);
            /* Realiza execucao de CPU */
            run_process(running_process, &time_slice);
        }
        else{

            // TODO: Alterar console para mostrar processo que está 
            // sendo executado ou se está ocioso
            printf("- Processador ocioso.\n");
        }

        /* ---------- FIM - EXECUCAO CPU ---------- */

        /* ---------- INICIO - EXECUCAO I/O ---------- */

        /* Se houver um processo a ser executado, realiza as operacoes de I/O */
        if(disk_process != NULL){
            printf("- DISCO executando %d.\n", disk_process->pid);
            disk_process = run_io(disk_process, DISK);
        }
        else{
            printf("- DISCO ocioso.\n");
        }

        if(magnetic_tape_process != NULL){
            printf("- FITA MAGNÉTICA executando %d.\n", magnetic_tape_process->pid);
            magnetic_tape_process = run_io(magnetic_tape_process, MAGNETIC_TAPE);
        }
        else{
            printf("- FITA MAGNÉTICA ociosa.\n");
        }

        if(printer_process != NULL){
            printf("- IMPRESSORA executando %d.\n", printer_process->pid);
            printer_process = run_io(printer_process, PRINTER);
        }
        else{
            printf("- IMPRESSORA ociosa.\n");
        }

        printf("\n");

        /* ---------- FIM - EXECUCAO I/O ---------- */

        // Verifica se todos os processos terminaram
        if (terminated == process_number){
            printf("Todos terminaram.\n");
            printf("%d instantes de tempo\n\n", t+1);
            // TODO: liberar memoria alocada dos processos
            return 0;
        }

        // Senao, incrementa o tempo
        t++;
    }
    // endwin();
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
        for(int i = 0; i < IO_TYPES; i++){
            // Descobre o tempo de inicio de I/O relativo ao tempo de serviço restante
            p->start_io[i] = time_cpu - start_io[i];
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
            printf(YELLOW "Bloqueio de %d\n" RESET, running_process->pid);
        }
        // zera o time slice para indicar que houve bloqueio
        *time_slice = 0;
    }
    // Se o tempo de serviço do processo acabou, termina o processo
    else if(running_process->time_cpu == 0){
        terminate_process(running_process);
        // zera o time slice para indicar que houve término
        *time_slice = 0;
    }
    // Se o time slice acabou, faz a preempcao
    else if(*time_slice == 0){
        printf(YELLOW "Preempcao de %d.\n" RESET, running_process->pid);
        // Move o processo para a fila de baixa prioridade
        add_process(running_process, &low_priority_queue);
        // Muda o status
        running_process->status = READY;
    }
}


// Faz as operacoes necessarias para terminar um processo
void terminate_process(Process* process){
    printf(RED "Terminou o %d\n" RESET, process->pid);
    process->status = TERMINATED;
    terminated++;
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

// Mostra os processos em um fila
    // Ex: 
        // print_queue(&printer_queue)
void print_queue(Process** queue, char* name) {
    printf("- Fila %s: ", name);
    if(queue == NULL) return;
    Process* head = *queue;
    while (head != NULL) {
        printf("%d ", head->pid);
        head = head->next;
    }
    printf("\n");
}

Process* run_io(Process* io_process, unsigned int io_index){
    
    char* io_name;

    // Decrementa o duration_io
    io_process->duration_io[io_index]--;

    // Se tiver acabado a operacao
    if (io_process->duration_io[io_index] == 0){

        // Se a parte de CPU acabou também
        if(io_process->time_cpu == 0){
            // Termina o processo
            terminate_process(io_process);
        }
        // Senao, envia para a fila de CPU apropriada
        else{
            io_process->status = READY;
            // Se é DISCO, volta para fila de BAIXA prioridade
            if(io_index == DISK)
                add_process(io_process, &low_priority_queue);
            // Senao, volta para a fila de ALTA prioridade
            else if((io_index == MAGNETIC_TAPE) || (io_index == PRINTER))
                add_process(io_process, &high_priority_queue);
        }
        
        switch(io_index){
            case DISK:
                io_name = "DISCO";
                break;
            case MAGNETIC_TAPE:
                io_name = "FITA MAGNÉTICA";
                break;
            case PRINTER:
                io_name = "IMPRESSORA";
                break;
        }

        printf(GREEN "Fim da operacao de %s de %d\n" RESET, io_name, io_process->pid);
        // Muda variavel para indicar que o dispositivo esta livre
        io_process = NULL;
    }
    return io_process;
}

// Imprime as características de um processo
void print_process(Process* process) {
    printf("\nProcesso %d:\n", process->pid);
    printf("\tStatus = ");
    switch (process->status) {
        case NEW:
            printf("NEW\n");
            break;
        case READY:
            printf("READY\n");
            break;
        case RUNNING:
            printf("RUNNING\n");
            break;
        case WAITING:
            printf("WAITING\n");
            break;
        case TERMINATED:
            printf("TERMINATED\n");
            break;
    }
    printf("\tPriority = ");
    switch (process->priority) {
        case LOW:
            printf("LOW\n");
            break;
        case HIGH:
            printf("HIGH\n");
            break;
    }
    printf("\tCPU Time = %d\n", process->time_cpu);
    printf("\tArrival = %d\n", process->arrival);
    for (int i = 0; i < IO_TYPES; i++) {
        if ((process->start_io[i] >= 0) && (process->duration_io[i] > 0)) {
            switch (i) {
                case DISK:
                    printf("Disk:\n");
                    printf("\tStart = %d\n", process->start_io[i]);
                    printf("\tDuration = %d\n", process->duration_io[i]);
                    break;
                case MAGNETIC_TAPE:
                    printf("Magnetic Tape:\n");
                    printf("\tStart = %d\n", process->start_io[i]);
                    printf("\tDuration = %d\n", process->duration_io[i]);
                    break;
                case PRINTER:
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
