# Asssignment-3
: SimpleScheduler
##### Abhirup Das | Armaan Singh
## File Structure

```
Assignment-3
|    
|---Colors
|    |---color.c
|    |---color.h
|    |---Makefile
|
|---Shellcommands
|    |---simple-shell.c
|    |---shell.h
|    |---Makefile
|
|---Testing
|    |---test2.c
|    |---test3.c
|    |---test4.c
|    |---test5.c
|    |---test6.c
|    |---testbench.c
|    |---a.out
|    |---test2
|    |---test3
|    |---test4
|    |---test5
|    |---test6
|
|---dummy_main.h
|---main.c
|---Makefile

```
## Code_Explanantion
### Shellcommmands
#### simple-shell.c
```
This code implements a process scheduler within a shell program. The scheduler is responsible for managing the execution of processes with different priorities. It maintains a process queue, where processes are organized based on their priority. The scheduler uses a shared memory segment to store information about the processes, such as their PID, priority, execution time, and waiting time.

The scheduler handles the creation of new processes using the fork() system call and manages their execution using the execvp() system call. It also tracks the start and end times of each process, calculates their execution time, and updates the waiting time for processes that are waiting to be executed.

When a process is terminated, the scheduler removes it from the queue, updates the statistics, and adds the process details to an exit sequence array. The scheduler also handles signal handling, specifically the Ctrl+C signal (SIGINT), by printing out the details of executed processes and terminating the shell program.
```
####  shell.h
```
contains the necessary imports for the shell and scheduler to run smoothly.
```


### main.c
```
contains the main execution of the shell, once make has successfully ran qwe have to run ./bin/main and that would initiate the shell code
```
### Makefile (main)
```
This Makefile is used to compile and link shell.c, color.c, and main.c into the target binary file (bin/main) using the specified compiler and flags. The clean target is provided to remove the build artifacts.
```
### Colors
#### color.c
```
This c file is just for printing in different colours 
```
### Testing
#### test2.c
```
contains just a print statement for testing purposes
```
#### test3.c
```
contains an infinite loop with a print statement in it
```
#### test4.c
```
contains an infinte loop with a print statement in it
```
#### test5.c
```
contains code for a recursive fibonacci code set at 45
```
#### test6.c
```
contains code for printing a value 15 times at a delay
```

## Some execution comparisions
#### (b/w priority and non-priority based schedulers)

