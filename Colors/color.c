#include "color.h"

/*For colors we are goin to use the ANSI escape code
Black:30
Red:31
Green:32
Yellow:33
Blue:34
Magenta:35
Cyan:36
White:37
Reset:0
*/
//Here each function would print the text  provided in the specified color

void black(char* s){
    //black=30
    printf("\033[0;30m");
    printf("%s",s);
    printf("\033[0m");
}

void red(char* s){
    //red=31
    printf("\033[0;31m");
    printf("%s",s);
    printf("\033[0m");
}

void green(char* s){
    //green=32
    printf("\033[0;32m");
    printf("%s",s);
    printf("\033[0m");
}

void yellow(char* s){
    //yellow=33
    printf("\033[0;33m");
    printf("%s",s);
    printf("\033[0m");
}

void blue(char* s){
    //blue=34
    printf("\033[0;34m");
    printf("%s",s);
    printf("\033[0m");
}

void magenta(char* s){
    //magenta=35
    printf("\033[0;35m");
    printf("%s",s);
    printf("\033[0m");
}

void cyan(char* s){
    //cyan=36
    printf("\033[0;36m");
    printf("%s",s);
    printf("\033[0m");
}

void white(char* s){
    //white=37
    printf("\033[0;37m");
    printf("%s",s);
    printf("\033[0m");
}