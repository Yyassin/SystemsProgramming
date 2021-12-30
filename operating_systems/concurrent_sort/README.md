# Concurrent Sorting
> Yousef Yassin

## Contents
1. Compiling and Running 
2. Pseudocode
3. Discussion: How the algorithm solves the problem
4. Discussion: Test Results

## Compiling and Running
Once the files have been downloaded to your local machine, follow the instructions
below to run the project:

- Open a terminal and navigate to the directory with the project
source files and dependencies.
```
    $ cd ~/concurrent_sort
```

- Run the "make" command inside the project directory to compile.
```
    $ make
```

- Following the compilation, the project executable (CSORT) will be added to the project
directory. Run it with the following command:
```
    $ ./CSORT
```

- Once execution begin, the program will prompt the user for five distinct integers. At 
this stage, different test cases can be used to ensure correct implementation of the program. 

- Enjoy!


## Pseudocode

```
    create and init semaphores
    create and init shared memory
    create (fork) 3 child processes
    for each process p:
        do
            if (p isn't finished sorting this iteration)
                - Perform bubble sort on the assigned range of indices. If element(1-indexed) 3
                or 5 is to be accessed (read write), wait on corresponding semaphore.
                - Check if elements should be swapped.
                - When a swap occurs, reset all processes to unfinished state. No semaphore
                needed since this is checked repeatedly in a while loop. If a process swap occurs, the reset 
                to all processes happens first before that process sleeps. We're sure if all are finished, then all of them
                have finished sorting.
                - If in debug mode, print additional swapping status info.
                - If swap occured and we waited on a semaphore, perform signal on corresponding semaphore.
                - When no swaps occur on an given iteration, mark the process p as finished
                for this iteration.
        while (not all processes are finished)

        exit process

    print sorted array
    clean up semaphores and shared memory
```

Consider P2 has held the semaphore to write in arr[3]. Suppose P1 checks arr[3] before P2 decides it will change it. P1 
decides there's no swap. P2 then changes arr[3], followed by P1 declaring it's finished and then P2 declaring
it's also finished. We have an issue here since P1 may need to perform another swap. 
Hence, a process accessing a shared index for reading, may need to write as well, hence we lock the critical
section on access.


## Discussion: How the algorithm correctly solves the problem.
- We've partioned the sorting into three equal ranges for three processes.
- The ranges are treated as bubble sort: a process will continuously loop over its range
and swap out-of order values until its range is sorted. When the range is sorted, the process
is marked as "finished" and effectively sleeps since it has no more work to be done for now.
- The sorting process coverts all indices to lower case.
- When a process will access a shared index (2 or 4), it will wait on the corresponding semaphore.
Hence, concurrency is maintained since the entire array is not locked - if P1 is accessing arr[2], P2
can still access the other elements in it's range, P3 can access all its elements.
- Swaps waken neighbours. When a P1 makes a swap, it wakens P2. When P2 makes a swap, it wakens both P1 
and P3. When P3 makes a swap, it wakens P2. 
- The time where all processes sleep (are done), is when the sorting is done - otherwise, a swap would
be made and wake any process that may need to take action.
- To check for the done case, we continuously check if all the processes are in the finished state: this does not affect 
concurrency as it's strictly a repeated reading procedure, hence nothing needs to be locked.
- If the user wants to run the program in debug mode, their choice is stored in a boolean variable that is 
checked for the debug logs.

## Discussion: Test Results 

### Test 1: X, A, z, M, W, y, D     (Debug)
```
Would you like to run in debug mode (yes/no)? yes
Ok, running in debug.
Please enter 7 letters to sort: 
Character 1: X
Character 2: A
Character 3: z
Character 4: M
Character 5: W
Character 6: y
Character 7: D
[Debug] Process P1: performed swapping.   
[Debug] Process P1: performed no swapping.
[Debug] Process P1: performed no swapping.
[Debug] Process P1: performed no swapping.
[Debug] Process P3: performed no swapping.
[Debug] Process P2: performed swapping.
[Debug] Process P3: performed swapping.
[Debug] Process P2: performed swapping.
[Debug] Process P3: performed swapping.
[Debug] Process P1: performed no swapping.
[Debug] Process P2: performed no swapping.
[Debug] Process P3: performed swapping.
[Debug] Process P1: performed swapping.
[Debug] Process P2: performed swapping.
[Debug] Process P3: performed no swapping.
[Debug] Process P1: performed no swapping.
[Debug] Process P3: performed no swapping.
[Debug] Process P2: performed swapping.
[Debug] Process P1: performed no swapping.
[Debug] Process P2: performed swapping.
[Debug] Process P3: performed no swapping.
[Debug] Process P2: performed no swapping.
[Debug] Process P3: performed no swapping.
[Debug] Process P1: performed no swapping.
[Debug] Process P2: performed no swapping.
[Debug] Process P1: performed swapping.
[Debug] Process P1: performed no swapping.
[Debug] Process P2: performed no swapping.
[Debug] Process P1: performed no swapping.
[Debug] Process P2: performed no swapping.
Sorted Array: [ a d m w x y z ]
```

## Test 2: Q, H, t, b, U, S, a      (Production)
```
Would you like to run in debug mode (yes/no)? no
Ok, running in production.
Please enter 7 letters to sort:
Character 1: Q
Character 2: H
Character 3: t
Character 4: b
Character 5: U
Character 6: S
Character 7: a
Sorted Array: [ a b h q s t u ]
```

Clearly, both test cases produced a correct and expected result
with the letters lowercase and sorted in Lexicographic order.