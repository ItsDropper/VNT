#include <stdio.h>

int main(void) {
    /*
     * volatile prevents the C optimizer from replacing this fixed loop with
     * a closed-form sum. This makes the workload closer to the VNT/Python
     * loops, though it is still a simple microbenchmark, not a full language
     * performance test.
     */
    volatile int round = 0;
    volatile int i = 0;
    volatile int total = 0;

    while (round < 20) {
        i = 0;
        total = 0;
        while (i < 40000) {
            total = total + i;
            i = i + 1;
        }
        round = round + 1;
    }

    printf("%d\n", total);
    return 0;
}
