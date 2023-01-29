#include "os.h"

int get_victim_page(table* my_table) {
    //next week
    //This function returns victim page to be evicted
    return 0;
}

int alloc_page(table* my_table, pool* my_pool) {
    //This function returns one PFN to be allocated
    return 0;
}

int swap(table* my_table) {
    //next week
    //This function does some swap works
    return 0;
}

void allocator(task my_task) {
    //OS thread uses this function
    //This function checks task type and calls alloc function
    //This function must handle following three types of tasks
    //task type 0: new allocation is needed
    //task type 1: access page in swap space
    //task type 2: deallocation of finished thread

    if(my_task.type==0){

    }
    else if(my_task.type == 1){

    }
    else if(my_task.type == 2){
        
    }
    return;
}

int VA_to_VPN(int VA) {
    //this function translate VA to VPN
    return 0;
}

void dealloc_page(table* my_table, pool* mem_pool, pool* swap_pool) {
    //This function traverses page table and dealloc page if there exists allocated page
    //You don't have to consider swap_pool because swap call isn't called this week
    return;
}

void classify_task(table* my_table, int VPN) {
    if(my_table->entry[VPN].valid == 1){
        if(my_table->entry[VPN].present == 1){ //page in memory
            my_table->entry[VPN].update_time = time(NULL);
        }
        //next week
        //task type 1: page in swap space
        else{
            pthread_mutex_lock(&mutex_q);
            task new_task;
            new_task.my_table = my_table;
            new_task.type = 1;
            new_task.VPN = VPN;
            add_task(&task_pool, new_task);
            pthread_cond_wait(&(my_table->my_cond), &mutex_q);
            pthread_mutex_unlock(&mutex_q);
        }
        //next week ends
    }
    //task type 0: allocation is needed
    else{
        pthread_mutex_lock(&mutex_q);
        task new_task;
        new_task.my_table = my_table;
        new_task.type = 0;
        new_task.VPN = VPN;
        add_task(&task_pool, new_task);
        pthread_cond_wait(&(my_table->my_cond), &mutex_q);
        pthread_mutex_unlock(&mutex_q);
    }
}

void mmu(table* my_table, int VA){
    int VPN = VA_to_VPN(VA);
    classify_task(my_table, VPN);
}