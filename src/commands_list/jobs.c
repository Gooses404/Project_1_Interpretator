#include "jobs.h"
static JobArr *Table;
int least_id ;
JobArr *CreateJobArray(){
    Table = malloc(sizeof(JobArr));
    Table->head = malloc(sizeof(Job) * JOB_TABLE_INITIAL_SIZE); 
    Table->cur_index = 0;
    Table->cur_size = JOB_TABLE_INITIAL_SIZE;
    least_id = 0;
    return Table; 
}
JobArr* AddJob(pid_t pgit, pid_t *pr_pids, int pr_count, char* command){
    if(Table == NULL){
        CreateJobArray();
    }
    Job *new_job = &(Table->head[Table->cur_index++]) ;  //Изменяем сущ токен из массива
    new_job->pgid = pgit;
    new_job->command = command;
    new_job->status = -1;
    new_job->state = RUNNING;
    new_job->processes = NULL;
    for(int i = 0 ; i < pr_count ; ++i){
        new_job->processes = AddJobProcess(new_job->processes ,pr_pids[i]);
    }
    if(Table->cur_index >= Table->cur_size){
        Table->cur_size *= 2;
        Table->head = Resize_Job_Array(Table->cur_size);
    }
    return Table;
}

JobProcessArr *AddJobProcess(JobProcessArr *arr, pid_t pid){
    if(arr == NULL){
        arr = CreateJobProcessArray();
    }
    if(arr->cur_index >= arr->cur_size){
        arr->cur_size *=2;
        arr->head = Resize_JobProcess_Array(arr, arr->cur_size);
    }
    JobProcess *proc = &(arr->head[arr->cur_index++]);
    proc->pid = pid;
    proc->state = RUNNING;
    proc->status = -1;
    return arr;
}

JobProcessArr *CreateJobProcessArray(){
    JobProcessArr *arr = malloc(sizeof(JobProcessArr));
    if (arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    arr->head = malloc(sizeof(JobProcess) * JOB_PROCESS_INITIAL_SIZE);
    if (arr->head == NULL) {
        free(arr);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    arr->cur_index = 0;
    arr->cur_size = JOB_PROCESS_INITIAL_SIZE;
    return arr;
}

JobProcess *CreateJobProcess(pid_t pid){
    JobProcess *proc = malloc(sizeof(JobProcess));
    if (proc == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    proc->pid = pid;
    proc->state = RUNNING;
    proc->status = -1;
    return proc;
}


void free_jobs(){
    if(!Table->head)
        return;
    
    
}

void free_jobprocess_array(JobProcessArr *arr){
    if(arr == NULL)
        return;
    free(arr->head);
    free(arr);
}

Job* Resize_Job_Array(int size){
    Job* new_head = (Job*)realloc(Table->head, size * sizeof(Job));
    if (new_head == NULL) {    
        free(Table->head);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_head;
}
JobProcess *Resize_JobProcess_Array(JobProcessArr *arr,int size){
    JobProcess* new_head = (JobProcess*)realloc(arr->head, size * sizeof(JobProcess));
    if (new_head == NULL) {    
        free(arr->head);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_head;
}
