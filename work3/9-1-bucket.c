#include <stdio.h>
#include <stdlib.h>

static int bucket_sort(int values[], size_t count)
{
    int lo;
    int hi;
    int value;
    unsigned int span;
    size_t buckets;
    size_t *frequency;
    size_t i;
    size_t out = 0;
    if (count < 2) {
        return 1;
    }

    lo = values[0];
    hi = values[0];
    for (i = 1; i < count; ++i) {
        if (values[i] < lo) {
            lo = values[i];
        }
        if (values[i] > hi) {
            hi = values[i];
        }
    }

    /* Unsigned subtraction gives the exact nonnegative distance. */
    span = (unsigned int)hi - (unsigned int)lo;
    buckets = (size_t)span;
    if ((unsigned int)buckets != span || buckets >= (size_t)-1 / sizeof *frequency) {
        return 0;
    }
    ++buckets;
    frequency = calloc(buckets, sizeof *frequency);
    if (frequency == NULL) {
        return 0;
    }

    for (i = 0; i < count; ++i) {
        size_t index = (size_t)((unsigned int)values[i] - (unsigned int)lo);
        ++frequency[index];
    }

    value = lo;
    for (i = 0; i < buckets; ++i) {
        size_t copies;
        for (copies = frequency[i]; copies > 0; --copies) {
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
    size_t i;
    if (!bucket_sort(values, count)) {
        fputs("Could not allocate the buckets.\n", stderr);
        return EXIT_FAILURE;
    }
    printf("Vec = {");
    for (i = 0; i < count; ++i) {
        printf("%s%d", i == 0 ? "" : ", ", values[i]);
    }
    puts("}");
    return EXIT_SUCCESS;
}
