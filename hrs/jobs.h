#pragma once
#ifndef JOBS_H
#define JOBS_H

#include "commands.h"
#define JOB_TABLE_INITIAL_SIZE 8
#define JOB_PROCESS_INITIAL_SIZE 4

typedef enum ProccesState{
    RUNNING,
    STOPPED,
    DONE,
}ProccesState;

typedef struct JobProcess{
    pid_t pid;
    ProccesState state;
    int status;
}JobProcess;

typedef struct JobProcessArr{
    JobProcess *head;
    int cur_index;
    int cur_size;
}JobProcessArr;

typedef struct Job{
    pid_t pgid;
    JobProcessArr* processes;
    ProccesState state;
    int status;
    char *command;
}Job;

typedef struct JobArr{
    Job *head;
    int cur_index;
    int cur_size;
}JobArr;

JobArr *CreateJobArray();
Job *Resize_Job_Array(int size);

void free_jobs_array();
/////////////////////////
JobArr* AddJob(pid_t pgit, pid_t *pr_pids , int pr_count, char* command);
JobArr* RemoveJob(int id);
///////////////////////
JobProcessArr *CreateJobProcessArray();
JobProcessArr *AddJobProcess(JobProcessArr *arr , pid_t pid);

JobProcess *Resize_JobProcess_Array(JobProcessArr *arr,int size);
JobProcess * CreateJobProcess(pid_t pid);
void free_jobprocess_array(JobProcessArr *arr);

#endif