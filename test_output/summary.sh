env=$1
out="OUT.file"
echo > $out
for i in 0 1
do
  for j in 0 1 2 3 4 5
  do
    python3 total_time_extractor.py $env $i $j >> $out
    echo "" >> $out
  done
  echo "----------------------------------------------" >> $out
done