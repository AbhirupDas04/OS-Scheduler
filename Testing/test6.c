#include <stdio.h>

int fib(int n){
    if(n == 0 || n==1){
        return n;
    }
    else{
        return fib(n-1) + fib(n-2);
    }
}

void main(){
    for(int i = 0; i < 15; i++){
        fib(41);
        printf("%d TESTING...\n", i);
    }
}