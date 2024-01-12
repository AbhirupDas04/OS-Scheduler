#include "shell.h"

#define PATH_MAX 4096

char user_input[100][80];
int curr_idx = 0;

//process struct definition
typedef struct process{
    pid_t pid;                      //pid of the process
    int prio;                       //priority of the process

    int killed;                     // flag to check if the process is killed or not
    int running;                    // flag to check if the process has started execution or not
    
    struct timespec st_time;        // the start time of the process
    struct timespec end_time;       // the end time of the process
    struct timespec last_time;      //the last time the process got CPU time
    
    double execution_time;          // the total execution time of the process
    
    double total_waiting_time;      // the total waiting time of the process
    
    char name[20];                  //the name of the process
}proc;


typedef struct Process_Queue{
    int e_proc;                     // the number of executed processes that have been converted into a string format and stored into the exit_Sequence
    int n_proc;                     //the total number of processes given to the scheduler
    int n_proc_arr[4];              //the proirity queue array(to store the processes in respective priority sub-arrays)
    int d_proc;                     //the total number of deleted processes(executed processes)

    int scheduler_pid;              //the pid of the scheduler
    int scheduler_parent_pid;       //the pid of the schedulers parent
    int shell_pid;

    char exit_Sequence[100][1000];  //array to store all the executed processes details in a string format

    proc list_procs[4][20];         //all the processes given to the scheduler are stored here
    proc list_del[80];              //all the executed processes are stored here
    double avg_wait_list[4];
    int n_del_proc_arr[4];
    char wait_str_list[4][20];

    int sig_int_flag;

    sem_t lock;                     // a semaphore lock
}Proc_Queue;


//initialization of the process queue
Proc_Queue* queue;


int fd_shm;
char* text = "Shared_Mem";

//Escape_sequence: it is a function to print out the details of each and every executed processes when ctrl+C is encountered by the shell
/*Output Format:
Name: <>
PID: <>
Priority: <> (defaults to 1 (least))
Start Time: <>
End Time: <>
Total WaitingTime: <>ms
Total Execution Time: <>ms
*/
void Escape_sequence(int signum){
    if(signum == SIGINT){
        //killing the scheduler and its parent to prevent multiple execution of this function upon catching ctrl+C
        if(kill(queue->scheduler_parent_pid,SIGTERM) == -1){
            write(1,"Kill Failed!\n",13);
            _exit(1);
        }

        //write is a async safe function
        int i=0;
        if(write(1,"\n",1) == -1){
            _exit(1);
        }
        while(strncmp(queue->exit_Sequence[i],"\0", strlen(queue->exit_Sequence[i]))){
            int len = strlen(queue->exit_Sequence[i]);
            for (int j = 0; j < len; j++) {
                if(write(1, &queue->exit_Sequence[i][j], 1) == -1){
                    _exit(1);
                }
            }
            i++;
        }

        if(queue->d_proc!=0){
            if(write(1,"-------------------------------------------------------------------------------------------------------\n\n\nSTATISTICS ON THE EFFECT OF PRIORITY ON OUR PROCESSES:\n\n\n",163) == -1){
                _exit(1);
            }
            if(write(1,"Average Waiting Time of:-\n\n",27) == -1){
                _exit(1);
            }

            if(queue->n_del_proc_arr[3] != 0){
                if(write(1,"Priority 4 -> ",15) == -1){
                    _exit(1);
                }
                if(write(1,queue->wait_str_list[3],sizeof(queue->wait_str_list[3])) == -1){
                    _exit(1);
                }
                if(write(1," ms\n",4) == -1){
                    _exit(1);
                }
            }
            if(queue->n_del_proc_arr[2] != 0){
                if(write(1,"Priority 3 -> ",15) == -1){
                    _exit(1);
                }
                if(write(1,queue->wait_str_list[2],sizeof(queue->wait_str_list[2])) == -1){
                    _exit(1);
                }
                if(write(1," ms\n",4) == -1){
                    _exit(1);
                }
            }
            if(queue->n_del_proc_arr[1] != 0){
                if(write(1,"Priority 2 -> ",15) == -1){
                    _exit(1);
                }
                if(write(1,queue->wait_str_list[1],sizeof(queue->wait_str_list[1]))==-1){
                    _exit(1);
                }
                if(write(1," ms\n",4) == -1){
                    _exit(1);
                }
            }
            if(queue->n_del_proc_arr[0] != 0){
                if(write(1,"Priority 1 -> ",15) == -1){
                    _exit(1);
                }
                if(write(1,queue->wait_str_list[0],sizeof(queue->wait_str_list[0])) == -1){
                    _exit(1);
                }
                if(write(1," ms\n",4) == -1){
                    _exit(1);
                }
            }

            if(write(1,"\n\n",2) == -1){
                _exit(1);
            }
        }

        // for(int i = 0; i < queue->d_proc; i++){
        //     free(&queue->list_del[i]);
        // }

        if(kill(queue->shell_pid,SIGQUIT) == -1){
            write(1,"Kill Failed!\n",13);
            _exit(1);
        }

        if(munmap(queue,sizeof(Proc_Queue)) == -1){
            write(1,"Munmap Failed!\n",15);
            _exit(1);
        }
        if(close(fd_shm) == -1){
            write(1, "Close Failed!\n",14);
            _exit(1);
        }
        if(shm_unlink(text) == -1){
            write(1,"Shm_Unlink Failed!\n",19);
            _exit(1);
        }

        _exit(0);
    }

    if(signum == SIGCHLD){
        int* n = 0;
        waitpid(-1,n,WNOHANG);
    }

    if(signum == SIGQUIT){
        _exit(0);
    }
}


