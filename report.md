# Assignment report on multithreading

## Program layout

The program consists of a main function and several additional functions in a single file called
newton.c. All required include files are standard headers.

- File handling functions in stdio.h
- Functions for memory allocation and string handling in stdlib.h
- String handling functions in string.h
- Mathematic functions in math.h
- Type double complex in complex.h
- Multithreading functionality in pthread.h

The program has a number of global variables:
- itarr: an array that stores the number of iterations for each pixel.
- carr: an array that stores the root attractor for each pixel.
- items: an array that marks which pixels are finished
- other important values including file pointers, roots and error range

We start our main function by reading user arguments i.e. number of threads, polynomial degree and
number of lines. Then we initialize the threads, in addition to our main thread we have one thread
for writing and the specified number of threads for running the newton iterations. We calculate the
roots of the pollynomial function and store it in a global array. These values are later used as a
stopping condition in the newton iterations.

Now we have reached the point of calling pthread_create for each of the threads. We use the function
newtwrapper to start the threads, which we give only one argument: the id of the thread (a unique
integer from 0 to "nr of threads" - 1).

The newtwrapper function in turn calls the function newt, which performs the newton iterations for a
single pixel. The wrapper function is designed so that all threads start iterations for the first
pixels. E.g if the thread has id=3 and the number of threads is 7, thread number 3 will perform
iterations for pixels 3+7x0, 3+7x1, 3+7x2, 3+7x3, .... This makes it possible for the writer thread to be
the most efficient, because it has to write the pixels in increasing order. After finding a root it
will also set the corresponding element in "item_done" list to 1.

The newt function simply performs newton iteration with the given stop conditions. We added a
condition that checks wether the x-value is outside the unit circle + allowed error. By doing this
we dont have to use the for-loop to check the value against all roots in every iteration. The function
intpow is simply a replacement for the slow pow[]-function, with hardcoded cases to the power of 6 and
a loop if the exponed would be of a higher order. The newt function will also save which root it converges
to in a long array (l times l long). It will also save the number of iterations it took in an l time l
array, and also save the maximum interation before the function is terminated.

Parallell to the newton-calulculating-threads we have a writing-thread. The writing thread will simply
open two files (one for the attractors, one for convergence) and write the results found in itarr and
carr. To overcome the fact that the threads are working parallell we use our "item_done"-list. The
writing thread will have a while loop that always check if the element in "item_done" is equal to
1 (default 0). If it is, it will check and write to the ppm files with predefined colors for attractors
(P3) and a black and white (P2) for convergence.

Our main program will then call the funtion pthread_join for all initialized threads which will wait for
all the threads to finnish before continue down the main fucntion.

Last of all we deallocate the memory previously allocated with the funtion free.

-----------

Our performance was quite good, if we are to compare to the performance goals. We get, unless Gantenbein
lags, quite large margins for example:

|  Arguments   | Our result | Perf. goal|
-----------------------------------------
-t1 -l1000 1   | 0.159 s    | 1.01 s    |
-----------------------------------------
-t10 -l50000 7 | 415.15 s   | 594 s     |
-----------------------------------------
-t4 -l1000 5   | 0.167 s    | 0.42 s    |
-----------------------------------------

Our way of using threads is probably best when using many threads. First of all picking just one element
then jump "number of threads" to pick next one isn't optimized. But, by using this strategy and letting each
thread calculate the each first  element we can make sure that our single writer-thread is able to access the
information as early as it is possible. Thus we assume that the bottleneck of our program is writing to ppm-
files which seems reasonable since we can choose the number of threads who do the newton iteration.

A thing which could have improved our code could have been trying to vectorize some instructions. E.g. the
first thread takes element 0,1,2,3; second thread 4,5,6,7 etc. in each loop. For this we need a number, -l, that
is dividable by, in this case, 4.

Another thing to try is instead of chosing thread #1 to take element 0, thread #2 to take element 1 etc., would
be to let each thread calculate a whole row instead of element. For example thread #1 will calculate the first
row (with index 0). Second thread would calculate row two (with index 1) etc. and in that way take advantage
of that c is a row-major language.

More obvious optimizations would be to reduce the size of the ppm-file when the number of lines  is getting
to big.

Using fwrite instead of fprintf would perhaps been faster.
