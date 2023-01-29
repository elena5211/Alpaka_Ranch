#include <pthread.h>
#include <string.h>
#include <stdlib.h>

typedef struct page_table_entry{
    int valid;
    int present;
    int PFN;
    time_t update_time;

}entry;

typedef struct page_table {    
    entry* entry;
    int size;
    pthread_cond_t my_cond;
}table;

typedef struct task{
    table* my_table;
    int type;
    int VPN;
}task;

typedef struct pool{
    int* pool;
    int size;
    int front;
    int rear;
}pool;

typedef struct task_q{
    task* task_q;
    int front;
    int rear;
    int size;
}task_q;

int isIDLE(task_q* os_q){
    if(os_q->front == os_q->rear)
        return 1;
    else
        return 0;
}

task get_task(task_q* os_q){
    os_q->front = (os_q->front+1)%(os_q->size);
    return os_q->task_q[os_q->front];
}

void add_task(task_q* os_q, task new_task){
    os_q->rear = (os_q->rear+1)%(os_q->size);
    os_q->task_q[os_q->rear]=new_task;
}

int IsEmpty(pool* queue){
    if(queue->front==queue->rear)
        return 1;
    else return 0;
}

void addq(pool* queue, int value){
    queue->rear = ((queue->rear)+1)%(queue->size);
    queue->pool[queue->rear]=value;
}
int deleteq(pool* queue){
    queue->front = ((queue->front)+1)%(queue->size);
    return queue->pool[queue->front];
}

void task_q_init(task_q* os_q, int num){
    os_q->task_q = (task*)malloc(sizeof(task)*num);
    memset(os_q->task_q, 0, sizeof(task)*num);
    os_q->front = -1;
    os_q->rear = -1;
    os_q->size = num;
}

void pool_init(pool* pool, int page_num){
    pool->pool = malloc(sizeof(int)*page_num);
    memset(pool->pool, 0, sizeof(int)*page_num);
    pool->front = -1;
    pool->rear = -1;
    pool->size = page_num;

    for(int i = 0; i < page_num; ++i){
        addq(pool, i);
    }
    printf("pool initialized\n");
}

int PHYSIZE = 67108864;
int NUMPAGE = 16384;
int PAGESIZE = 4096;
int evict_ctr = 0;

pool mem_pool;
pool swap_pool;
task_q task_pool;

pthread_mutex_t mutex_q;
