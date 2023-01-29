#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "os.c"
#include <pthread.h>

int proc_num = 0;
int off = 0;
int rd = 1;
int m = 4;
int64_t num_write = 0;

void p_write(table* my_table, int VA){
    mmu(my_table, VA);
}

void* p_main(void* new_table){
    sleep(random()%(10)+1);
    table* my_table = (table*)new_table;
    for (int i = 0; i < (my_table->size)*PAGESIZE; ++i){
        p_write(my_table, i);
    }
    if(rd){
        for(int j = 0; j < 3; ++j){
            for(int i = 0; i < (my_table->size)*PAGESIZE; ++i){
                int VA = random()%(PAGESIZE*(my_table->size));
                p_write(my_table, VA);
            }
        }
    }
    else {
        for(int j = 0; j < 3; ++j){
            for(int i = 0; i < (my_table->size)*PAGESIZE; ++i){
                p_write(my_table, i);
            }
        }
    }
    sleep(10);
    pthread_mutex_lock(&mutex_q);
    task dealloc_task;
    dealloc_task.my_table = new_table;
    dealloc_task.type = 2;
    dealloc_task.VPN = -1;
    add_task(&task_pool, dealloc_task);
    pthread_cond_wait(&my_table->my_cond, &mutex_q);
    pthread_mutex_unlock(&mutex_q);
    return NULL;
}

void* os_main(){
    while(!off) {
        while(!isIDLE(&task_pool)){
            pthread_mutex_lock(&mutex_q);
            task task = get_task(&task_pool);
            allocator(task);
            //Attention here!! following function wake up sleeping thread
            pthread_cond_signal(&task.my_table->my_cond);
            pthread_mutex_unlock(&mutex_q);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    const char *opt = "p:r:m:";
    char option;

    optind = 1;
    while ( -1 != (option = getopt(argc, argv, opt))) {
        switch (option) {
            case 'p' :
                proc_num = atoi(optarg);
                break;
            case 'r' :
                rd = atoi(optarg);
                break;
            case 'm' :
                m = atoi(optarg);
                PHYSIZE = m*1024*1024;
                NUMPAGE = PHYSIZE/PAGESIZE;
                break;
            default:
                break;
        }
    }
    pool_init(&mem_pool, NUMPAGE);
    pool_init(&swap_pool, 1048576*10);
    task_q_init(&task_pool, RAND_MAX);

    pthread_mutex_init(&mutex_q, NULL);
    
    pthread_t pthread[proc_num];
    table tables[proc_num];
    
    for(int i = 0; i < proc_num; ++i){
        int num_vpage = random()%1024+1;
        num_write += num_vpage*PAGESIZE*4;
        tables[i].entry = malloc(sizeof(entry)*num_vpage);
        memset(tables[i].entry, 0, sizeof(entry)*num_vpage);
        tables[i].size = num_vpage;
        pthread_cond_init(&(tables[i].my_cond), NULL);
        pthread_create(&pthread[i], NULL, p_main, &tables[i]);
    }
    pthread_t os;
    pthread_create(&os, NULL, os_main, NULL);

    for(int i = 0; i < proc_num; ++i){
        pthread_join(pthread[i], NULL);
    }
    off=1;
    pthread_mutex_destroy(&mutex_q);
    printf("%d process finished\n", proc_num);
    printf("num of write %d\n", num_write);
    printf("evict counter %d\n", evict_ctr);
    printf("percent %lf\n", (double)evict_ctr/num_write * 100);
}