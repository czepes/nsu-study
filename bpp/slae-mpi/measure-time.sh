#!/bin/bash

#PBS -l select=2:ncpus=12:mpiprocs=12:mem=8000m,place=scatter
#PBS -l walltime=01:00:00
#PBS -m n

cd $PBS_O_WORKDIR

echo "=== GENERATION ==="
./gen
echo "=== EXECUTION TIME ==="
echo "cpus;seq_time;seq_accel;seq_eff;par_time;par_accel;par_eff"

seq_time=$(./seq | grep Time | awk '/Time:/ {print $2}')

cpus=(2 4 8 16 24)
for n in "${cpus[@]}"; do
  if (( n < 16 )); then
    par_time=$(mpirun -n "$n" ./par | grep Time | awk '/Time:/ {print $2}')
  elif (( n == 16 )); then
    par_time=$(mpirun -perhost 8 -n 16 ./par16 | grep Time | awk '/Time:/ {print $2}')
  elif (( n == 24 )); then
    par_time=$(mpirun -perhost 12 -n 24 ./par24 | grep Time | awk '/Time:/ {print $2}')
  fi

  par_acc=$(echo "scale=4; $seq_time / $par_time" | bc -l)
  par_eff=$(echo "scale=4; ($par_acc / $n) * 100" | bc -l)
  seq_eff=$(echo "scale=4; 100 / $n" | bc -l)

  echo "$n;$seq_time;1;$seq_eff;$par_time;$par_acc;$par_eff"
done
