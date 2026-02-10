N=2048
M=10
echo "Building..."
make > /dev/null
echo ""
echo "Naive:"
time ./naive $N $M > /dev/null
echo ""
echo "SSE:"
time ./sse $N $M > /dev/null
echo ""
echo "AVX:"
time ./avx $N $M > /dev/null
echo ""
echo "OpenBLAS:"
time ./blas $N $M > /dev/null
echo ""
echo "Cleaning..."
make clean > /dev/null
