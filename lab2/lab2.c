#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>

#include <fcntl.h>
#include "../lib/FW_1.3.1_Lin64/fwBase.h"
#include "../lib/FW_1.3.1_Lin64/fwSignal.h"

#define A 12
#define SEED 0
#define DEFAULT_N 20
#define DEFAULT_M 1

double* fill_array(size_t size, unsigned int min, unsigned int max);
int map(double *arr1, size_t size1, double *arr2, size_t size2);
double reduce(double *arr, size_t size);
int merge(double *arr1, double *arr2, size_t size2);
int insert_sort(double *arr, size_t size);

int main(int argc, char* argv[]) {
    struct timeval T1, T2;
    long time_ms, minimal_time_ms = -1;
    size_t N;
    if(argc > 1)
        N = (size_t) atoi(argv[1]);
    else
        N = DEFAULT_N;
    if(argc > 2) {
        fwSetNumThreads(atoi(argv[2]));
    } else {
        fwSetNumThreads(DEFAULT_M);
    }
    double *m1, *m2, x;

    for (int i = 0; i < 5; ++i) {
        gettimeofday(&T1, NULL);
        m1 = fill_array(N, 1, A);
        m2 = fill_array(N/2, A, 10*A);
        map(m1, N, m2, N/2);
        merge(m1, m2, N/2);
        insert_sort(m2, N/2);
        x = reduce(m2, N/2);
        gettimeofday(&T2, NULL);

        time_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
        if ((minimal_time_ms == -1) || (time_ms < minimal_time_ms))
            minimal_time_ms = time_ms;
        free(m1);
        free(m2);
    }

    printf("time: %ld ms. N = %zu. X = %f.\n", minimal_time_ms, N, x);
    return 0;
}

double* fill_array(size_t size, unsigned int min, unsigned int max) {
    double *arr = (double*)malloc(sizeof(double) * size);
    unsigned int seed = SEED;
    int i;

    for (i = 0; i < size; i++)
        arr[i] = ((double) (rand_r(&seed)%(100*(max-min)))/100) + min;

    return arr;
}

/*Cubic root after division by e*/
int map(double *arr1, size_t size1, double *arr2, size_t size2) {
    double x = 0;
    int i = 0;

    fwsDivC_64f(arr1,exp(1), arr1, size1);
    fwsCbrt_64f_A50(arr1, arr1, size1);

     /*Natural logarithm of the tangent modulus*/
    for (i = 0; i < size2; i++) {
        arr2[i] = log(tan(arr2[i] + x));
        x = arr2[i];
    }
    return 0;
}


/*Raising to a power (i.e.M2[i] =M1[i]ˆM2[i])*/
int merge(double *arr1, double *arr2, size_t size2) {
    fwsPow_64f_A50(arr1, arr2, arr2, size2);
    return 0;
}

/* insert sort*/
int insert_sort(double *arr, size_t size) {
    int m, n;
    double key;
    for(m = 1; m < size; m++){
        key = arr[m];
        n = m- 1;
        while((n>=0) && (arr[n] > key)) {
            arr[n + 1] = arr[n];
            n--;
        }
        arr[n + 1] = key;
    }
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

    for (i = 0; i < size; i++) {
        if ((int)(arr[i] / min) % 2 == 0) {
            res += sin(arr[i]);
        }
    }
    return res;
}
