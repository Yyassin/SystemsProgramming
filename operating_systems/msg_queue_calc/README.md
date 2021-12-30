>>> Client-Server Message-Queue Based Calculator <<<
> Author: Yousef Yassin
> Date: November 29, 2021

## Table of Contents
    - Assumptions and Design Choices
    - Compilation Instructions
    - Pseudocode
    - Discussion on Correctness
    - Discussion of Test Results

## Assumptions and Design Choices
    We have decided that our implementation will only accept *integer* arguments, and 
    return integer responses except for the average which will be presented to 3 decimal spaces.
    This decision has been specifically made to make the Delete (N) command more intuitive, in that
    deleting multiple instances of a specific real number would be a rare occurence. Making
    the change to real numbers would be relatively trivial - method signatures would need to be 
    adapted and equality checks between real numbers would be need to be done with a threshold check.

    ** We do not check if the input is indeed an integer, we assume the user will enter valid inputs. **

    We have also decided that a Delete (N) instances that results in no deletions (N wasn't in the 
    dataset) will still result in a success. Semantically, all instances of N are no longer in the 
    set so this result seemed logical. The exception is deleting from an empty dataset.

    We have decided that Delete, Median, Minimum, Sum and Average commands on an empty
    dataset will result in an error rather than returning 0. An empty set has no elements so it did 
    not seem logical to accept a delete request or return a result of 0. Instead this is reserved for 
    an *actual* minimum of  0, or average of 0, etc..

    We have also decided to define elapsed time as "processing" time, hence it is calculated server side.

## Compilation Instructions
    To complile and run the program, follow these steps:
    
    - Open a terminal and navigate to the directory with the project
    source files and dependencies:
    ```
    $ cd ~/msg_queue_calc
    ```

    - Run the "make" command inside the project directory to compile.
    ```
    $ make
    ```

    - Following the compilation, the project executables "user" and "calculator" will be 
    added to the project directory. Open two terminals, one to run each process.

    - Run the ./calculator (server) process in one of the terminals first.

    - Then run the ./user (client) process in the other terminal.

    - Follow the prompts given by the user process to use the program. Test
    cases have been provided further below for convenient testing.

    - Enjoy!

