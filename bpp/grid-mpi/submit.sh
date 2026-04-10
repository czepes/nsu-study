#!/bin/bash

#PBS -l select=2:ncpus=12:mpiprocs=12:mem=8000m,place=scatter
#PBS -l walltime=02:00:00
#PBS -m n

echo "=== GENERATION ==="
./gen

echo "=== EXECUTION TIME ==="
rows=(1 1 4 1 8 2 2 4)
cols=(1 4 1 8 1 2 4 2)
for i in "${!rows[@]}"; do
  p1=${rows[$i]}
  p2=${cols[$i]}
  n=$((p1 * p2))
  echo "=== $n = $p1 x $p2 ==="
  mpirun -n $n grid $p1 $p2
done
