#!/usr/bin/bash

for i in {1..21}
do
    for (( j=0; j<10; j++ )); do
        if [ $i -lt 10 ];
        then
            ../MC_memetic.out ../instances/CHAO_MCTTRP_0$i.txt
        else
            ../MC_memetic.out ../instances/CHAO_MCTTRP_$i.txt
        fi
        echo "---------------------------------------------------------------"
    done
done