## Pseudocode
    - This project depends on two primary data structures: a vector that acts as a dynamically 
    resizable collection and a priorityqueue that can act as either a min or max heap. The Pseudocode
    below will not cover the semantics for the general operations on these data structures.

    >>> User.c
    Open/connect to two message queues to acheive bidirectional communication.
    while (the user does not enter the exit command)
        prompt the user to enter a command until they enter a valid one.
        If the command requires an argument, prompt the user for that argument.
        Package the input into a packet and send it to the calculator.
        Once the calcultor's response is received, process the message and output the result.
    end while
    Exit
        
    >>> Calculator.c
    Create/open two message queues to acheive bidirectional communication.
    while (haven't received the exit command from the user client)
        wait for a message to be received from the client.
        Once received, process the message in a subroutine.
            *As a design choice, if the dataset is empty and the
            operation is not an insert, return an error (0 is reserved for a real 0).
            Otherwise, begin the timer.
            Based on the operation, compute the result (described below).
            End the timer.
            Format the result and elapsed time in the received packet and sent it back to user.
    end while
    Destroy and cleanup any datastructures used and message queues.

## Discussion on Correctness
    - This project correctly solves the problem namely in the use of well-defined datastructures that guarantee 
    a correct output in response to the supplied command. Moreover, we have implemented a rendez-vous message system
    with a block send and receive such that both the client and server remain synchronized. A description
    of the command implementations is provided below:

    - The dataset of numbers was stored in a Median Heap, a data structure that consists
    of a min heap and a max heap. It is managed in such a way that the median can be determined 
    in constant time: namely, upon insertion, all values less than the median are placed into the maxheap 
    while the rest in the minheap. We rebalance the heap following an insertion to force the size difference
    between the heaps to be a maximum of 1. This is involves popping the root from the larger heap and inserting
    it into the smaller heap.

    For example: 1 2 3 4 5 6 8 9 -> MaxHeap: *4 3 2 1, MinHeap: *5 6 8 9
    The median is the between 4 and 5, the roots of both structures. 
    Size and sum are tracked for easy sum and average computation. All other operations follow from heaps.

    ### Operations
    >> (I)nsert N
    if (first element to insert)
        insert into min heap (technically, it is larger than the median)
    else
        if (N > curr_median)
            insert N into min heap
        else
            insert N into max heap

    Update the tracked sum & size
    rebalance so that difference in size is <= 1.

    >> (u)Median
    The min heap has half the set, all larger than or equal to the median. The max heap has the other half,
    all less than or equal to the median. 

    if (minheap.size == maxheap.size)
        return both minheap.root and maxheap.root (max has lower median, min has larger one)
    else if (minheap.size > maxheap.size)
        return minheap.root (difference will be 1, so minheap root is the median)
    else 
        return maxheap.root (difference will be 1, so maxheap root is the median)

    >> (D)elete N
    count = 0
    For each heap
        for number in heap
            if number == N
                remove it
                count++
    
    heapify the heap to restore its structure
    update the sum and size (which is why we need count)

    >> (S)um
    We keep track of it, return it directly.

    >> (A)verage
    We keep track of sum and size, return sum / size.

    >> (M)inimum
    The lower half of our data set is stored in the max heap, unless its 
    the first element, in which case return minheap.peek().

    Otherwise...
    Initialize the minimum to be the first element in the bottom row of the max heap
    For each element in the bottom row of the max heap
        if element is smaller than the current minimum, update the current minimum

    return the minimum

## Discussion of Test Results
** Note that the tests below are applied sequentially - 
   that is, Test 2 picks off from Test 1 and so on.. **

### TEST 1 - Operations on an empty set
    These should all produce errors as documented in the Assumptions
    and Design Choices section above. Note, the elapsed time is not 0 since
    we still factor in the time for the server to print it's own status.

>>> $ ./calculator
>>> $ ./user

>>> User
```
    Message Size: 32
    Welcome to the user interface.     
    Please begin by entering a command:
    (I)nsert (N)
    (D)elete (N)
    (U)Median
    (M)inimum
    (S)um
    (A)verage

    Enter a command: D
    Selected Delete(). Insert an *integer* argument: 5
    [av.elapsed=21.000us] Server encountered an error processing the request! Retry.

    Enter a command: p
    That's an invalid command, try again!

    Enter a command: U
    [av.elapsed=21.000us] Server encountered an error processing the request! Retry.

    Enter a command: m
    [av.elapsed=48.000us] Server encountered an error processing the request! Retry.

    Enter a command: s
    [av.elapsed=23.000us] Server encountered an error processing the request! Retry.

    Enter a command: a
    [av.elapsed=21.000us] Server encountered an error processing the request! Retry.
```
>>> Calculator
```
    Calculator started successfully.
    Received command on empty set, return error!

    Received command on empty set, return error!

    Received command on empty set, return error!

    Received command on empty set, return error!

    Received command on empty set, return error!
```


### TEST 2 - Insert and operations on 1 element
    All results are trivial, should return the inserted value.

>>> User
```
    Enter a command: I
    Selected Insert(). Insert an *integer* argument: 4
    [av.elapsed=79.000us] Server inserted 4 successfully. 

    Enter a command: u
    [av.elapsed=12.500us] Server> median= 4.

    Enter a command: m
    [av.elapsed=11.500us] Server> minimum= 4.

    Enter a command: s
    [av.elapsed=10.500us] Server> sum= 4.

    Enter a command: a
    [av.elapsed=20.000us] Server> average= 4.000.
```
>>> Calculator
```
    Received command Insert with argument 4.

    Received command Median.
    Returned result 4

    Received command Minimum.
    Returned result 4

    Received command Sum.
    Returned result 4

    Received command Average.
    Returned result 4.000
```

### TEST 3 - Insert multiple elements in the following sequence:
    > 5 -3 99 20 5 1 4 1
    Then do the operations.

    With the previous 4, the sorted sequence is -3 1 1 4 4 5 5 20 99. Sum=136, Average=136/9 ~ 15.111

>>> User (lines omitted for brevity)
```
    ...
    Enter a command: I
    Selected Insert(). Insert an *integer* argument: -3
    [av.elapsed=52.500us] Server inserted -3 successfully. 

    Enter a command: I
    Selected Insert(). Insert an *integer* argument: 99
    [av.elapsed=48.333us] Server inserted 99 successfully. 
    ...

    Enter a command: u
    [av.elapsed=18.000us] Server> median= 4.

    Enter a command: s
    [av.elapsed=17.667us] Server> sum= 136.

    Enter a command: a
    [av.elapsed=21.333us] Server> average= 15.111.

    Enter a command: m
    [av.elapsed=23.333us] Server> minimum= -3.
```
>>> Server (lines omitted for brevity)
```
    ...
    Received command Insert with argument -3.

    Received command Insert with argument 99.
    ...

    Received command Median.
    Returned result 4

    Received command Sum.
    Returned result 136

    Received command Average.
    Returned result 15.111

    Received command Minimum.
    Returned result -3
```

### TEST 4 - Add one more element, 5, to test even median case.
    New sorted set would be -3 1 1 4 4 5 5 5 20 99. Two middle elements are 4 and 5.
    The results can be verified to be correct.

>>> User
```
    Enter a command: I
    Selected Insert(). Insert an *integer* argument: 5
    [av.elapsed=40.100us] Server inserted 5 successfully. 

    Enter a command: u
    [av.elapsed=24.500us] Server> medians= 4 5.

    Enter a command: s
    [av.elapsed=19.250us] Server> sum= 141.

    Enter a command: m
    [av.elapsed=26.250us] Server> minimum= -3.

    Enter a command: a
    [av.elapsed=23.750us] Server> average= 14.100.
```
>>> Calculator
```
    Received command Insert with argument 5.

    Received command Median.
    Returned result two medians 4 5

    Received command Sum.
    Returned result 141

    Received command Minimum.
    Returned result -3

    Received command Average.
    Returned result 14.100
```

### TEST 5 - Delete all instances of 5 and 4 and do operations
    New sorted set would be -3 1 1 20 99. Median=1, Sum=118, Average=23.6

>>> User
```
    Enter a command: D
    Selected Delete(). Insert an *integer* argument: 5
    [av.elapsed=13.500us] Server removed all instances of 5 successfully. 

    Enter a command: D
    Selected Delete(). Insert an *integer* argument: 4
    [av.elapsed=16.667us] Server removed all instances of 4 successfully. 

    Enter a command: u
    [av.elapsed=24.000us] Server> median= 1.

    Enter a command: s
    [av.elapsed=20.000us] Server> sum= 118.

    Enter a command: m
    [av.elapsed=25.400us] Server> minimum= -3.

    Enter a command: a
    [av.elapsed=25.600us] Server> average= 23.600.
```
>>> Calculator
```
    Received command Delete with argument 5.

    Received command Delete with argument 4.

    Received command Median.
    Returned result 1

    Received command Sum.
    Returned result 118

    Received command Minimum.
    Returned result -3

    Received command Average.
    Returned result 23.600
```

## TEST 6 - Make sure everything can be deleted, and that min is updated. Then quit.
```
    Enter a command: D
    Selected Delete(). Insert an *integer* argument: -3
    [av.elapsed=20.000us] Server removed all instances of -3 successfully. 

    Enter a command: m
    [av.elapsed=25.333us] Server> minimum= 1.

    Enter a command: D
    Selected Delete(). Insert an *integer* argument: 1
    [av.elapsed=21.600us] Server removed all instances of 1 successfully. 

    Enter a command: m
    [av.elapsed=24.857us] Server> minimum= 20.

    Enter a command: D
    Selected Delete(). Insert an *integer* argument: 20
    [av.elapsed=22.333us] Server removed all instances of 20 successfully. 

    Enter a command: m
    [av.elapsed=24.625us] Server> minimum= 99.

    Enter a command: D
    Selected Delete(). Insert an *integer* argument: 99
    [av.elapsed=24.286us] Server removed all instances of 99 successfully. 

    Enter a command: m
    [av.elapsed=45.000us] Server encountered an error processing the request! Retry.

    Enter a command: q
    Client shutting down.
```
>>> Server
```
    Received command Delete with argument -3.

    Received command Minimum.
    Returned result 1

    Received command Delete with argument 1.

    Received command Minimum.
    Returned result 20

    Received command Delete with argument 20.

    Received command Minimum.
    Returned result 99

    Received command Delete with argument 99.

    Received command on empty set, return error!

    Received command Quit. Exiting.
    Calculator shutting down.
```
