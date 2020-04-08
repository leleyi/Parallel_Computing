#!/bin/bash

if [ ! -f lab2.o ];then
    g++ -m64 -c -I ../lib/FW_1.3.1_Lin64 lab2.c
    g++ -m64 -L ../lib/FW_1.3.1_Lin64/lib/ lab2.o -lfwBase -lfwSignal
fi

export LD_LIBRARY_PATH=../lib/FW_1.3.1_Lin64/lib:$LD_LIBRARY_PATH

N1=500000 # 100ms
N2=70000000 #2000ms depends on your cp ,change by yourself
DELTA=$(expr $(expr ${N2} - ${N1}) / 10)

corenum=`sudo cat /proc/cpuinfo |grep processor|sed '$!d'|awk '{print $3}'`

#exce=a.out
para=1

while [ ${para} -le ${corenum} ]
do
    N=$N1

    if [ ! -d res ];then
        mkdir res
    fi

    echo "N,TIME,X">./res/para-${para}.csv

    while [ ${N} -le ${N2} ]
    do

        RESULT=$(./a.out ${N} ${para})
        echo "${RESULT} "para="${para}"
        TIME=`echo ${RESULT} | awk '{print $2}'`
        X=`echo ${RESULT} | awk '{print $9}'`
        echo "$N,$TIME,$X">>./res/para-${para}.csv
        N=$(expr ${N} + $DELTA)

    done
    para=$(expr $para + 1)
done



