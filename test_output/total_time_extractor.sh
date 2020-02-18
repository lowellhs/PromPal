#!/bin/bash

procs=$1
file=$2

point2pointrow=25
tt1=$(echo "$point2pointrow + 2 + 2*($procs-1) + 1" | bc -l)
tt2=$(echo "$tt1 + 2 + 2*($procs-1) + 1" | bc -l)
tt3=$(echo "$tt2 + 2 + 2*($procs-1) + 1" | bc -l)
head -n $point2pointrow $file | tail -n 1
min=$((
  head -n $tt1 $file | tail -n 1 && \
  head -n $tt2 $file | tail -n 1 && \
  head -n $tt3 $file | tail -n 1
) | awk 'BEGIN{a=1000}{if ($4<0+a) a=$4} END{print a}')
echo "=> ${min} s"
# echo ""

point2pointcol=$(echo "$tt3 + 2" | bc -l)
tt1=$(echo "$point2pointcol + 2 + 2*($procs-1) + 1" | bc -l)
tt2=$(echo "$tt1 + 2 + 2*($procs-1) + 1" | bc -l)
tt3=$(echo "$tt2 + 2 + 2*($procs-1) + 1" | bc -l)
head -n $point2pointcol $file | tail -n 1
min=$((
  head -n $tt1 $file | tail -n 1 && \
  head -n $tt2 $file | tail -n 1 && \
  head -n $tt3 $file | tail -n 1
) | awk 'BEGIN{a=1000}{if ($4<0+a) a=$4} END{print a}')
echo "=> ${min} s"
# echo ""

point2pointmat=$(echo "$tt3 + 2" | bc -l)
tt1=$(echo "$point2pointmat + 2 + 2*($procs-1) + 1" | bc -l)
tt2=$(echo "$tt1 + 2 + 2*($procs-1) + 1" | bc -l)
tt3=$(echo "$tt2 + 2 + 2*($procs-1) + 1" | bc -l)
head -n $point2pointmat $file | tail -n 1
min=$((
  head -n $tt1 $file | tail -n 1 && \
  head -n $tt2 $file | tail -n 1 && \
  head -n $tt3 $file | tail -n 1
) | awk 'BEGIN{a=1000}{if ($4<0+a) a=$4} END{print a}')
echo "=> ${min} s"
# echo ""

collectiverow=$(echo "$tt3 + 2" | bc -l)
tt1=$(echo "$collectiverow + 2 + ($procs) + 1" | bc -l)
tt2=$(echo "$tt1 + 2 + ($procs) + 1" | bc -l)
tt3=$(echo "$tt2 + 2 + ($procs) + 1" | bc -l)
head -n $collectiverow $file | tail -n 1
min=$((
  head -n $tt1 $file | tail -n 1 && \
  head -n $tt2 $file | tail -n 1 && \
  head -n $tt3 $file | tail -n 1
) | awk 'BEGIN{a=1000}{if ($4<0+a) a=$4} END{print a}')
echo "=> ${min} s"
# echo ""

collectivecol=$(echo "$tt3 + 2" | bc -l)
tt1=$(echo "$collectivecol + 2 + ($procs) + 1" | bc -l)
tt2=$(echo "$tt1 + 2 + ($procs) + 1" | bc -l)
tt3=$(echo "$tt2 + 2 + ($procs) + 1" | bc -l)
head -n $collectivecol $file | tail -n 1
min=$((
  head -n $tt1 $file | tail -n 1 && \
  head -n $tt2 $file | tail -n 1 && \
  head -n $tt3 $file | tail -n 1
) | awk 'BEGIN{a=1000}{if ($4<0+a) a=$4} END{print a}')
echo "=> ${min} s"
# echo ""

collectivemat=$(echo "$tt3 + 2" | bc -l)
tt1=$(echo "$collectivemat + 2 + ($procs) + 1" | bc -l)
tt2=$(echo "$tt1 + 2 + ($procs) + 1" | bc -l)
tt3=$(echo "$tt2 + 2 + ($procs) + 1" | bc -l)
head -n $collectivemat $file | tail -n 1
min=$((
  head -n $tt1 $file | tail -n 1 && \
  head -n $tt2 $file | tail -n 1 && \
  head -n $tt3 $file | tail -n 1
) | awk 'BEGIN{a=1000}{if ($4<0+a) a=$4} END{print a}')
echo "=> ${min} s"
# echo ""