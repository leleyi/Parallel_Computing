#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <unistd.h>
//Ensure forward compatibility of a written parallel program
#ifdef _OPENMP
#include <omp.h>
#else
int omp_get_max_threads(){
    return 1;
}

int omp_get_num_procs(){
    return 1;
}

int omp_get_thread_num(){
    return 0;
}

void omp_set_num_threads(int tds){
    return ;
}
// if not use the gettimeofday like before
double omp_get_wtime(){
    struct timeval T;
    double time_ms;

    gettimeofday(&T, NULL);
    time_ms = (1000.0 * ((double) T.tv_sec) + ((double)T.tv_usec) / 1000.0);
    return (double)(time_ms / 1000.0);
}

void omp_set_nested(int nests){
    return ;
}
#endif

#define A 12
#define SEED 0
#define DEFAULT_N 20
#define DEFAULT_M 1
#define THREADS 1

double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max,unsigned int seed);

int map(double *arr1, size_t size1, double *arr2, size_t size2);
double reduce(double *arr, size_t size);
int merge(double *arr1, double *arr2, size_t size2);
int insert_sort(double *arr, size_t from, size_t to);
void sort(double **array, size_t size);
void compare_time(double time_begin, double time_end, double* min_time);
int threads_num;



int main(int argc, char* argv[]) {
    size_t N;
    if(argc > 1){
        N = (size_t)atoi(argv[1]);
    }
    else{
        N = DEFAULT_N;
    }
    omp_set_nested(1);
    if(argc > 2){
        omp_set_num_threads(atoi(argv[2]));
    }
    else{
        omp_set_num_threads(DEFAULT_M);
    }
    double x, *m1 = (double*)malloc(sizeof(double) *N), *m2 = (double*)malloc(sizeof(double) * N / 2 );

    double t1, t2, minimal_time_ms = -1.0,
           step_begin, step_end,
           minimal_generate_time = -1.0,
           minimal_map_time = -1.0,
           minimal_merge_time = -1.0,
           minimal_sort_time = -1.0,
           minimal_reduce_time = -1.0;

    int i, iter = 5;
    omp_set_nested(1);
    #pragma omp parallel shared(i, iter)
    #pragma omp sections
    {
        #pragma omp section
        {
            // using the wtime replace gettimeofday
            for(i = 0; i < iter; ++i){
                t1 = omp_get_wtime();
                step_begin = omp_get_wtime();
                fill_array(m1, N, 0, A,i);
                fill_array(m2, N / 2, A, 10 * A, i);
                step_end = omp_get_wtime();
                compare_time(step_begin, step_end, &minimal_generate_time);

                step_begin = omp_get_wtime();
                map(m1, N, m2, N / 2);
                step_end = omp_get_wtime();
                compare_time(step_begin, step_end, &minimal_map_time);

                step_begin = omp_get_wtime();
                merge(m1, m2,  N / 2);
                step_end = omp_get_wtime();
                compare_time(step_begin, step_end, &minimal_merge_time);

                step_begin = omp_get_wtime();
                sort(&m2, N / 2);
                step_end = omp_get_wtime();
                compare_time(step_begin, step_end, &minimal_sort_time);

                step_begin = omp_get_wtime();
                x = reduce(m2, N / 2);
                step_end = omp_get_wtime();
                compare_time(step_begin, step_end, &minimal_reduce_time);

                t2 = omp_get_wtime();
                compare_time(t1, t2, &minimal_time_ms);

            }
        }
    }
        free(m1);
        free(m2);
        printf("time: %f ms; N = %zu; X = %f ; threads: %d; generate: %f ms; map: %f ms; merge: %f ms; sort: %f ms; reduce: %f ms\n",
                minimal_time_ms,
                N,
                x,
                omp_get_max_threads(),
                minimal_generate_time,
                minimal_map_time,
                minimal_merge_time,
                minimal_sort_time,
                minimal_reduce_time);
        return 0;

}

double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max,unsigned int seed) {
    int i;
    unsigned tmp_seed;
    #pragma omp parallel for default(none) shared(size, arr, min, max) \
    private(i, seed, tmp_seed) schedule(runtime)
    for (i = 0; i < size; i++){
        tmp_seed = sqrt((i + 2) * 100);
        arr[i] = ((double) (rand_r(&tmp_seed)%(100*(max-min)))/100) + min;
    }
    return arr;
}

