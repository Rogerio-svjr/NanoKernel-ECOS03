#include <stdio.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 20
#define MAX_NUM_QUEUES 3
#define MAX_BUFFER_SIZE 10

#define MAX_ROWS 20
#define MAX_COLS 3
#define MAX_LINE_LENGTH 100

int vetPos = 0;
int rowOut = 0;
int flagQ8 = 0;
int flagQ16 = 0;
int emptyFlag = 0;

typedef struct {
    int id;
    int time_left;
    int priority;
} Process;

typedef struct {
    Process *processes[MAX_QUEUE_SIZE];
    int id;
    int quantum;
    int head;
    int tail;
} Queue;

typedef struct {
    Process *buffer[MAX_BUFFER_SIZE];
    int head;
    int tail;
} Buffer;

typedef struct {
    Queue queues[MAX_NUM_QUEUES];
    int num_queues;
} Scheduler;

void scheduler_init(Scheduler *s) {
    s->num_queues = 0;
}

// Cria uma nova fila e retorna o id da fila criada
void scheduler_add_queue(Scheduler *s, int idVal, int quantumVal) {
    if (s->num_queues >= MAX_NUM_QUEUES) {
        printf("Max number of queues reched\n");
        return;
    }
    Queue *q = &(s->queues[s->num_queues]);
    q->id = idVal;
    q->quantum = quantumVal;
    q->head = 0;
    q->tail = 0;
    s->num_queues++;
}

// Adiciona um novo processo na fila especificada
int enqueue(Queue *q, Process *p) {
    if ((q->tail + 1) % MAX_QUEUE_SIZE == q->head) {
        printf("Queue is full\n");
        return -1; // queue is full
    }
    Process *newProcess = (Process *)malloc(sizeof(Process)); // Aloca na memória para evitar extrapolação de ponteiros
    *newProcess = *p;
    q->processes[q->tail] = newProcess;
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    return 0;
}

// Retira um processo da fila especificada e retorna este mesmo processo
void dequeue(Queue *q, Process vet[MAX_ROWS]) {
    if (q->head+1 == q->tail) {
        emptyFlag = 1;       // Atualiza flag
    } else emptyFlag = 0;
    if (q->head == q->tail) {
        printf("Queue is empty\n");
        return;
    }
    Process *p = q->processes[q->head];
    printf("dequeue:  id: %d  time: %d  priority: %d\n", p->id, p->time_left, p->priority);
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    // LÓGICA DO OUTPUT
    if (p->time_left <= 0 && vetPos < 20) {
        printf("Finished process:  id: %d  time: %d  priority: %d\n", p->id, p->time_left, p->priority);
        vet[vetPos] = *p;
        vetPos++;
    }
}

// Inclui um processo na fila correspondente à sua prioridade
int scheduler_enqueue(Scheduler *s, Process *p) {
    switch (p->priority) {
        case 1:
            return enqueue(&(s->queues[0]), p); // Se processo for prioridade 1, vai pra fila 0
            break;
        case 0:
            return enqueue(&(s->queues[1]), p); // Se processo for prioridade 0, vai pra fila 1
            break;
        default:
            printf("Priority mistach\n");
            return -1;
            break;
    }
}

// Mostra a lista de processos
void show_processes(Scheduler *s){
    for (int i = 0; i < s->num_queues; i++) { // Roda todas as filas 
        Queue *q = &(s->queues[i]);
        int j = q->head;
        while (j != q->tail) { // Roda os processos do ponteiro
            Process p = *q->processes[j];
            printf("Queue: %d  ", i);
            printf("id: %d  time: %d  priority: %d\n", p.id, p.time_left, p.priority); 
            j = (j + 1) % MAX_QUEUE_SIZE;
        }
    }
}

// Decresce o tempo restante do primeiro processo de cada fila
void clock_tick(Scheduler *s, Process vet[MAX_QUEUE_SIZE]) { 
    for (int i = 0; i < s->num_queues; i++) {       // Roda todas as filas 
        Queue *q = &(s->queues[i]);
        //printf("i: %d\n", i);
        while (emptyFlag == 0) {                    // Enquanto uma fila não estiver vazia não passa para a próxima fila
            Process *p = q->processes[q->head];         // Pega o primeiro processo da fila
            switch(i) {                                 // Verifica a fila em questão
                case 0:                                     // Se for fila 0 (quantum 8)
                    p->time_left -= 8;                           // Decresce o timer do processo pelo quantum da fila
                    if (p->time_left > 0) {                      // Se não foi o suficiente para o processo terminar
                        dequeue(q, vet);                            // Retira da fila 0
                        enqueue(&(s->queues[1]), p);                // Joga na fila 1
                    }
                    break;
                case 1:                                     // Se for a fila 1 (quantum 16)
                    p->time_left -= 16;                         // Decresce o timer do processo pelo quantum da fila
                    if (p->time_left > 0) {                     // Se não foi o suficiente para o processo terminar
                        dequeue(q, vet);                            // Retira da fila 1
                        enqueue(&(s->queues[2]), p);                // Joga na fila 2
                    }
                    break;
                case 2:                                     // Se for a fila 2 (sem quantum)
                    p->time_left = 0;                           // "zera" seu tempo para simular um tempo real
                    //dequeue(q, mat);                            // Termina o processo
                    break;
            }
            if (p->time_left <= 0) {                    // Se foi o suficiente para terminar o processo
                p->time_left = 0;                           // "zera" seu tempo para simular um tempo real
                dequeue(q, vet);                            // Retira ele da fila
            }
        }
        emptyFlag = 0;
    }
}

