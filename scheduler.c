#include <stdio.h>

typedef struct processo {
    int pid;
    int status;
    int prioridade;
    int t_cpu;
    int t_io[6]; /* [tempo_disco, inicio_disco, tempo_fita, inicio_fita, tempo_impressora, inicio_impressora] */
} Processo;

int main(int argc, char **argv){
    printf("Hello World\n");
}