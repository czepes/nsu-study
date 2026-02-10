#!/bin/bash

N=1000000000

echo "========= Time test =========="
echo "|       N = 1000000000       |"
echo "==== Optimization level 0 ===="
time ./p0 "$N" 5
echo "==== Optimization level 1 ===="
time ./p1 "$N" 5
echo "==== Optimization level 2 ===="
time ./p2 "$N" 5
echo "==== Optimization level 3 ===="
time ./p3 "$N" 5
echo "=============================="

N=$(echo "$N * 0.5" | bc -l)

echo "|            0.5N            |"
echo "==== Optimization level 0 ===="
time ./p0 "$N" 5
echo "==== Optimization level 1 ===="
time ./p1 "$N" 5
echo "==== Optimization level 2 ===="
time ./p2 "$N" 5
echo "==== Optimization level 3 ===="
time ./p3 "$N" 5
echo "=============================="

N=$(echo "$N * 3" | bc -l)

echo "|             2N             |"
echo "==== Optimization level 0 ===="
time ./p0 "$N" 5
echo "==== Optimization level 1 ===="
time ./p1 "$N" 5
echo "==== Optimization level 2 ===="
time ./p2 "$N" 5
echo "==== Optimization level 3 ===="
time ./p3 "$N" 5
echo "=============================="
