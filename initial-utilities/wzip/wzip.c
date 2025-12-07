#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

void write_rle(const char runchar, uint32_t runlen) {
    fwrite(&runlen, 4, 1, stdout);
    fputc(runchar, stdout);
}

int main(int argc, char* argv[]) {
    if (1 == argc) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    char prev = 0;
    uint32_t count = 1;
    for (int c = 1; c < argc; c++) {
        const char* fn = argv[c];
        FILE* fp = fopen(fn, "r");
        if (NULL == fp) {
            printf("wzip: cannot open file\n");
            return 1;
        }

        char curr;
        while (EOF != (curr = fgetc(fp))) {
            if (prev == 0) {
                prev = curr;
            } else if (curr == prev) {
                count++;
            } else {
                write_rle(prev, count);
                prev = curr;
                count = 1;
            }
        }

        if (0 != fclose(fp)) {
            printf("wzip: cannot close file\n");
            return 1;
        }
    }

    write_rle(prev, count);
    return 0;
}
