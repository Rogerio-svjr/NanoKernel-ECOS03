#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS 100
#define MAX_COLS 3
#define MAX_LINE_LENGTH 100
#define MAX_NUM_PROCESS 20
#define BUFFERSIZE 10

//Defini��o de estrutura de processo
typedef struct
{
    int arrival_time;
    int service_time;
    int priority;
} Process;

//Vetor de retorno do programa
Process process_final[MAX_NUM_PROCESS];

//Vetor manipulado pela l�gica do escalonador
Process buffer[BUFFERSIZE];
int head = 0;
int tail = 0;

//Inicializa��o do vetor de retorno para l�gica interna ao escalonador
void process_final_Init(void)
{

    int i = 0;
    for(i=0; i<20; i++)
    {
        process_final[i].arrival_time = i;
        process_final[i].service_time = 100;
        process_final[i].priority = 0;
    }
}

//Entrada de processos no buffer circular
void BufferIn(int arrival, int service, int priority)
{
    if (((tail+1)%BUFFERSIZE) != head)
    {
        buffer[tail].arrival_time = arrival;
        buffer[tail].service_time = service;
        buffer[tail].priority = priority;

        tail = (tail+1)%(BUFFERSIZE);
    }
}

//Sa�da de processos no buffer circular
void BufferOut (void)
{
    if (head != tail)
    {
        head = (head +1)%(BUFFERSIZE);
    }
}

//L�gica do escalonador SHORTEST JOB FIRST
void shortest_job_first (Process *buffer_aux, int position)
{
    int i, less = 50;

    //Os processos s�o incluidos individualmente e verificados pela condi��o service_time - arrival_time
    for(i = 0; i < position + 1; i++)
    {
        if((buffer_aux->service_time - buffer_aux->arrival_time) < (process_final[i].service_time - process_final[i].arrival_time))
        {
            less = i;
            break;
        }
    }

    //Os processos s�o reorganizados de modo que os de menor dura��o rodem primeiro no escalonador
    if (less != 50)
    {
        for(i = MAX_NUM_PROCESS - 1; i >= less + 1; i--)
        {
            process_final[i] = process_final[i-1];
        }
        process_final[less] = *buffer_aux;
    }

}

int main()
{

    FILE *file;
    char line[MAX_LINE_LENGTH];
    int matrix[MAX_ROWS][MAX_COLS];
    int row = 0;
    int i, j;

    //Cria��o de ponteiro de fun��o do escalonador
    void (*Scheduler)(Process*, int);
    Scheduler = shortest_job_first;

    //Caminho de busca do arquivo: deve ser mudado para endere�o dentro da m�quina utilizada
    char file_path[] = "C:\\Users\\Acer\\Desktop\\dados.txt";

    //Processos s�o lidos do arquivo
    file = fopen(file_path, "r");

    if (file == NULL) {
        printf("Falha ao abrir o arquivo.\n");
        return 1;
    }

    //Matrix recebe valores lidos do arquivo base
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL && row < MAX_ROWS) {
        sscanf(line, "%d %d %d", &matrix[row][0], &matrix[row][1], &matrix[row][2]);
        row++;
    }
    //Arquivo de entrada � fechado
    fclose(file);

    //Print no terminal dos processos de entrada
    printf("\nEntrada do programa\n\n");

    for (i = 0; i < row; i++) {
        for (j = 0; j < MAX_COLS; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    //Chamada da fun��o de inicilaiza��o do vetor de retorno do programa
    process_final_Init();

    //Passagem dos processos ao buffer circular e inclus�es ao vetor de retorno do programa
    for(i=0; i<5; i++)
    {
        BufferIn(matrix[i][0], matrix[i][1], matrix[i][2]);
        Scheduler(&buffer[i], i);
        BufferOut();
    }
    for(i=5; i<10; i++)
    {
        BufferIn(matrix[i][0], matrix[i][1], matrix[i][2]);
        Scheduler(&buffer[i], i);
        BufferOut();
    }
    for(i=10; i<15; i++)
    {
        BufferIn(matrix[i][0], matrix[i][1], matrix[i][2]);
        Scheduler(&buffer[i-10], i);
        BufferOut();
    }
    for(i=15; i<20; i++)
    {
        BufferIn(matrix[i][0], matrix[i][1], matrix[i][2]);
        Scheduler(&buffer[i-10], i);
        BufferOut();
    }

    //Print no terminal dos processos de saida
    printf("\n\nSaida do programa\n\n");
    for(i=0; i<20; i++)
    {
        printf("%d %d %d\n", process_final[i].arrival_time, process_final[i].service_time, process_final[i].priority);
    }

    //Gera��o do arquivo de sa�da
    file = fopen("C:\\Users\\Acer\\Desktop\\saida.txt", "w");
    if (file == NULL) {
        printf("Falha ao criar o arquivo de sa�da.\n");
        return 1;
    }

    //Print no terminal a sa�da do escalonamento
    for (i = 0; i < MAX_NUM_PROCESS; i++) {
        fprintf(file, "%d %d %d\n", process_final[i].arrival_time, process_final[i].service_time, process_final[i].priority);
    }

    //Arquivo de sa�da � fechado
    fclose(file);

    return 0;
}
