#include<stdlib.h>
#include<sys/time.h>
#include<stdio.h>
#include<iostream>
#include<math.h>

#include<omp.h>

#define THREADS 1
int main(int argc, char* argv[])
{
    int threads_nums;
    if(argc > 1)
        threads_nums=atoi(argv[1]);
    else
        threads_nums=THREADS;

    #pragma omp parallel num_threads(threads_nums)
    {
            #pragma omp for schedule(runtime)
        for(int i=0; i<9; ++i){
                    #pragma omp critical
                    std::cout << omp_get_thread_num() << i << " ";
        }
    }
    return 0;
}
