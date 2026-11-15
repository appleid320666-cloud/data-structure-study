#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { PATTERNS = 200, MAX_PATTERN = 30, REPEATS = 3 };

static int brute_b(const char *text, int n, const char *pattern, int m)
{
    int i = 0;
    int j = 0;
    while (i < n && j < m && i - j <= n - m) {
        if (text[i] == pattern[j]) {
            ++i;
            ++j;
        } else {
            i = i - j + 1;
            j = 0;
        }
    }
    return j == m ? i - j : -1;
}

static void build_nnext(const char *pattern, int m, int next[])
{
    if (m == 0) {
        return;
    }
    next[0] = -1;
    int j = 0;
    int t = -1;
    while (j < m - 1) {
        if (t < 0 || pattern[j] == pattern[t]) {
            ++j;
            ++t;
            next[j] = pattern[j] != pattern[t] ? t : next[t];
        } else {
            t = next[t];
        }
    }
}

static int kmp(const char *text, int n, const char *pattern, int m,
               const int next[])
{
    int i = 0;
    int j = 0;
    while (i < n && j < m) {
        if (j < 0 || text[i] == pattern[j]) {
            ++i;
            ++j;
        } else {
            j = next[j];
        }
    }
    return j == m ? i - j : -1;
}

/* Xorshift32 with a fixed nonzero initial seed. */
static uint32_t random32(uint32_t *state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static double run_batch(const char *text, int n,
                        char patterns[PATTERNS][MAX_PATTERN], int m,
                        int use_kmp, int result[PATTERNS])
{
    clock_t start = clock();
    for (int p = 0; p < PATTERNS; ++p) {
        if (use_kmp) {
            int next[MAX_PATTERN];
            build_nnext(patterns[p], m, next);
            result[p] = kmp(text, n, patterns[p], m, next);
        } else {
            result[p] = brute_b(text, n, patterns[p], m);
        }
    }
    clock_t end = clock();
    if (start == (clock_t)-1 || end == (clock_t)-1 || end <= start) {
        return -1.0;
    }
    return 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
}

static double median3(double a, double b, double c)
{
    if (a > b) {
        double temporary = a;
        a = b;
        b = temporary;
    }
    return c < a ? a : (c < b ? c : b);
}

static int measure(const char *text, int n, int m)
{
    char patterns[PATTERNS][MAX_PATTERN];
    int first = n - n / 10 - m;
    int stride = (n / 10) / PATTERNS;
    for (int p = 0; p < PATTERNS; ++p) {
        memcpy(patterns[p], text + first + p * stride, (size_t)m);
    }

    double time[2][REPEATS];
    int positions[2][PATTERNS];
    for (int r = 0; r < REPEATS; ++r) {
        for (int order = 0; order < 2; ++order) {
            int algorithm = (r + order) % 2;
            time[algorithm][r] = run_batch(text, n, patterns, m,
                                           algorithm, positions[algorithm]);
            if (time[algorithm][r] < 0.0) {
                return 0;
            }
        }
        for (int p = 0; p < PATTERNS; ++p) {
            int hit = positions[0][p];
            if (hit < 0 || hit > first + p * stride || hit != positions[1][p]
                || memcmp(text + hit, patterns[p], (size_t)m) != 0) {
                return 0;
            }
        }
    }
    printf("%d,%d,%d,%.3f,%.3f\n", n, m, PATTERNS,
           median3(time[0][0], time[0][1], time[0][2]),
           median3(time[1][0], time[1][1], time[1][2]));
    return 1;
}

int main(void)
{
    enum { MAX_TEXT = 1000000 };
    const int lengths[] = {100000, 250000, 500000, 750000, MAX_TEXT};
    const int sizes[] = {15, 20, 25, 30};
    char *text = malloc(MAX_TEXT);
    if (text == NULL) {
        return EXIT_FAILURE;
    }
    uint32_t seed = UINT32_C(20261115);
    for (int i = 0; i < MAX_TEXT; ++i) {
        text[i] = random32(&seed) % 100U < 60U ? 'a' : 'b';
    }

    puts("N,M,patterns,brute_b_ms,kmp_ms");
    for (size_t i = 0; i < sizeof lengths / sizeof lengths[0]; ++i) {
        for (size_t j = 0; j < sizeof sizes / sizeof sizes[0]; ++j) {
            if (!measure(text, lengths[i], sizes[j])) {
                fputs("Timing or match validation failed.\n", stderr);
                free(text);
                return EXIT_FAILURE;
            }
        }
    }
    free(text);
    return EXIT_SUCCESS;
}
