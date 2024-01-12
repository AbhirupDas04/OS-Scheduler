#include <stdio.h>
#include <stdlib.h>
#include "Shellcommands/shell.h" 

int main(int argc, char** argv){ 
    if(argc!=3){
        printf("Incorrect number of arguments!\n");
        exit(1);
    }

    int NCPU = atoi(argv[1]);
    if(NCPU <= 0){
        printf("Wrong value of NCPU.\n");
        exit(1);
    }

    int TSLICE = atoi(argv[2]);
    if(TSLICE <= 0){
        printf("Wrong value of Time Slice.\n");
        exit(1);
    }

    int len_ncpu = strlen(argv[1]);
    int len_tslice = strlen(argv[2]);

    for(int i = 0; i < len_ncpu; i++){
        if(argv[1][i] == '.'){
            printf("NCPU can't be a float!\n");
            exit(1);
        }
    }

    for(int i = 0; i < len_tslice; i++){
        if(argv[2][i] == '.'){
            printf("Tslice will accept only integer values in our implementation!\n");
            exit(1);
        }
    }

    Simple_Shell(NCPU,TSLICE);
    
    return 0; 
}