//trim: it is a function to remove all trailing spaces,\t,\n,\0
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


// forward_trim: function to remove all leading spaces,\n,\t,\0
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


//lastBack:  it is a function checks if the last character of a string is an ampersand (&), indicating a background process.
int lastBack(char* string){
    int len = (int)strlen(string);
    for(int i = len-1; i >=0; i--){
        if(string[i] == ' ' || string[i] == '\t' || string[i] == '\n'){
        continue;
        }
        else if(string[i] == '&'){
        return 1;
        }
        else{
        return 0;
        }
    }
    return 0;
}


// remAmp: it is a function to remove '&' from the input
void remAmp(char* old_str, char* new_str){
    int pos;

    int len = (int)strlen(old_str);
    for(int i = len-1; i >=0; i--){
        if(old_str[i] == '&'){
        pos = i;
        break;
        }
    }

    int i;

    for(i = 0; i < pos; i++){
        new_str[i] = old_str[i];
    }

    new_str[i] = '\0';
}


// history: it is a function to print out all the user entered commands to the shell up till that moment
void history(){
    int i=0;
    while(strncmp(user_input[i],"\0", strlen(user_input[i]))){
        printf("%d. ", i+1);
        cyan(user_input[i]);
        printf("\n");
        i++;
    }
}


// launch: it is a function to launch all the functions entered by the user through exec family command
int launch(char command[30],char arg[50],int mode){
    // Create a child process
    int status = fork();

    if(status < 0){
        printf("Fork Failed!");
        return 0;
    }

    else if(status == 0){
        // This code block runs in the child process

        // Check the 'mode' parameter to determine the execution mode
        if(mode == 0 || mode == 3){
            // Split the 'command' string into separate commands if pipes are used
            char* args[50];
            char str[50];
            char* str2 = (char*)malloc(50*sizeof(char));

            if(str2 == NULL){
                printf("Malloc failed!");
                exit(EXIT_FAILURE);
            }

            char* token = strtok(command,"|");

            int fd[2];
            int tmp = 0;

            int i = 0;
            while(token != NULL){
                // Trim leading/trailing spaces from the command
                trim(token,str);
                strcpy(str2,str);
                args[i] = str2;
                str2 = str;
                str2 = (char*)malloc(50*sizeof(char));

                if(str2 == NULL){
                    printf("Malloc failed!");
                    exit(EXIT_FAILURE);
                }

                i++;
                token = strtok(NULL,"|");
            }
            args[i] = NULL;

            int stdin_storage = 0;
            int stdout_storage = 1;

            for(int j = 0; j < i; j++){
                if(pipe(fd) == -1){
                    perror("ERROR");
                    exit(EXIT_FAILURE);
                }

                int pid = fork();
                if(pid < 0){
                    perror("ERROR");
                    exit(EXIT_FAILURE);
                }

                if(pid == 0){
                    // Redirect input and output for the child process
                    if((stdin_storage = dup(STDIN_FILENO)) == -1){
                        perror("ERROR");
                        exit(EXIT_FAILURE);
                    }

                    if(dup2(tmp, 0) == -1){
                        perror("ERROR");
                        exit(EXIT_FAILURE);
                    }

                    if(j != i-1){
                        stdout_storage = dup(STDOUT_FILENO);
                        if(stdout_storage == -1){
                            perror("ERROR");
                            exit(EXIT_FAILURE);
                        }

                        if(dup2(fd[1], 1) == -1){
                            perror("ERROR");
                            exit(EXIT_FAILURE);
                        }
                    }

                    if(close(fd[0]) == -1){
                        perror("ERROR");
                        exit(EXIT_FAILURE);
                    }

                    char arg3[50];
                    char *token1;
                    token1 = strtok(args[j], " ");
                    strcpy(command, token1);
                    token1 = strtok(NULL, "");
                    if(token1 != NULL) {
                        strcpy(arg3, token1);
                    }
                    else {
                        strcpy(arg3, "");
                    }

                    char* arg2[50];
                    token1 = strtok(arg3," ");
                    arg2[0] = args[j];
                    int i = 1;
                    while(token1 != NULL){
                        arg2[i] = token1;
                        i++;
                        token1 = strtok(NULL," ");
                    }
                    arg2[i] = NULL;
                    // Execute the command in the child process
                    if(execvp(args[j],arg2) == -1){
                        perror("ERROR");
                        exit(EXIT_FAILURE);
                    }

                    printf("Error: Execvp failed!");
                    exit(EXIT_FAILURE);
                }
                else{
                    int ret;
                    int pid = wait(&ret);

                    if(!WIFEXITED(ret)) {
                        printf("Abnormal termination of %d\n",pid);
                        exit(EXIT_FAILURE);
                    }

                    if(close(fd[1]) == -1){
                        perror("ERROR");
                        exit(EXIT_FAILURE);
                    }

                    tmp = fd[0];
                }
            }
            // Restore standard input and output
            if(dup2(stdin_storage, STDIN_FILENO) == -1){
                perror("ERROR");
                exit(1);
            }
            if(dup2(stdout_storage,STDOUT_FILENO) == -1){
                perror("ERROR");
                exit(1);
            }

            free(str2);

            exit(0);
        }
        //to check if the user has entered history or not and if yes to call the history()
        if(!strcmp(command,"history")){
            char temp[100];
            trim(arg,temp);
            if(strlen(temp) > 0){
                printf("Too many args!");
                exit(1);
            }
            history();
            exit(0);
        }
        // Allocate memory for a command string with 'which' for checking command existence
        char* main_str = (char*)malloc(100);
        if(main_str == NULL){
            printf("Malloc failed!");
            exit(EXIT_FAILURE);
        }

        char* str1 = "which ";
        char* str2 = " > /dev/null 2>.1";
        int curr_index = 0;
        for(int i = 0; i < (int)strlen(str1); i++){
            curr_index++;
            main_str[i] = str1[i];
        }
        for(int i = 0; i < (int)strlen(command); i++){
            main_str[curr_index] = command[i];
            curr_index++;
        }
        for(int i = 0; i < (int)strlen(str2); i++){
            main_str[curr_index] = str2[i];
            curr_index++;
        }
        // Use the 'system' function to check if the command exists
        int val = system(main_str);
        if(val){
            printf("Command: \"%s\" not found.\n",command); 
            exit(0);
        }
        else if(val == -1){
            perror("ERROR");
            exit(EXIT_FAILURE);
        }
        else{
            // Execute the user-entered command
            char* args[50];
            char *token = strtok(arg," ");
            args[0] = command;
            int i =1;
            while(token != NULL){
                args[i] = token;
                i++;
                token = strtok(NULL," ");
            }
            args[i] = NULL;
            if(execvp(command,args) == -1){
                perror("ERROR");
                exit(EXIT_FAILURE);
            }

            printf("Exec Failed!\n");
            exit(EXIT_FAILURE);
        }

        free(main_str);
        
        exit(0);
    }

    else{
        // This code block runs in the parent process
        int ret;

        if(mode != 2 && mode != 3){
            // Wait for the child process to finish
            status = wait(&ret);
            if(!WIFEXITED(ret)) {
                printf("Abnormal termination of %d\n",status);
            }
        }

        return status;
    }
}