### 1)
#### 1.1 running 8 same process with 1 cpus and 2000 ms time slice
```
---------------------------------------------------------------------------

1) 	NAME: ./Testing/test6

		PID: 3420
		Priority: 1
		Start Time: 01:01:44.985016580
		End Time: 01:04:19.398520103
		Total Waiting Time: 132872.461 ms
		Total Execution Time: 154413.504 ms


---------------------------------------------------------------------------

2) 	NAME: ./Testing/test6

		PID: 3423
		Priority: 1
		Start Time: 01:01:45.679946500
		End Time: 01:04:24.543883496
		Total Waiting Time: 136597.424 ms
		Total Execution Time: 158863.937 ms


---------------------------------------------------------------------------

3) 	NAME: ./Testing/test6

		PID: 3425
		Priority: 1
		Start Time: 01:01:46.558414603
		End Time: 01:04:30.379488393
		Total Waiting Time: 141950.181 ms
		Total Execution Time: 163821.074 ms


---------------------------------------------------------------------------

4) 	NAME: ./Testing/test6

		PID: 3429
		Priority: 1
		Start Time: 01:01:48.330959759
		End Time: 01:04:32.292836763
		Total Waiting Time: 141549.825 ms
		Total Execution Time: 163961.877 ms


---------------------------------------------------------------------------

5) 	NAME: ./Testing/test6

		PID: 3433
		Priority: 1
		Start Time: 01:01:50.111585799
		End Time: 01:04:34.812788867
		Total Waiting Time: 142471.204 ms
		Total Execution Time: 164701.203 ms


---------------------------------------------------------------------------

6) 	NAME: ./Testing/test6

		PID: 3435
		Priority: 1
		Start Time: 01:01:50.926251722
		End Time: 01:04:40.648287277
		Total Waiting Time: 147830.614 ms
		Total Execution Time: 169722.036 ms


---------------------------------------------------------------------------

7) 	NAME: ./Testing/test6

		PID: 3431
		Priority: 1
		Start Time: 01:01:49.230278556
		End Time: 01:04:42.548837526
		Total Waiting Time: 151363.368 ms
		Total Execution Time: 173318.559 ms


---------------------------------------------------------------------------

8) 	NAME: ./Testing/test6

		PID: 3427

		Priority: 1
		Start Time: 01:01:47.427011564
		End Time: 01:04:45.431076273
		Total Waiting Time: 155719.469 ms
		Total Execution Time: 178004.065 ms
```
#### 1.2 running 8 same processes with 8 cpus and time slice as 2000ms
```
---------------------------------------------------------------------------

1) 	NAME: ./Testing/test6

		PID: 4501
		Priority: 1
		Start Time: 01:12:34.484043712
		End Time: 01:13:13.781511052
		Total Waiting Time: 2000.948 ms
		Total Execution Time: 39297.467 ms


---------------------------------------------------------------------------

2) 	NAME: ./Testing/test6

		PID: 4505
		Priority: 1
		Start Time: 01:12:36.162209000
		End Time: 01:13:14.389537019
		Total Waiting Time: 322.834 ms
		Total Execution Time: 38227.328 ms


---------------------------------------------------------------------------

3) 	NAME: ./Testing/test6

		PID: 4503
		Priority: 1
		Start Time: 01:12:35.307119078
		End Time: 01:13:14.391605750
		Total Waiting Time: 1177.931 ms
		Total Execution Time: 39084.487 ms


---------------------------------------------------------------------------

4) 	NAME: ./Testing/test6

		PID: 4509
		Priority: 1
		Start Time: 01:12:38.191332811
		End Time: 01:13:16.431537914
		Total Waiting Time: 300.710 ms
		Total Execution Time: 38240.205 ms


---------------------------------------------------------------------------

5) 	NAME: ./Testing/test6

		PID: 4507
		Priority: 1
		Start Time: 01:12:37.007416869
		End Time: 01:13:17.000314960
		Total Waiting Time: 1484.659 ms
		Total Execution Time: 39992.898 ms


---------------------------------------------------------------------------

6) 	NAME: ./Testing/test6

		PID: 4511
		Priority: 1
		Start Time: 01:12:39.008433053
		End Time: 01:13:17.798002290
		Total Waiting Time: 1485.299 ms
		Total Execution Time: 38789.569 ms


---------------------------------------------------------------------------

7) 	NAME: ./Testing/test6

		PID: 4513
		Priority: 1
		Start Time: 01:12:39.808587737
		End Time: 01:13:17.932690487
		Total Waiting Time: 685.097 ms
		Total Execution Time: 38124.103 ms


---------------------------------------------------------------------------

8) 	NAME: ./Testing/test6

		PID: 4515
		Priority: 1
		Start Time: 01:12:40.717497957
		End Time: 01:13:19.563288824
		Total Waiting Time: 1776.993 ms
		Total Execution Time: 38845.791 ms
``` 
with the presence of multiple cpus the execution and wait times drastically reduce showing that the code those run with ncpus
### Conclusion drawn from test 1
```
Total execution time:
    case1:
        Total Execution Time for all 8 processes: 1,292,682.198 ms
        Avg. Execution Time for all 8 processes: 164,268.767 ms
    
    case2:
        Total Execution Time for all 8 processes: 312,569.318 ms
        Avg. Execution Time for all 8 processes:  38,997.745 ms


% change in execution time for both the cases:
    formula: ((Avg. Execution Time in Case 2 - Avg. Execution Time in Case 1) / Avg. Execution Time in Case 1) * 100

    value: -76.23%(neg value showcasing a decrease in execution time)

Total wait time:
    case1:
        Total Waiting Time for all 8 processes: 1,109,394.151 ms
        Avg. Waiting Time for all 8 processes: 142,267.997 ms

    case2:
        Total Waiting Time for all 8 processes: 6,853.581 ms
        Avg. Waiting Time for all 8 processes:  1,112.755 ms

% change in waiting time for both the cases:
    formula:((Avg. Waiting Time in Case 2 -Avg. Waiting Time in Case 1) / Avg. Waiting Time in Case 1) * 100

    value:-92.16%(negative value showing decrease in wait time)
```




### 2)
##### checking with priority and non priority

