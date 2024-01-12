#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Colors/color.h"
#include <time.h>
#include <sys/time.h>
#include <features.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

void Simple_Shell(int NCPU, int TSLICE);