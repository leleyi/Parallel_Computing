#!/bin/bash

g++ ./lab3.c -o lab3 -fopenmp

N1=100000 # 100ms
N2=50000000 #2000ms depends on your cp ,change by yourself
DELTA=$(expr $(expr ${N2} - ${N1}) / 10)



#export OMP_SCHEDULE="GUIDED,4"
#export OMP_SCHEDULE="STATIC"
#export OMP_SCHEDULE="AUTO"

arr=("DYNAMIC,1" "DYNAMIC,1000" "DYNAMIC" "GUIDED,1" "GUIDED,4" "STATIC,1" "STATIC,1000" "STATIC")
#arr=("DYNAMIC" "STATIC")
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

    echo "N,TIME,X">./res/${value}-threads-${para}.csv

    while [ ${N} -le ${N2} ]
    do

        RESULT=$(./lab3 ${N} ${para})
        echo "${RESULT} "para="${para}"
        TIME=`echo ${RESULT} | awk '{print $2}'`
        X=`echo ${RESULT} | awk '{print $9}'`
        echo "$N,$TIME,$X">>./res/${value}-threads-${para}.csv
        N=$(expr ${N} + $DELTA)

    done
    para=$(expr $para + 1)
done

done