void Buffer_enqueue(Buffer *b, Process *p) {
    if ((b->tail + 1) % MAX_BUFFER_SIZE == b->head) {
        printf("Buffer is full\n");
        return;
    }
    Process *newProcess = (Process *)malloc(sizeof(Process)); // Aloca na memória para evitar extrapolação de ponteiros
    *newProcess = *p;
    printf("B_id: %d", newProcess->id);
    b->buffer[b->tail] = newProcess;
    printf("B_id: %d", newProcess->id);
    b->tail = (b->tail + 1) % MAX_BUFFER_SIZE;
}

int main() {
    // LÓGICA DA LEITURA DO ARQUIVO DE ENTRADA
    // Cria as variáveis
    FILE *fileIn;
    FILE *fileOut;
    char line[MAX_LINE_LENGTH];
    int input[MAX_ROWS][MAX_COLS];
    int rowIn = 0;
    int output[MAX_ROWS][MAX_COLS];
    
    // Procura o caminho e abre o arquivo
    char file_in_path[] = "C:\\ECOS03ProjetoFinal\\Input.txt";
    fileIn = fopen(file_in_path, "r");
    // Se não achar printa erro
    if (fileIn == NULL) { // Checa se achou o arquivo
        printf("Falha ao abrir o arquivo.\n");
        return 1;
    }
    // Lê todo o arquivo e salva cada caracter na matriz de entrada
    while (fgets(line, MAX_LINE_LENGTH, fileIn) != NULL && rowIn < MAX_ROWS) {
        sscanf(line, "%d %d %d", &input[rowIn][0], &input[rowIn][1], &input[rowIn][2]);
        rowIn++;
    }
    // Zera a matriz de saída
    for (int i = 0; i <20; i++) {
        output[i][0] = 0;
        output[i][1] = 0;
        output[i][2] = 0;
    }
    // Fecha o arquivo
    fclose(fileIn);

    // LÓGICA DA CRIAÇÃO DOS PROCESSOS
    // Inicializa o escalonador e cria as filas
    Scheduler s;
    scheduler_init(&s);
    scheduler_add_queue(&s, 0, 8);
    scheduler_add_queue(&s, 1, 16);
    scheduler_add_queue(&s, 2, 0);
    // Inicializa o vetor de processos e o buffer
    Process vetProc[20];
    Buffer buffer;

    // Cria todos os processos e separa por filas de prioridade
    for (int i = 0; i < 20; i++) {
        Process p = {input[i][0], input[i][1], input[i][2]};
        scheduler_enqueue(&s, &p);
    }
    //show_processes(&s);
    // Escalona os processos e adiciona eles no vetor
    clock_tick(&s, vetProc);
    for (int i = 0; i < 20; i++) {
        //printf("id: %d  Time: %d  Priority: %d\n", vetProc[i].id, vetProc[i].time_left, vetProc[i].priority);
        //Buffer_enqueue(&buffer, &(vetProc[i]));
    }
    show_processes(&s);


    // LÓGICA DA ESCRITA DO ARQUIVO DE SAÍDA
    // Gera arquivo de saída
    char file_out_path[] = "C:\\ECOS03ProjetoFinal\\Output.txt";
    fileOut = fopen(file_out_path, "w");
    // Se não conseguir printa erro
    if (fileOut == NULL) {
        //printf("Falha ao criar o arquivo de saida.\n");
        return 1;
    }
    // Escreve a matriz no arquivo de saída
    for (int i = 0; i < rowOut; i++) {
        fprintf(fileOut, "%d %d %d\n", output[i][0], output[i][1], output[i][2]);
    }
    for (int i = 0; i < 20; i++) {
        //printf("%d %d %d\n", output[i][0], output[i][1], output[i][2]);
    }
    // Fecha arquivo de saída
    fclose(fileOut);
    return 0;
}