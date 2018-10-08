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
iterations for pixels 3, 3+7, 3+7*2, 3+7*3, .... This makes it possible for the writer thread to be
the most efficient, because it has to write the pixels in increasing order.

The newt function simply performs newton iteration with the given stop conditions. We added a
condition that checks wether the x-value is outside the unit circle + allowed error. By doing this
we dont have to use the for-loop to check the value against all roots in every iteration.



In our main function we use four different functions.
