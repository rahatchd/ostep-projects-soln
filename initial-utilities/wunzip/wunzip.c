#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if (1 == argc) {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }

    for (int c = 1; c < argc; c++) {
        FILE* fp = fopen(argv[c], "r");
        if (NULL == fp) {
            printf("wunzip: cannot open file\n");
            return 1;
        }

        size_t ret;
        uint32_t count;
        while (1 == (ret = fread(&count, 4, 1, fp))) {
            const char a = fgetc(fp);
            if (a == EOF) {
                printf("wunzip: unexpected format\n");
                return 1;
            }
            for (int i = 0; i < count; i++) {
                fputc(a, stdout);
            }
        }

        if (0 != fclose(fp)) {
            printf("wunzip: cannot close file\n");
            return 1;
        }
    }
    return 0;
}