//stopAdd: it is a function to add the process to the list_procs(list of processes to be executed by the scheduler)
void stopAdd(Proc_Queue* queue1, pid_t pid, int priority,char* name){
    struct timespec start;
    //getting the current system time and assign it to start, to store the starting time of the process 
    if(clock_gettime(CLOCK_REALTIME, &start) == -1){
        perror("Clock_gettime");
        exit(1);
    }

    proc* p1 = (proc*)malloc(sizeof(proc));
    if(p1 == NULL){
        perror("Malloc");
        exit(1);
    }

    for(int i = 0; i < (int)strlen(name); i++){
        p1->name[i] = name[i];
    }
    p1->pid = pid;
    p1->prio = priority;
    p1->st_time = start;
    p1->total_waiting_time = 0;
    p1->execution_time = 0;
    p1->last_time = start;
    p1->killed = 0;
    p1->running = 0;

    if(sem_wait(&queue1->lock) == -1){
        perror("Sem_Wait");
        exit(1);
    }

    queue1->n_proc++;
  
    //adding it to list_procs and incrementing n_procs by one
    if(queue1->n_proc_arr[priority-1] < 20){
        queue1->list_procs[priority-1][queue1->n_proc_arr[priority-1]] = *p1;
        queue1->n_proc_arr[priority-1]++;
    }
    else{
        printf("Process_Queue Filled!\n");
        exit(1);
    }

    if(sem_post(&queue1->lock) == -1){
        perror("Sem_Post");
        exit(1);
    }

    if (kill(pid,SIGSTOP) == -1){
        perror("kill");
        return;
    }
}

