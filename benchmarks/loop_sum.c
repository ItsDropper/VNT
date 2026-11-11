#include <stdint.h>
#include <stdio.h>

int main(void) {
    int64_t round = 0;
    int64_t i = 0;
    int64_t total = 0;

    while (round < 20) {
        i = 0;
        total = 0;
        while (i < 40000) {
            total = total + i;
            i = i + 1;
            /* Keep the loop observable without forcing every value to memory. */
#if defined(__GNUC__) || defined(__clang__)
            __asm__ __volatile__("" : "+r"(i), "+r"(total) : : "memory");
#endif
        }
        round = round + 1;
    }

    printf("%lld\\n", (long long)total);
    return 0;
}
