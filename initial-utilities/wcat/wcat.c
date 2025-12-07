#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        exit(0);
    }

    for (int c = 1; c < argc; c++) {
        const char* fn = argv[c];
        FILE* fp = fopen(fn, "r");
        if (fp == NULL) {
            printf("wcat: cannot open file\n");
            exit(1);
        }

        int buffersize = 50;
        char buffer[buffersize];
        while (NULL != fgets(buffer, buffersize, fp)) {
            if (errno != 0) {
                printf("error reading file '%s': %s\n", fn, strerror(errno));
                exit(1);
            }
            printf("%s", buffer);
        }

        if (0 != fclose(fp)) {
            printf("error closing file '%s': %s\n", fn, strerror(errno));
            exit(1);
        }
    }

    return 0;
}