//takePut: takes the process at the mentioned index and enqueues to the particular sub_array based on priority of the process
void takePut(Proc_Queue* queue1,int index,int priority){
    if(index > queue1->n_proc_arr[priority-1]){
        printf("Index Out of Bounds!\n");
        return;
    }

    proc takenProcess = queue1->list_procs[priority-1][index];

    for(int i = index+1; i < queue1->n_proc_arr[priority-1]; i++){
        queue1->list_procs[priority-1][i-1] = queue1->list_procs[priority-1][i];
    }

    queue1->list_procs[priority-1][queue1->n_proc_arr[priority-1]-1] = takenProcess;
}

//strProc: it is a function to  take all details of the process like pid,name,execution time,start time,end time,wait time,priority and returns it in a string format
char* strProc(proc* process) {
    char* processDetails = (char*)malloc(2000 * sizeof(char)); // Adjust the buffer size as needed
    if (processDetails == NULL) {
        perror("Malloc failed!");
        exit(EXIT_FAILURE);
    }

    char start_time_str[30], end_time_str[30];

    strftime(start_time_str, sizeof(start_time_str), "%H:%M:%S", localtime(&process->st_time.tv_sec));
    strftime(end_time_str, sizeof(end_time_str), "%H:%M:%S", localtime(&process->end_time.tv_sec));

    if(sprintf(processDetails, "---------------------------------------------------------------------------\n\n%d) \tNAME: %s\n\n\t\tPID: %d\n\t\tPriority: %d\n\t\tStart Time: %s.%09ld\n\t\tEnd Time: %s.%09ld\n\t\tTotal Waiting Time: %.3lf ms\n\t\tTotal Execution Time: %.3lf ms\n\n\n",queue->d_proc + 1,process->name,process->pid,process->prio,start_time_str,process->st_time.tv_nsec,end_time_str,process->end_time.tv_nsec,process->total_waiting_time,process->execution_time) < 0){
        printf("Sprintf Failed!\n");
    }

    return processDetails;
}

