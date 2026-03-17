#!/bin/bash

#PBS -l select=2:ncpus=12:mpiprocs=12:mem=8000m,place=scatter
#PBS -l walltime=01:00:00
#PBS -m n

cd $PBS_O_WORKDIR

echo "=== GENERATION ==="
./gen
echo "=== SEQUENTIAL ==="
./seq
echo "=== PARALLEL-2 ==="
mpirun -n 2 ./par
echo "=== PARALLEL-4 ==="
mpirun -n 4 ./par
echo "=== PARALLEL-8 ==="
mpirun -n 8 ./par
echo "=== PARALLEL-16 ==="
mpirun -perhost 8 -n 16 ./par16
echo "=== PARALLEL-24 ==="
mpirun -perhost 12 -n 24 ./par24

