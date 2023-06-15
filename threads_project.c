// Multithreading Project
// Left Riemann

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// take 1 command line arg for the number of threads
// each thread can change the value of global var area w/o corrupting it

// function: y = 2x + 3
// interval [0, 32]
// delta x = 0.2

pthread_mutex_t my_lock;
float nrectangles = 32.0/0.2;
int col_per_thread = 0; 
int nthreads;
float area = 0;

// function each thread excutes
void *col_area(void *i);

// take 1 cmd line arg for the number of threads
int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        printf("Program takes 1 number only.\n");
        exit(0);
    }

    int i;
    nthreads = atoi(argv[1]); 
    col_per_thread = 32.0 / nthreads / 0.2;

    // initiallize the mutex
    pthread_mutex_init(&my_lock, NULL);

    int retval;

    // make an array to hold the threads
    pthread_t threads[nthreads];
    int thread_ids[nthreads];

    // create the threads
    for (int i = 0; i < nthreads; i++) {
        thread_ids[i] = i;
        retval = pthread_create(&threads[i], NULL, col_area, (void *) &thread_ids[i]);
    }

    // join the threads
    for (int i = 0; i < nthreads; i++) {
        retval = pthread_join(threads[i], NULL);
    }  

    printf("Integration with Left Riemann's Method\n");    
    printf("The estimated area under the curve y = 2x + 3 on interval [0,32]: \n%f \n", area);

    // destroy threads
    pthread_mutex_destroy(&my_lock);

    pthread_exit(NULL);

    return 0;
}

void *col_area(void *id) {
    int i = *(int *) id;
    
    float x, f_x;   
    float x_min = 32.0 / nthreads * i; 
    float group_area = 0;


    for (int j = 0; j < col_per_thread; j++) {
        x =  x_min + 0.2 * j; 
        f_x =  2.0 * x + 3.0;
        group_area += 0.2 * f_x;
    }

    // critical section
    // update to area 
    pthread_mutex_lock(&my_lock);
    area += group_area;
    pthread_mutex_unlock(&my_lock); 

}