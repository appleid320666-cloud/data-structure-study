#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int bucket_sort(int values[], size_t count)
{
    if (count < 2) {
        return 1;
    }

    int lo = values[0];
    int hi = values[0];
    for (size_t i = 1; i < count; ++i) {
        if (values[i] < lo) {
            lo = values[i];
        }
        if (values[i] > hi) {
            hi = values[i];
        }
    }

    /* Unsigned subtraction gives the exact nonnegative distance. */
    uintmax_t span = (uintmax_t)hi - (uintmax_t)lo;
    if (span >= SIZE_MAX / sizeof(size_t)) {
        return 0;
    }
    size_t buckets = (size_t)span + 1;
    size_t *frequency = calloc(buckets, sizeof *frequency);
    if (frequency == NULL) {
        return 0;
    }

    for (size_t i = 0; i < count; ++i) {
        size_t index = (size_t)((uintmax_t)values[i] - (uintmax_t)lo);
        ++frequency[index];
    }

    size_t out = 0;
    int value = lo;
    for (size_t i = 0; i < buckets; ++i) {
        for (size_t copies = frequency[i]; copies > 0; --copies) {
            values[out++] = value;
        }
        if (value < hi) {
            ++value;
        }
    }
    free(frequency);
    return 1;
}

int main(void)
{
    int values[] = {7, -1, 6, 9, -3, 5, 6, 4, 2, -12, 8, 0, 4, -3, 6};
    size_t count = sizeof values / sizeof values[0];
    if (!bucket_sort(values, count)) {
        fputs("Could not allocate the buckets.\n", stderr);
        return EXIT_FAILURE;
    }
    printf("Vec = {");
    for (size_t i = 0; i < count; ++i) {
        printf("%s%d", i == 0 ? "" : ", ", values[i]);
    }
    puts("}");
    return EXIT_SUCCESS;
}