void Simple_Scheduler(int NCPU, int TSLICE){
    //getting the scheduler's pid
    queue->scheduler_pid = getpid();

    int temp_var;
    int prim_idx = 0;
    int temp_arr[4];

    struct timespec temp;
    long long diff_ns;
    double diff_ms;

    //main execution loop of the scheduler
    while(1){
        // Wait for a lock to access the process queue
        if(sem_wait(&queue->lock) == -1){
            perror("Sem_wait!");
            exit(1);
        }

        // Check if there are processes in the queue
        if(queue->n_proc == 0){
            if(signal(SIGINT,Escape_sequence) == SIG_ERR){
                perror("SIGINT Handler");
                exit(1);
            }

            // If no processes, release the lock and sleep briefly
            if(sem_post(&queue->lock) == -1){
                perror("Sem_Post");
                exit(1);
            }
            if(usleep(TSLICE*1000) == -1){
                perror("Usleep");
                exit(1);
            }
        }
        else{
            if(signal(SIGINT,SIG_IGN) == SIG_ERR){
                perror("SIGINT");
                exit(1);
            }
        }
        // Increment the primary index for scheduling
        prim_idx++;

        // Determine the number of processes to schedule (limited by NCPU or the number of processes in the queue)
        if(NCPU < queue->n_proc){
            temp_var = NCPU;
        }
        else{
            temp_var = queue->n_proc;
        }

        if(sem_post(&queue->lock) == -1){
            perror("Sem_Post");
            exit(1);
        }

        temp_arr[0] = 0;
        temp_arr[1] = 0;
        temp_arr[2] = 0;
        temp_arr[3] = 0;

        proc* main_proc;
        int req_list;

        if(sem_wait(&queue->lock) == -1){
            perror("Sem_Wait");
            exit(1);
        }
        
        if(prim_idx % 7 == 0){
            //Selecting list based on condition(time%7 for least priority)
            if(queue->n_proc_arr[0] != 0){
                req_list = 1;
            }
            else{
                if(queue->n_proc_arr[3] != 0){
                    req_list = 4;
                }
                else{
                    if(queue->n_proc_arr[2] != 0){
                        req_list = 3;
                    }
                    else{
                        req_list = 2;
                    }
                }
            }
        }
        else if(prim_idx % 5 == 0){
            //Selecting list based on condition(time%5 for on above the least priority)
            if(queue->n_proc_arr[1] !=0){
                req_list = 2;
            }
            else{
                if(queue->n_proc_arr[3]!= 0){
                    req_list = 4;
                }
                else{
                    if(queue->n_proc_arr[2] != 0){
                        req_list = 3;
                    }
                    else{
                        req_list = 1;
                    }
                }
            }
        }
        else if(prim_idx % 3 == 0){
            //Selecting list based on condition(time%3 for two above the least priority)
            if(queue->n_proc_arr[2] !=0){
                req_list = 3;
            }
            else{
                if(queue->n_proc_arr[3] != 0){
                    req_list = 4;
                }
                else{
                    if(queue->n_proc_arr[1]!= 0){
                        req_list = 2;
                    }
                    else{
                        req_list = 1;
                    }
                }
            }
        }
        else{
            if(queue->n_proc_arr[3] !=0){
                req_list = 4;
            }
            else{
                if(queue->n_proc_arr[2] != 0){
                    req_list = 3;
                }
                else{
                    if(queue->n_proc_arr[1] != 0){
                        req_list = 2;
                    }
                    else{
                        req_list = 1;
                    }
                }
            }
        }

        int curr_list = req_list;

        for(int i = 0; i < temp_var; i++){
            if(queue->n_proc_arr[(curr_list+3)%4] == temp_arr[(curr_list+3)%4]){
                curr_list = (curr_list + 3)%4;
                if(queue->n_proc_arr[(curr_list + 3)%4] == temp_arr[(curr_list + 3)%4]){
                    curr_list = (curr_list + 3)%4;
                    if(queue->n_proc_arr[(curr_list + 3)%4] == temp_arr[(curr_list + 3)%4]){
                        curr_list = (curr_list + 3)%4;
                    }
                }
            }
            
            // get the next process for executing
            main_proc = &queue->list_procs[(curr_list+3)%4][temp_arr[(curr_list+3)%4]];
            temp_arr[(curr_list+3)%4]++;
            
            // to record the current system time and update the process state
            if(clock_gettime(CLOCK_REALTIME, &temp) == -1){
                perror("Clock_GetTime");
                exit(1);
            }
            diff_ns = (temp.tv_sec - main_proc->last_time.tv_sec) * 1000000000LL + (temp.tv_nsec - main_proc->last_time.tv_nsec);
            diff_ms = (double)diff_ns / 1000000.0;
            main_proc->total_waiting_time += diff_ms;
            main_proc->last_time = temp;
            main_proc->running = 1;
            
            //sending a continue signal to start the process execution
            if(kill(main_proc->pid,SIGCONT) == -1){
                perror("kill");
                exit(1);
            }
        }

        if(sem_post(&queue->lock) == -1){
            perror("Sem_Post");
            exit(1);
        }

        if(usleep(TSLICE*1000) == -1){
            perror("Usleep");
            exit(1);
        }

        if(sem_wait(&queue->lock) == -1){
            perror("Sem_Wait");
            exit(1);
        }

        //to check if the are any processes in the process queue
        if(queue->n_proc == 0){
            if(sem_post(&queue->lock) == -1){
                perror("Sem_Post");
                exit(1);
            }
            continue;
        }

        if(NCPU < queue->n_proc){
            temp_var = NCPU;
        }
        else{
            temp_var = queue->n_proc;
        }

        if(sem_post(&queue->lock) == -1){
            perror("Sem_Post");
            exit(1);
        }
        if(sem_wait(&queue->lock) == -1){
            perror("Sem_Wait");
            exit(1);
        }
        
        //suspending the current running process 
        for(int i = 3; i >= 0; i--){
            for(int j = 0; j < temp_arr[i]; j++){
                if(queue->list_procs[i][j].killed == 0 && queue->list_procs[i][j].running == 1){
                    if(kill(queue->list_procs[i][j].pid,SIGSTOP) == -1){
                        continue;
                    }
                    
                    //calculating the execution time,updating the last time and changing the runnning flag to zero before the process gets kicked out of the cpu
                    if(clock_gettime(CLOCK_REALTIME, &temp) == -1){
                        perror("Clock_GetTime");
                        exit(1);
                    }
                    diff_ns = (temp.tv_sec - queue->list_procs[i][j].last_time.tv_sec) * 1000000000LL + (temp.tv_nsec - queue->list_procs[i][j].last_time.tv_nsec);
                    diff_ms = (double)diff_ns / 1000000.0;
                    queue->list_procs[i][j].last_time = temp;
                    queue->list_procs[i][j].running = 0;
                }
            }
        }

        //adding the process to the end of the process queue.
        for(int i = 3; i >= 0; i--){
            for(int j = 0; j < temp_arr[i]; j++){
                takePut(queue,0,i+1);
            }
        }

        if(sem_post(&queue->lock) == -1){
            perror("Sem_Post");
            exit(1);
        }
    }

    if(munmap(queue,sizeof(Proc_Queue)) == -1){
        perror("Munmap");
        exit(1);
    }
    if(close(fd_shm) == -1){
        perror("Close");
        exit(1);
    }

    exit(0);
}

