#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_size(size_t value, FILE *out)
{
    char digits[sizeof value * CHAR_BIT];
    size_t length = 0;
    do {
        digits[length++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    while (length != 0) {
        fputc(digits[--length], out);
    }
}

static int print_valid_runs(const char *text, FILE *out)
{
    size_t n = strlen(text);
    size_t *right;
    unsigned char *matched;
    size_t top = 0;
    size_t i;
    size_t start;
    if (n == 0) {
        return 1;
    }
    if (n > (size_t)-1 / sizeof *right) {
        return 0;
    }
    right = malloc(n * sizeof *right);
    matched = calloc(n, sizeof *matched);
    if (right == NULL || matched == NULL) {
        free(right);
        free(matched);
        return 0;
    }

    /* top is the next free slot in the stack of right parentheses. */
    for (i = n; i > 0;) {
        --i;
        if (text[i] == ')') {
            right[top++] = i;
        } else if (text[i] == '(' && top != 0) {
            matched[i] = 1;
            matched[right[--top]] = 1;
        }
    }

    i = 0;
    while (i < n) {
        if (!matched[i]) {
            ++i;
            continue;
        }
        start = i;
        while (i < n && matched[i]) {
            ++i;
        }
        fwrite(text + start, 1, i - start, out);
        fputs("  start=", out);
        print_size(start, out);
        fputs("  length=", out);
        print_size(i - start, out);
        fputc('\n', out);
    }

    free(matched);
    free(right);
    return !ferror(out);
}

int main(void)
{
    const char text[] = "())(())((";
    if (!print_valid_runs(text, stdout)) {
        fputs("Could not process parentheses.\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
