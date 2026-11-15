#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int print_valid_runs(const char *text, FILE *out)
{
    size_t n = strlen(text);
    if (n == 0) {
        return 1;
    }
    if (n > SIZE_MAX / sizeof(size_t)) {
        return 0;
    }
    size_t *right = malloc(n * sizeof *right);
    unsigned char *matched = calloc(n, sizeof *matched);
    if (right == NULL || matched == NULL) {
        free(right);
        free(matched);
        return 0;
    }

    /* top is the next free slot in the stack of right parentheses. */
    size_t top = 0;
    for (size_t i = n; i > 0;) {
        --i;
        if (text[i] == ')') {
            right[top++] = i;
        } else if (text[i] == '(' && top != 0) {
            matched[i] = 1;
            matched[right[--top]] = 1;
        }
    }

    size_t i = 0;
    while (i < n) {
        if (!matched[i]) {
            ++i;
            continue;
        }
        size_t start = i;
        while (i < n && matched[i]) {
            ++i;
        }
        fwrite(text + start, 1, i - start, out);
        fprintf(out, "  start=%zu  length=%zu\n", start, i - start);
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
