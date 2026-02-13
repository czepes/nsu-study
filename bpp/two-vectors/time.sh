#!/bin/bash

make all

cpus=(2 4 8 16 24)

touch time.csv
echo "cpus;consequtive;peer-to-peer;collective" > time.csv

touch acceleration.csv
echo "cpus;consequtive;peer-to-peer;collective" > acceleration.csv

touch efficiency.csv
echo "cpus;consequtive;peer-to-peer;collective" > efficiency.csv

echo "Measuring time for consequtive program"
csq_time=$(./csq | grep time | awk '/time:/ {print $NF}')
echo "Calculation took $csq_time sec."

for n in "${cpus[@]}"; do
  echo "Measuring time for parallel programs with $n cpus"

  ptp_time=$csq_time
  pc_time=$csq_time

  echo "Point-to-point calculations took:"
  for i in {0..4}; do
    exec_time=$(mpiexec -n "$n" ./ptp | grep time | awk '/time:/ {print $NF}')
    if (( $(echo "$exec_time < $ptp_time" | bc -l) )); then
      ptp_time=$exec_time
    fi
    echo "$exec_time sec."
  done
  echo "Point-to-point minimal time is $ptp_time sec."

  echo "Collective calculations took:"
  for i in {0..4}; do
    exec_time=$(mpiexec -n "$n" ./pc | grep time | awk '/time:/ {print $NF}')
    if (( $(echo "$exec_time < $pc_time" | bc -l) )); then
      pc_time=$exec_time
    fi
    echo "$exec_time sec."
  done
  echo "Collective minimal time is $pc_time sec."

  ptp_acc=$(echo "scale=6; $csq_time / $ptp_time" | bc -l)
  pc_acc=$(echo "scale=6; $csq_time / $pc_time" | bc -l)
  csq_eff=$(echo "scale=6; 100 / $n" | bc -l)
  ptp_eff=$(echo "scale=6; ($ptp_acc / $n) * 100" | bc -l)
  pc_eff=$(echo "scale=6; ($pc_acc / $n) * 100" | bc -l)

  echo "$n;$csq_time;$ptp_time;$pc_time;" >> time.csv
  echo "$n;1;$ptp_acc;$pc_acc" >> acceleration.csv
  echo "$n;$csq_eff;$ptp_eff;$pc_eff" >> efficiency.csv
done

make clean
