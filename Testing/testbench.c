#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <features.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

// so dat it doesnt raise any errors at compilation
typedef struct process{
    pid_t pid;
    time_t st_time;
    time_t end_time; 
}proc;

typedef struct Process_Queue{
    int n_proc;
    proc list_procs[100];
    sem_t lock;
}Proc_Queue;

char* trim(char* string, char* str){
    int entry_status = 1;
    int exit_status = 0;
    int index = 0;
    for(int i = 0; i < (int)strlen(string);i++){
        if(entry_status == 1 && string[i] != ' ' && string[i] != '\t' && string[i] != '\n'){
            entry_status = 0;
            exit_status = 1;
            str[index] = string[i];
            index++;
            continue;
        }
        if(exit_status == 1){
            if(string[i] == ' ' || string[i] == '\t' || string[i] == '\n'){
                int flag = 0;
                for(int j = i; j < (int)strlen(string); j++){
                    if(string[j] == ' ' || string[j] == '\t' || string[j] == '\n'){
                        continue;
                    }
                    else{
                        flag = 1;
                        break;
                    }
                }
                if(flag == 0){
                    str[index] = '\0';
                    return str;
                }
                else{
                    str[index] = string[i];
                    index++;
                    continue;
                }
            }
            else if(string[i] == '\0'){
                str[index] = '\0';
                return str;
            }
            else{
                str[index] = string[i];
                index++;
                continue;
            }
        }
    }

    str[index] = '\0';
    return str;
}

char* forward_trim(char* string, char* str){
    int index = 0;
    int len = strlen(string);

    for(int i = 0; i < len; i++){
        if(string[i] == ' ' || string[i] == '\t' || string[i] == '\n' || string[i] == '\0'){
            if(index!=0){
                str[index] = string[i];
                index++;
            }
            continue;
        }
        else{
            str[index] = string[i];
            index++;
        }
    }

    if(index == 0){
        return NULL;
    }

    str[index] = '\0';
    return str;
}

void Escape_sequence(int signum){
    if(signum == SIGINT){
        _exit(0);
    }

    if(signum == SIGCHLD){
        int* n = 0;
        int pid = waitpid(-1,n,WCONTINUED);
        printf("%d\n",pid);
    }
}

//takePut: takes the process at the mentioned index and enqueues tot the queue
void takePut(Proc_Queue* queue1,int index){
    proc takenProcess = queue1->list_procs[index];
    for(int i = index+1; i < queue1->n_proc; i++){
        queue1->list_procs[i-1] = queue1->list_procs[i];
    }
    queue1->list_procs[queue1->n_proc] = takenProcess;
}

void formatTime(struct timespec t, char* buffer, size_t bufferSize) {
    long ms = t.tv_nsec / 1000000;
    snprintf(buffer, bufferSize, "%02ld:%02ld:%02ld.%03ld:%09ld", t.tv_sec / 3600, (t.tv_sec % 3600) / 60, t.tv_sec % 60, ms, t.tv_nsec);
}



void timeDEFF(){

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    struct timespec sleep_time;
    sleep_time.tv_sec = 4;
    sleep_time.tv_nsec = 4 * 100000000;

    nanosleep(&sleep_time, NULL);


    clock_gettime(CLOCK_REALTIME, &end);

    long long diff_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    double diff_ms = (double)diff_ns / 1000000.0;

    char start_time_str[30], end_time_str[30];

    strftime(start_time_str, sizeof(start_time_str), "%H:%M:%S", localtime(&start.tv_sec));
    strftime(end_time_str, sizeof(end_time_str), "%H:%M:%S", localtime(&end.tv_sec));

    printf("Start time: %s.%09ld\n", start_time_str, start.tv_nsec);
    printf("End time: %s.%09ld\n", end_time_str, end.tv_nsec);
    printf("Time diff = %.3lf milliseconds\n", diff_ms);
}


void main(){
    char* args[] = {"./test2",NULL};

    int status = fork();

    if(status == 0){
        execvp("./test2",args);
    }
    // else if(status > 0){
    //     kill(status,SIGSTOP);
    //     sleep(5);
    //     kill(status,SIGCONT);
    // }

    // Proc_Queue* p1 = (Proc_Queue*)malloc(sizeof(Proc_Queue));
    // proc* p2 = (proc*)malloc(sizeof(proc));
    // proc* p3 = (proc*)malloc(sizeof(proc));
    // p2->pid = 3;
    // p3->pid = 4;
    // p1 ->list_procs[0] = *p2;
    // p1 ->list_procs[1] = *p3;
    // printf("%d\n",p1->list_procs[0].pid);
    // takePut(p1,0);
    // printf("%d\n",p1->list_procs[1].pid);
    /*
        struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    struct timespec sleep_time;
    sleep_time.tv_sec = 4;
    sleep_time.tv_nsec = 4 * 100000000;

    nanosleep(&sleep_time, NULL);

    clock_gettime(CLOCK_REALTIME, &end);

    long long diff_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    double diff_ms = (double)diff_ns / 1000000.0;

    char start_time_str[30], end_time_str[30];

    strftime(start_time_str, sizeof(start_time_str), "%H:%M:%S", localtime(&start.tv_sec));
    strftime(end_time_str, sizeof(end_time_str), "%H:%M:%S", localtime(&end.tv_sec));

    printf("Start time: %s.%09ld\n", start_time_str, start.tv_nsec);
    printf("End time: %s.%09ld\n", end_time_str, end.tv_nsec);
    printf("Time diff = %.3lf milliseconds\n", diff_ms);*/
//   timeDEFF();
printf("%d",-1%4);

}