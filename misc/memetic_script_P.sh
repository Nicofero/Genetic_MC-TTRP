#!/usr/bin/bash

for i in {1..8}
do
    for (( j=0; j<10; j++ )); do
        ../MC_memetic.out ../instances/P$i.csv
        echo "---------------------------------------------------------------"
        sleep 1
    done
done