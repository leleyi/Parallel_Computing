#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>

#include <omp.h>

#define A 12
#define SEED 0
#define DEFAULT_N 20

double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max);
int map(double *arr1, size_t size1, double *arr2, size_t size2);
double reduce(double *arr, size_t size);
int merge(double *arr1, double *arr2, size_t size2);
int stupid_sort(double *arr, size_t size);

int main(int argc, char* argv[]) {
    struct timeval T1, T2;
    long time_ms, minimal_time_ms = -1;
    size_t N;
    if(argc > 1)
        N = (size_t) atoi(argv[1]);
    else
        N = DEFAULT_N;
    double x, *m1 = malloc(sizeof(double) * N), *m2 = malloc(sizeof(double) * (N / 2));

    for (int i = 0; i < 10; ++i) {
        gettimeofday(&T1, NULL);
        fill_array(m1, N, 0, A);
        fill_array(m2, N/2, A, 10*A);
        map(m1, N, m2, N/2);
        merge(m1, m2, N/2);
        stupid_sort(m2, N/2);
        x = reduce(m2, N/2);
        gettimeofday(&T2, NULL);
        time_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
        if ((minimal_time_ms == -1) || (time_ms < minimal_time_ms))
            minimal_time_ms = time_ms;
    }

    free(m1);
    free(m2);

    printf("time: %ld ms; N = %zu; X = %f\n", minimal_time_ms, N, x);
    return 0;
}

double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max) {
    unsigned int seed = SEED;
    int i;

    for (i = 0; i < size; i++)
        arr[i] = ((double) (rand_r(&seed)%(100*(max-min)))/100) + min;

    return arr;
}

int map(double *arr1, size_t size1, double *arr2, size_t size2) {
    double x;
    int i;

    #if defined(CHUNK) && defined(SCHEDULE)
    #pragma omp parallel for default(none) shared(size1, arr1) private(i) schedule(SCHEDULE, CHUNK) num_threads(THREADS)
    #else
    #pragma omp parallel for default(none) shared(size1, arr1) private(i) num_threads(THREADS)
    #endif
    /* Map Stage. Apply  Cubic root after division by e */
    for (i = 0; i < size1; i++) {
        arr1[i] = pow(arr1[i] / exp(1) , 1.0 /3);
    }

    x = 0;
    /*Natural logarithm of the tangent modulus*/
    for (i = 0; i < size2; i++) {
        arr2[i] = log(tan(arr2[i] + x));
        x = arr2[i];
    }

    return 0;
}

/* Merge Stage.*/
int merge(double *arr1, double *arr2, size_t size2) {
    int i;
    #if defined(CHUNK) && defined(SCHEDULE)
    #pragma omp parallel for default(none) shared(size2, arr1, arr2) private(i) schedule(SCHEDULE, CHUNK) num_threads(THREADS)
    #else
    #pragma omp parallel for default(none) shared(size2, arr1, arr2) private(i) num_threads(THREADS)
    #endif
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

    #if defined(CHUNK) && defined(SCHEDULE)
    #pragma omp parallel for default(none) shared(size, arr, min, res) private(i) schedule(SCHEDULE, CHUNK) num_threads(THREADS)
    #else
    #pragma omp parallel for default(none) shared(size, arr, min, res) private(i) num_threads(THREADS)
    #endif
    for (i = 0; i < size; i++) {
        if ((int)(arr[i] / min) % 2 == 0) {
            res += sin(arr[i]);
        }
    }

    return res;
}


 /* insert sort */
int insert_sort(double *arr, size_t size) {
    int m,n,key;
    for (m=1;m< size;m++){
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
