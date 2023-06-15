#include <stdio.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 3
#define MAX_NUM_QUEUES 2

typedef struct {
    int id;
    int priority;
    int time_left;
    int running;
} Process;

typedef struct {
    Process *processes[MAX_QUEUE_SIZE];
    int head;
    int tail;
} Queue;

typedef struct {
    Queue queues[MAX_NUM_QUEUES];
    int num_queues;
} Scheduler;

void scheduler_init(Scheduler *s) {
    s->num_queues = 0;
}

int scheduler_add_queue(Scheduler *s) {
    if (s->num_queues >= MAX_NUM_QUEUES) {
        return -1;
    }
    Queue *q = &(s->queues[s->num_queues]);
    q->head = 0;
    q->tail = 0;
    s->num_queues++;
    return s->num_queues - 1;
}

int enqueue(Queue *q, Process *item) {
    if ((q->tail + 1) % MAX_QUEUE_SIZE == q->head) {
        return -1; // queue is full
    }
    q->processes[q->tail] = item;
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    return 0;
}

void *dequeue(Queue *q) {
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }
    Process *item = q->processes[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    return item;
}

int scheduler_enqueue(Scheduler *s, int queue_idx, Process *item) {
    if (queue_idx < 0 || queue_idx >= s->num_queues) {
        return -1; // invalid queue index
    }
    return enqueue(&(s->queues[queue_idx]), item);
}

void *scheduler_dequeue(Scheduler *s) {
    int i;
    for (i = 0; i < s->num_queues; i++) {
        Queue *q = &(s->queues[i]);
        Process *item = dequeue(q);
        if (item != NULL) {
            return item;
        }
    }
    return NULL; // all queues are empty
}

// Liga o processo com a maior prioridade da fila
void update_running_process_priority(Queue *q){
    Process *current_process;
    for (int i = 0; i < MAX_QUEUE_SIZE; i++){
        Process *p = q->processes[i];
        if (p->priority > current_process->priority){
            current_process = p;
            //printf("%d\n", current_process->id);
        }
        else p->running = 0;
    }
    current_process->running = 1;
}
// Determina a fila que quer atualizar
void scheduler_priority(Scheduler *s, int queue_idx){
    Queue *q = &(s->queues[queue_idx]);
    update_running_process_priority(q);
}

//Liga o processo com o menor tempo restante
void update_running_process_time(Queue *q){
    Process *current_process = q->processes[0];
    for (int i = 0; i < MAX_QUEUE_SIZE; i++){
        Process *p = q->processes[i];
        if (p->time_left < current_process->time_left){
            current_process = p;
            //printf("%d\n", current_process->id);
        }
        else p->running = 0;
    }
    current_process->running = 1;
}
// Determina a fila que quer atualizar
void scheduler_time_left(Scheduler *s, int queue_idx){
    Queue *q = &(s->queues[queue_idx]);
    update_running_process_time(q);
}

// Mostra a lista de processos
void show_processes(Scheduler *s){
    for (int i = 0; i < s->num_queues; i++){
        Queue *q = &(s->queues[i]);
        for(int j = 0; j < MAX_QUEUE_SIZE; j++){
            Process *p = q->processes[j];
            printf("id: %d  priority: %d  time left: %d  running: %d\n", p->id, p->priority, p->time_left, p->running);
        }
    }
}

void scheduler_clock(Scheduler *s){
    for (int i = 0; i < s->num_queues; i++){
        Queue *q = &(s->queues[i]);
        for(int j = 0; j < MAX_QUEUE_SIZE; j++){
            Process *p = q->processes[j];
            p->time_left--;
        }
    }
}

int main() {
    // Inicializa o escalonador e cria os processos
    Scheduler s;
    scheduler_init(&s);
    int q1 = scheduler_add_queue(&s);
    int q2 = scheduler_add_queue(&s);
    Process p0 = {0, 1, 20, 0};
    Process p1 = {1, 3, 10, 0};
    Process p2 = {2, 6, 5, 0};
    Process p3 = {3, 0, 40, 0};
    Process p4 = {4, 0, 30, 0};
    Process p5 = {5, 0, 50, 0};
    scheduler_enqueue(&s, q1, &p0);
    scheduler_enqueue(&s, q1, &p1); // q1: priority
    Process *item = dequeue(&s.queues[q1]);
    scheduler_enqueue(&s, q1, &p2);
    scheduler_enqueue(&s, q2, &p3);
    scheduler_enqueue(&s, q2, &p4); // q2: time_left
    item = dequeue(&s.queues[q2]);
    scheduler_enqueue(&s, q2, &p5);
    while ((item = scheduler_dequeue(&s)) != NULL) {
        printf("%d ", item->id);
    }
    for(int i = 0; i < 5; i++){
        scheduler_clock(&s);
    }
    printf("\n");
    scheduler_priority(&s, q1);
    scheduler_time_left(&s, q2);
    show_processes(&s);
    return 0;
}