#### 2.1 running 8 same processes in 2 CPUs and time slice as 2000
```
---------------------------------------------------------------------------

1)      NAME: ../Testing/test6

                PID: 1124
                Priority: 1
                Start Time: 01:33:56.590306104
                End Time: 01:38:00.498822060
                Total Waiting Time: 180173.586 ms
                Total Execution Time: 243908.516 ms


---------------------------------------------------------------------------

2)      NAME: ../Testing/test6

                PID: 1129
                Priority: 1
                Start Time: 01:33:58.195158241
                End Time: 01:38:04.092195240
                Total Waiting Time: 182587.070 ms
                Total Execution Time: 245897.037 ms


---------------------------------------------------------------------------

3)      NAME: ../Testing/test6

                PID: 1137
                Priority: 1
                Start Time: 01:33:59.971425433
                End Time: 01:38:08.276825368
                Total Waiting Time: 184820.985 ms
                Total Execution Time: 248305.400 ms


---------------------------------------------------------------------------

4)      NAME: ../Testing/test6

                PID: 1141
                Priority: 1
                Start Time: 01:34:01.630745692
                End Time: 01:38:09.881021723
                Total Waiting Time: 185133.495 ms
                Total Execution Time: 248250.276 ms


---------------------------------------------------------------------------

5)      NAME: ../Testing/test6

                PID: 1127
                Priority: 1
                Start Time: 01:33:57.381991270
                End Time: 01:38:10.874809058
                Total Waiting Time: 189410.950 ms
                Total Execution Time: 253492.818 ms


---------------------------------------------------------------------------

6)      NAME: ../Testing/test6

                PID: 1143
                Priority: 1
                Start Time: 01:34:03.168602817
                End Time: 01:38:10.876915358
                Total Waiting Time: 185646.812 ms
                Total Execution Time: 247708.313 ms


---------------------------------------------------------------------------

7)      NAME: ../Testing/test6

                PID: 1131
                Priority: 1
                Start Time: 01:33:59.125680399
                End Time: 01:38:13.500400349
                Total Waiting Time: 191689.707 ms
                Total Execution Time: 254374.720 ms


---------------------------------------------------------------------------

8)      NAME: ../Testing/test6

                PID: 1139
                Priority: 1
                Start Time: 01:34:00.807796764
                End Time: 01:38:13.505408849
                Total Waiting Time: 189975.429 ms
                Total Execution Time: 252697.612 ms
```
#### 2.2 running 8 same processes(two of each priority) in 2 CPUs and time slice as 2000
```
---------------------------------------------------------------------------

1)      NAME: ../Testing/test6

                PID: 1039
                Priority: 4
                Start Time: 01:28:51.748956552
                End Time: 01:30:53.508358910
                Total Waiting Time: 64080.721 ms
                Total Execution Time: 121759.402 ms


---------------------------------------------------------------------------

2)      NAME: ../Testing/test6

                PID: 1043
                Priority: 4
                Start Time: 01:28:52.771916770
                End Time: 01:30:58.258136035
                Total Waiting Time: 67056.689 ms
                Total Execution Time: 125486.219 ms


---------------------------------------------------------------------------

3)      NAME: ../Testing/test6

                PID: 1045
                Priority: 3
                Start Time: 01:28:54.951010367
                End Time: 01:31:44.586913815
                Total Waiting Time: 108895.157 ms
                Total Execution Time: 169635.903 ms


---------------------------------------------------------------------------

4)      NAME: ../Testing/test6

                PID: 1047
                Priority: 3
                Start Time: 01:28:56.860096981
                End Time: 01:31:46.373840828
                Total Waiting Time: 108985.721 ms
                Total Execution Time: 169513.744 ms


---------------------------------------------------------------------------

5)      NAME: ../Testing/test6

                PID: 1051
                Priority: 2
                Start Time: 01:28:59.970828604
                End Time: 01:32:19.156827023
                Total Waiting Time: 141892.159 ms
                Total Execution Time: 199185.998 ms


---------------------------------------------------------------------------

6)      NAME: ../Testing/test6

                PID: 1049
                Priority: 2
                Start Time: 01:28:58.387970393
                End Time: 01:32:19.668838220
                Total Waiting Time: 143477.302 ms
                Total Execution Time: 201280.868 ms


---------------------------------------------------------------------------

7)      NAME: ../Testing/test6

                PID: 1055
                Priority: 1
                Start Time: 01:29:02.796831971
                End Time: 01:32:47.244771031
                Total Waiting Time: 169086.762 ms
                Total Execution Time: 224447.939 ms


---------------------------------------------------------------------------

8)      NAME: ../Testing/test6

                PID: 1053
                Priority: 1
                Start Time: 01:29:01.520440516
                End Time: 01:32:47.681899334
                Total Waiting Time: 170363.023 ms
                Total Execution Time: 226161.459 ms
```
### Conclusion drawn from test 2
```
Execution Time:
    case 1:
        Total Execution Time for all 8 processes: 1,279,570.571 ms
        Average Execution Time for all 8 processes: 159,946.321 ms

    case2:
        Total Execution Time for all 8 processes: 1,468,368.591 ms
        Average Execution Time for all 8 processes: 183,546.074 ms

Percentage Change in Execution Time:

    ((183,546.074 - 164,268.767) / 164,268.767) * 100
    = ((183,546.074 - 164,268.767) / 164,268.767) * 100
    ≈ 11.75%

Waiting Time:
    case 1:
        Total Waiting Time for all 8 processes: 1,141,652.175 ms
        Average Waiting Time for all 8 processes: 142,706.522 ms

    case2:
        Total Waiting Time for all 8 processes: 1,000.948 ms
        Average Waiting Time per process =  121,729.69175 ms

Percentage Change in Waiting Time:
    ((142,706.522 - 121,729.69175) / 121,729.69175) * 100
    = ((142,706.522 - 121,729.69175) / 121,729.69175) * 100
    ≈ 17.23%

Number of time slices allocated to each priority in case 2.2:
	Priority_4: 13/30
	Priority_3: 7/30
	Priority_2: 6/30
	Priority_1: 4/30
```
## Limitation of our program
```
Our priority count doesnt work well with high CPU counts and it is better with low CPU counts.
```
e
