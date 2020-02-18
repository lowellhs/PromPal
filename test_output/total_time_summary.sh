#!/bin/bash

procs=$1
flag=$(echo "2 * $2" | bc -l)
shift
shift

for file in $@
do
  time=$(bash total_time_extractor.sh $procs $file | head -n $flag | tail -n -1)
  echo "$time" | awk '{print $2","}'
done
