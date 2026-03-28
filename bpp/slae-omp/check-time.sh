echo "=== GENERATION ==="
./gen

echo "=== EXECUTION TIME ==="
echo "threads_num;time"
threads=(8 16)
for n in "${threads[@]}"; do
  min_time=$(./omp "$n" | awk '/Time:/ {print $2}')
  for i in {0..3}; do
    time=$(./omp "$n" | awk '/Time:/ {print $2}')
    if (($(echo "$time < $min_time" | bc -l))); then
      min_time=$time
    fi
  done
  echo "$n;$min_time"
done

echo "=== SCHEDULE TIME ==="
echo "chunk;static;dynamic;guided;"
chunks=(1 10 100)
schedules=("static" "dynamic" "guided")
for chunk in "${chunks[@]}"; do
  echo -n "$chunk;"
  for schedule in "${schedules[@]}"; do
    min_time=$(./omp 8 "$schedule" "$chunk" | awk '/Time:/ {print $2}')
    for i in {0..3}; do
      time=$(./omp 8 "$schedule" "$chunk" | awk '/Time:/ {print $2}')
      if (($(echo "$time < $min_time" | bc -l))); then
        min_time=$time
      fi
    done
    echo -n "$min_time;"
  done
  echo ""
done