//Simple_Shell: it is the main execution loop function where the shell runs in an infinite loop and prompts the user for input and gives out output of neccessary format
void Simple_Shell(int NCPU, int TSLICE){
    int status = 1;
    char input[100];
    char command[30];
    char arg[50];

    //signal handling for ctrl+C
    if(signal(SIGINT,Escape_sequence) == SIG_ERR){
        perror("ERROR");
        exit(1);
    }

    if(signal(SIGCHLD,Escape_sequence) == SIG_ERR){
        perror("ERROR");
        exit(1);
    }

    if(signal(SIGQUIT,Escape_sequence) == SIG_ERR){
        perror("ERROR");
        exit(1);
    }

    shm_unlink(text);

    //initializing the shared memory
    fd_shm = shm_open(text,O_CREAT | O_EXCL | O_RDWR, 0777);
    if(fd_shm == -1){
        perror("Shm_Open");
        exit(1);
    }
    
    if(ftruncate(fd_shm,sizeof(Proc_Queue)) == -1){
        perror("ftruncate");
        exit(1);
    }

    //initializing the process_queue
    queue = (Proc_Queue*)mmap(NULL,sizeof(Proc_Queue),PROT_READ | PROT_WRITE | PROT_EXEC,MAP_SHARED | MAP_ANONYMOUS,fd_shm,0);
    
    if((void*)queue == (void*)-1){
        perror("mmap");
        exit(1);
    }

    queue->n_proc = 0;
    queue->n_proc_arr[0] = 0;
    queue->n_proc_arr[1] = 0;
    queue->n_proc_arr[2] = 0;
    queue->n_proc_arr[3] = 0;

    queue->n_del_proc_arr[0] = 0;
    queue->n_del_proc_arr[1] = 0;
    queue->n_del_proc_arr[2] = 0;
    queue->n_del_proc_arr[3] = 0;

    queue->d_proc = 0;
    queue->e_proc = 0;
    queue->sig_int_flag = 0;

    queue->shell_pid = getpid();

    if(sem_init(&queue->lock,1,1) == -1){
        perror("Sem_Init");
        exit(1);
    }

    int status3 = fork();
    if(status3 < 0){
        printf("Fork Failure\n");
        exit(1);
    }
    else if(status3 == 0){
        int status2 = fork();
        if(status2 < 0){
            printf("Fork Failure\n");
            exit(1);
        }
        else if (status2 > 0){
            //getting the schedulers parent pid
            queue->scheduler_parent_pid = getpid();

            if(signal(SIGINT,SIG_IGN) == SIG_ERR){
                perror("ERROR");
                exit(1);
            }

            int rand2;
            int randpid = wait(&rand2);
            if(!WEXITSTATUS(rand2)){
                printf("Abnormal Termination of %d\n",randpid);
                exit(1);
            }

            if(munmap(queue,sizeof(Proc_Queue)) == -1){
                perror("Munmap");
                exit(1);
            }
            if(close(fd_shm) == -1){
                perror("Close");
                exit(1);
            }

            exit(0);
        }
        else{
            Simple_Scheduler(NCPU,TSLICE);
        }
    }

    if(signal(SIGINT,SIG_IGN) == SIG_ERR){
        perror("SIGINT");
        exit(1);
    }

    //main shell loop
    do{
        // Get the current working directory

        char cwd[PATH_MAX];
        if(getcwd(cwd,sizeof(cwd)) == NULL){
            perror("ERROR");
            exit(1);
        }

        // Display the shell prompt with the current directory
        magenta("SimpleShell");
        white("$ ");

        // Read user input from the standard input
        if(fgets(input,100,stdin) == NULL){
            printf("fgets has failed or there is nothing to input anymore!");
            exit(1);
        }
        // Remove the newline character from the input
        input[strcspn(input,"\n")] = 0;

        // If the input is empty, continue to the next iteration
        if (!strcmp(input,"")){continue;}

        // Store the input in a history array
        strncpy(user_input[curr_idx], input, 80); 

        char test_input[100];
        int flag_bg_detect = 0;
        // Check if the input command is meant to run from a file
        if(lastBack(input) == 1){
            // Remove the '&' character to detect background execution
            remAmp(input,test_input);
            flag_bg_detect = 1;
            strcpy(input,test_input);
        }
        // If there are no pipe characters ('|') in the input, parse the command and argument
        if(strstr(input,"|")==NULL){
            char *token;
            token = strtok(input, " ");
            strcpy(command, token);
            token = strtok(NULL, "");
            if(token != NULL) {
                strcpy(arg, token);
            }
            else {
                strcpy(arg, "");
            }

            // Check if the command is 'run' to execute a file
            if(!strcmp(command,"run")){
                FILE *fptr;

                fptr = fopen(arg, "r");
                if(fptr == NULL){
                    perror("ERROR");
                    continue;
                }

                long cursor;

                while(fgets(input, 100, fptr)) {
                    cursor = ftell(fptr);
                    if(cursor == -1){
                        printf("ftell failed!\n");
                        exit(1);
                    }

                    input[strcspn(input,"\n")] = 0;

                    if(!strcmp(input,"") || !strcmp(input,"\n")){
                        continue;
                    }

                    strncpy(user_input[curr_idx], input, 80); 

                    if(lastBack(input) == 1){
                        remAmp(input,test_input);
                        flag_bg_detect = 1;
                        strcpy(input,test_input);
                    }

                    curr_idx++;
                    // If there are no pipe characters ('|') in the input, parse the command and argument
                    if(strstr(input,"|")==NULL){
                        char *token;
                        token = strtok(input, " ");
                        strcpy(command, token);
                        token = strtok(NULL, "");
                        if(token != NULL) {
                            strcpy(arg, token);
                        }
                        else {
                            strcpy(arg, "");
                        }

                        if(flag_bg_detect == 1){
                            status = launch(command,arg,2);
                        }
                        else{
                            status = launch(command,arg,1);
                        }
                    }

                    else{
                        if(flag_bg_detect == 1){
                            // Execute a command with pipes in the background
                            status = launch(input,arg,3);

                        }
                        else{
                            // Execute a command with pipes in the foreground
                            status = launch(input,arg,0);
                        }
                    }

                    if(fseek(fptr,cursor,SEEK_SET) == -1){
                        perror("ERROR");
                        exit(1);
                    }
                    
                }
            
                fclose(fptr);
            }

            else{
                curr_idx++;
                if(flag_bg_detect == 1){
                    status = launch(command,arg,2);
                }
                else{
                    //to check if the user entered submit 
                    if(!strcmp(command,"submit")){
                        char temp[100];
                        char temp2[100];
                        char* arr_args[100];
                        int n_args = 1;
                        //trimming all trailing spaces
                        trim(arg,temp);
                        if(forward_trim(temp,temp2) == NULL){
                            printf("Incorrect number of arguments to 'submit', has to be at least 1 and max 2!\n");
                            continue;
                        }
                        char* temp3 = strtok(temp2," ");
                        arr_args[0] = temp3;
                        while(temp3 != NULL) {
                            temp3 = strtok(NULL," ");
                            arr_args[n_args] = temp3;
                            n_args++;
                        }

                        n_args--;

                        if(n_args > 2){
                            printf("Too many args to 'Submit'!\n");
                            continue;
                        }

                        if (access(arr_args[0], F_OK|X_OK) != 0){
                            printf("Not an executable or Executable doesn't exist!\n");
                            continue;
                        }

                        if(n_args == 2){
                            int prio = atoi(arr_args[1]);
                            if(prio <= 0){
                                printf("Invalid value of Priority!\n");
                                continue;
                            }

                            if(prio < 1 || prio > 4){
                                printf("Priority can only be between 1 and 4!\n");
                                continue;
                            }

                            int flag = 0;
                            int len = strlen(arr_args[1]);
                            for(int i = 0; i < len; i++){
                                if(arr_args[1][i] == '.'){
                                    printf("Priority can't be a float!\n");
                                    flag = 1;
                                    break;
                                }
                            }

                            if(flag == 1){
                                continue;
                            }
                        }

                        int new_proc_status = fork();
                        if(new_proc_status < 0){
                            printf("Fork Failure.\n");
                            continue;
                        }
                        else if (new_proc_status == 0){
                            int status3 = fork();
                            if(status3 < 0){
                                printf("Fork Failure.\n");
                                continue;
                            }
                            else if(status3 > 0){
                                if(signal(SIGINT,SIG_IGN) == SIG_ERR){
                                    perror("ERROR");
                                    exit(1);
                                }

                                int rand;
                                int pid = wait(&rand);
                                if(!WIFEXITED(rand)){
                                    printf("Abnormal Termination of %d\n",pid);
                                    exit(1);
                                }

                                if(sem_wait(&queue->lock) == -1){
                                    perror("Sem_Wait");
                                    exit(1);
                                }

                                //find the process that has 
                                //just terminated after calling exec and figure out a way to delete it effectively
                                int found = 0;
                                struct timespec temp;
                                long long diff_ns;
                                double diff_ms;

                                int prio;

                                if(n_args == 1){
                                    prio = 1;
                                }
                                if(n_args == 2){
                                    prio = atoi(arr_args[1]);
                                }

                                int len = queue->n_proc_arr[prio-1];

                                for (int i = 0; i < len; i++) {
                                    if (queue->list_procs[prio-1][i].pid == pid){
                                        // Found the process, remove it by shifting the remaining processes
                                        if(clock_gettime(CLOCK_REALTIME, &temp) == -1){
                                            perror("Clock_gettime");
                                            exit(1);
                                        }
                                        diff_ns = (temp.tv_sec - queue->list_procs[prio-1][i].st_time.tv_sec) * 1000000000LL + (temp.tv_nsec - queue->list_procs[prio-1][i].st_time.tv_nsec);
                                        diff_ms = (double)diff_ns / 1000000.0;

                                        queue->list_procs[prio-1][i].end_time = temp;
                                        queue->list_procs[prio-1][i].last_time = temp;
                                        queue->list_procs[prio-1][i].execution_time += diff_ms;
                                        queue->list_procs[prio-1][i].killed = 1;

                                        queue->avg_wait_list[prio-1] = ((queue->avg_wait_list[prio-1] * (queue->n_del_proc_arr[prio-1])) + queue->list_procs[prio-1][i].total_waiting_time) / (queue->n_del_proc_arr[prio-1] + 1);

                                        char * temp_str = (char*)malloc(20*sizeof(char));
                                        if(sprintf(temp_str,"%f",queue->avg_wait_list[prio-1]) < 0){
                                            printf("Sprintf Failed!\n");
                                            exit(1);
                                        }

                                        strcpy(queue->wait_str_list[prio-1],temp_str);
                                        free(temp_str);

                                        //adding the process details to exit_Sequence
                                        char* processDetails = strProc(&queue->list_procs[prio-1][i]);
                                        strcpy(queue->exit_Sequence[queue->e_proc], processDetails);
                                        queue->e_proc++;
                                        free(processDetails);

                                        //adding the process to the deleted array
                                        queue->list_del[queue->d_proc] = queue->list_procs[prio-1][i];
                                        for (int j = i; j < len - 1; j++) {
                                            queue->list_procs[prio-1][j] = queue->list_procs[prio-1][j + 1];
                                        }

                                        //incrementing the number of deleted processes by one
                                        queue->n_del_proc_arr[prio-1]++;
                                        queue->d_proc++;
                                        found = 1;
                                        break;
                                    }
                                }

                                //adding on basis of priority
                                queue->n_proc_arr[prio-1]--;
                                //decreasing the number of processes by one
                                queue->n_proc--;

                                if(!found){
                                    printf("PID not found");
                                }
                                if(sem_post(&queue->lock) == -1){
                                    perror("Sem_Post");
                                    exit(1);
                                }
                                if(munmap(queue,sizeof(Proc_Queue)) == -1){
                                    perror("Munmap");
                                    exit(1);
                                }
                                if(close(fd_shm) == -1){
                                    perror("Close");
                                    exit(1);
                                }

                                if(kill(getpid(),SIGTERM) == -1){
                                    perror("kill");
                                    exit(1);
                                }
                            }
                            else{
                                //creating a function for it
                                if(n_args == 1){
                                    stopAdd(queue,getpid(),1,arr_args[0]);
                                }
                                if(n_args == 2){
                                    stopAdd(queue,getpid(),atoi(arr_args[1]),arr_args[0]);
                                }

                                char* temp_arr[2];
                                temp_arr[0] = arr_args[0];
                                temp_arr[1] = NULL;

                                if(munmap(queue,sizeof(Proc_Queue)) == -1){
                                    perror("Munmap");
                                    exit(1);
                                }
                                if(close(fd_shm) == -1){
                                    perror("Close");
                                    exit(1);
                                }

                                execvp(arr_args[0],temp_arr);

                                printf("Execvp Failed!\n");
                                exit(1);
                            }
                        }
                    }
                    else{
                        status = launch(command,arg,1);
                    }
                }
            }
        }
        else{
            curr_idx++;
            if(flag_bg_detect == 1){
                status = launch(input,arg,3);
            }
            else{
                status = launch(input,arg,0);
            }
        }
    }
    while(status != 0);
}