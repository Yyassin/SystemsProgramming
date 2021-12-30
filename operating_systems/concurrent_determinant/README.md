>>> Concurrent Determinant Calculator <<<
> Author: Yousef Yassin
> Date: October 26, 2021

## Description
This project consists of a concurrent determinant computer for 3x3 matrices. The program
consists of 3 processes that use shared memory to compute both the determinant and 
largest value in an input matrix, M.

## Minimum Requirements
- Any Linux Distribution (Tested on Fedora 34)
- A C compiler

## Running Project
Once the files have been downloaded to your local machine, follow the instructions
below to run the project:

- Open a terminal and navigate to the directory with the project
source files and dependencies.
```
    $ cd ~/concurrent_determinant
```

- Run the "make" command inside the project directory to compile.
```
    $ make
```

- Following the compilation, the project executable (DET) will be added to the project
directory. Run it with the following command:
```
    $ ./DET
```

- Enjoy!

*Note*: The project will run with a default matrix described in the section below.


## Test Results Discussion

The input matrix M is defined in `main()`. For a given input, the program
computes the determinant of the matrix and the largest value in the matrix. 
Posted below are three example cases and their corresponding outputs:

### Example 1 (The current case)

Input:
M = [ 20 20 50 ]
    [ 10  6 70 ]
    [ 40  3  2 ]

Output:
(Determinant) D=75240.0000, (Largest number) L=71.0000

### Example 2

Input:
M = [ -20  0   0 ]
    [   0  6   0 ]
    [  40  3   2 ]

Output:
(Determinant) D=-240.0000, (Largest number) L=40.0000

### Example 3

Input:
M = [ -20       -1.1 15 ]
    [   0        3.3  0 ]
    [ 125.125 3  3    2 ]

Output:
(Determinant) D=-6325.6875, (Largest number) L=125.1250

Each of the results above are mathematically correct. The determinants can be verified, in fact 
Example 2's determinant is simply the product of the diagonal terms. Verifying the largest
number for each matrix is trivial, but once again the test cases pass.