int map(double *arr1, size_t size1, double *arr2, size_t size2) {

    double x;
    int i;
    //const clock_t begin_time = clock();
    //omp_set_num_threads(4);
    //double start = omp_get_wtime( );
    #pragma omp parallel for default(none) shared(size1, arr1) private(i)\
    schedule(runtime)
    /* Map Stage. Apply  Cubic root after division by e */
    for (i = 0; i < size1; i++) {
        arr1[i] = pow(arr1[i] / exp(1) , 1.0 /3);
        //#pragma omp critical
        //std::cout << omp_get_thread_num() << i << " ";
    }
    x = 0;
    /*Natural logarithm of the tangent modulus*/
    for (i = 0; i < size2; i++) {
        arr2[i] = log(tan(arr2[i] + x));
        x = arr2[i];
    }
    //double end = omp_get_wtime( );
    //std::cout << "Time clock(): "<< float( clock () - begin_time  ) /  CLOCKS_PER_SEC << std::endl;
    //std::cout << "Time wtime(): "<< float( end - start  )  << std::endl;
    return 0;
}

/* Merge Stage.*/
int merge(double *arr1, double *arr2, size_t size2) {

    int i;
    //printf("thread_num %d ", threads_num);
    #pragma omp parallel for default(none) shared(size2, arr1, arr2) \
    private(i) schedule(runtime)
    //num_threads(threads_num)
    for (i = 0; i < size2; i++) {
        arr2[i] = pow(arr1[i], arr2[i]);
    }
    return 0;
}

double reduce(double *arr, size_t size) {
    double res = 0, min;
    int i = 0;

    if (size > 0)
        min = arr[0];
    else
        min = 0;
    for (i = 0; i < size; i++) {
        if (arr[i] < min && arr[i] != 0) {
            min = arr[i];
        }
    }

    #pragma omp parallel for default(none) shared(size, arr, min, res) \
    private(i) schedule(runtime)
    //num_threads(threads_num)
    for (i = 0; i < size; i++) {
       if ((int)(arr[i] / min) % 2 == 0) {
            res += sin(arr[i]);
        }
    }
    return res;
}


 /* insert sort */
int insert_sort(double *arr, size_t from, size_t to) {
    int m,n,key;
    for (m=1 + from; m < to - 1;m++){
            key = arr[m];
            n=m-1;
            while((n>=0) && (arr[n]>key)) {
                     arr[n+1] = arr[n];
                    n--;
            }
            arr[n+1] = key;
    }
    return 0;
}

//merge all the split
void merge_arrays(double *array_old, double *array_new, unsigned int n, int num, int chunk){
    unsigned int i, m;
    unsigned int *arr_i = (unsigned int*)calloc(num, (sizeof(unsigned int)));
    unsigned int min;
    for (i = 0; i < n; i++){
        min = 0;
        for(m = 0; m < num; m++){
            if(arr_i[m] < chunk) {
                array_new[i] = array_old[m * chunk + arr_i[m]];
                min = m;
                break;
            }
        }
        for (m = 0; m < num; m++){
            if((m * chunk + arr_i[m] < n) && (arr_i[m] < chunk) && array_old[m * chunk + arr_i[m]] < array_new[i]){
                array_new[i] = array_old[m * chunk + arr_i[m]];
                min = m;
            }
        }
        arr_i[min]++;
    }
}

// split array to the num of theards
void sort(double **array, size_t size){
    int from, to;
    int count_threads = omp_get_max_threads();
    int curr_chunk = size % count_threads ? size / count_threads + 1 : size/count_threads;
    unsigned int i;
    double *array_new=(double*)malloc(sizeof(double) * size);
    #pragma omp parallel for default(none) shared(array, size, curr_chunk, count_threads)\
    private(i, from, to) schedule(runtime)
    for(i = 0; i < count_threads; i++){
        from = i * curr_chunk;
        to = i * curr_chunk + curr_chunk - 1;
        insert_sort(*array, from < size - 1 ? from : size - 1, to < size - 1 ? to : size - 1);
    }
    merge_arrays(*array, array_new, size, count_threads, curr_chunk);
    free(*array);
    *array = array_new;
}

void compare_time(double time_begin, double time_end, double* min_time){
    double step_time = 1000 * (time_end - time_begin);
    if((*min_time == -1.0) || step_time < *min_time){
        *min_time = step_time;
    }
}


