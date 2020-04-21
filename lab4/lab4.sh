#!/bin/bash

# dont use the openmp
g++ ./lab4.c -o lab4 
#g++ ./lab4.c -o lab4 -fopenmp

N1=100000 # 100ms
N2=50000000 #2000ms depends on your cp ,change by yourself
DELTA=$(expr $(expr ${N2} - ${N1}) / 10)


#export OMP_SCHEDULE="GUIDED,4"
#export OMP_SCHEDULE="STATIC"
#export OMP_SCHEDULE="AUTO"

#arr=("DYNAMIC,1" "DYNAMIC,2" "DYNAMIC,4" "DYNAMIC" "GUIDED,1"  "GUIDED,2" "GUIDED,4" "STATIC,1" "STATIC,2" "STATIC,4" "STATIC")
arr=("SEQ")
for value in ${arr[@]}
do
    echo $value
    export OMP_SCHEDULE=$value

para=1

while [ ${para} -le 4 ]
do
    N=$N1

    if [ ! -d res ];then
        mkdir res
    fi

    echo "N,TIME,X,GENERATE,MAP,MERGE,SORT,REDUCE">./res/${value}-threads-${para}.csv

    while [ ${N} -le ${N2} ]
    do
        #time: 28.420826 ms; N = 50000; X = nan; threads: 4; generate: 9.493006 ms; 
        #map: 6.558815 ms; sort: 2.968274 ms; reduce: 2.195562 ms 
        RESULT=$(./lab4 ${N} ${para})
        echo "${RESULT} "para="${para}"

        TIME=`echo ${RESULT} | awk '{print $2}'`
        X=`echo ${RESULT} | awk '{print $9}'`

        GENERATE=`echo ${RESULT} | awk '{print $14}'`
        MAP=`echo ${RESULT} | awk '{print $17}'`
        MERGE=`echo ${RESULT} | awk '{print $20}'`
        SORT=`echo ${RESULT} | awk '{print $23}'`
        REDUCE=`echo ${RESULT} | awk '{print $26}'`

        echo "$N,$TIME,$X,$GENERATE,$MAP,$MERGE,$SORT,$REDUCE">>./res/${value}-threads-${para}.csv
        N=$(expr ${N} + $DELTA)

    done
    para=$(expr $para + 1)
done

done



