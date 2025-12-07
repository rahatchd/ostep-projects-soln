#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void grep_file(const char* searchterm, int searchlen, FILE* fp) {
    char* lineptr = NULL;
    size_t n;
    bool found = false;
    ssize_t linelen;
    while (-1 != (linelen = getline(&lineptr, &n, fp))) {
        if (searchlen > linelen) continue;

        for (int i = 0; i < linelen - searchlen; i++) {
            found = true;
            for (int ii = 0; ii < searchlen; ii++) {
                if (searchterm[ii] != lineptr[i + ii]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                break;
            }
        }

        if (found) {
            printf("%s", lineptr);
        }
    }
    free(lineptr);
}

int main(int argc, char* argv[]) {
    if (1 == argc) {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }

    const char* searchterm = argv[1];
    const size_t searchlen = strlen(searchterm);
    if (0 == searchlen) {
        return 0;
    }
    if (2 == argc) {
        grep_file(searchterm, searchlen, stdin);
        return 0;
    }
    for (int c = 2; c < argc; c++) {
        const char* fn = argv[c];
        FILE* fp = fopen(fn, "r");
        if (NULL == fp) {
            printf("wgrep: cannot open file\n");
            exit(1);
        }

        grep_file(searchterm, searchlen, fp);

        if (0 != fclose(fp)) {
            printf("wgrep: cannot close file\n");
        }
    }

    return 0;
}
