#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{

    int i, N,j;

    struct timeval T1, T2;

    long delta_ms;

    N = atoi(argv[1]); /* N等于第一个命令行参数N equals the first command-line parameter */

    //N = 10000;

    gettimeofday(&T1, NULL); /* 记住当前时间T1 remember the current time T1 */

    unsigned int seed = 0;

    for (i = 0; i < 50; i++) /* 50 experiments50 experiments */
    {
        double M1[N] ,M2[(int)(N/2)];
        srand(i); /* 初始化RNG的初始值 initialize the initial value of the RNG*/
        int A = 12;
        for(j = 0; j < N; j++){/* 用N个数据填充初始数组 Fill the initial data array with size N */
            M1[j] = 1 + rand_r(&seed) % A;
        }
        for(j = 0; j < N/2; j++){
            M2[j] = A + rand_r(&seed)%(A*9);
        }
        /* 完成任务，用结果填充数组 Complete the task, fill in the array with the results */

        /* Map Stage. Apply  Cubic root after division by e */
        for(j = 0; j < N; j++){
            M1[j] = pow(M1[j] / exp(1), 1.0 / 3);
        }
        /* in the M2 array, add each element in turn with the previous one ,and apply Natural logarithm of the tangent modulus*/
        double M2_temp[(int)(N/2)+1];
        M2_temp[0] = 0;
        for(j = 1; j < N/2+1; j++){
            M2_temp[j] = M2[j-1];
        }
        for(j = 0; j < N/2; j++){
            M2[j] = log(tan(M2_temp[j]+M2_temp[j+1]));
        }

        /* Merge Stage. In arrays M1 and M2 apply Raising to a power to all elements in pairs with the same indexes in */
        for(j = 0; j < N/2; j++){
            M2[j] = pow(M1[j],M2[j]);
        }

        /* 通过指定的方法对数组和结果进行排序 Sort the array with the results by the specified method */
        /* insert sort */
	int m,n,key;
        for (m=1;m< N ;m++){
                key = M1[m];
                n=m-1;
                while((n>=0) && (M1[n]>key)) {
                        M1[n+1] = M1[n];
                        n--;
                }
                M1[n+1] = key;
        }


        /* Reduce Stage. determining parity */
        double min = A*10;
        for(j = 0; j < N/2; j++){/* Find the minimum */
            if(min > M2[j] && M2[j]!=0)
                min = M2[j];
        }

        //printf("min: %f\n",min);
        double sum = 0;
        for(j = 0; j < N/2; j++){
	//printf("M[j]: %f", M2[j]);
            int tmp = (int)(M2[j]/min);
            if(tmp%2==0)
                sum = sum + sin(M2[j]);
        }
        printf("i->%d,X->%f\n",i,sum);

    }
    gettimeofday(&T2, NULL);

    /* remember the current time T2 */

    delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;

    printf("N=%d. Milliseconds passed: %ld \n", N, delta_ms); /* T2 -T1 */
    return delta_ms;
}
