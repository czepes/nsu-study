#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

long long calc_fountain(int size, long long top, long long max_sum) {
    long long sum = top;
    long long mul = 0;

    for (int i = 1; i <= size / 2; i++) {
        mul += 8;
        sum += (top - i) * mul;
        if (sum < 0) return max_sum + 1;
    }

    return sum;
}

long long fountain(int size, long long max_sum) {
    if (size == 1) return max_sum;

    long long l = size / 2 + 1, m, r = max_sum / 9 + 1;
    long long cur_sum;

    while (l < r) {
        m = (r + l + 1) / 2;
        cur_sum = calc_fountain(size, m, max_sum);    

        if (cur_sum == max_sum) {
            return m;
        }

        if (cur_sum > max_sum) {
            r = m - 1;
        } else {
            l = m;
        }
    }

    cur_sum = calc_fountain(size, r, max_sum);

    return cur_sum <= max_sum ? r : 0;
}

int main(void) {
    int size;
    long long max_sum;

    if (scanf("%d %lld", &size, &max_sum) != 2) {
        return EXIT_FAILURE;
    }

    long long max_top = fountain(size, max_sum);

    if (max_top < size / 2 + 1) {
        printf("Impossible\n");
    } else {
        printf("%lld\n", max_top);
    }

    return EXIT_SUCCESS;
}
