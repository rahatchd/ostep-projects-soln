#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

bool same_file(FILE* f1, FILE* f2) {
    if (NULL == f1 || NULL == f2) return false;
    struct stat s1, s2;
    if (0 != fstat(fileno(f1), &s1)) return false;
    if (0 != fstat(fileno(f2), &s2)) return false;
    return (s1.st_ino == s2.st_ino) && (s1.st_dev == s2.st_dev);
}

typedef struct node {
    char* data;
    struct node* prev;
} Node;

Node* append(Node** node, const char* data) {
    Node* ret = (Node*) malloc(sizeof(Node));
    if (NULL == ret) {
        fprintf(stderr, "reverse: malloc failed\n");
        exit(1);
    }

    const size_t datalen = strlen(data);
    ret->data = (char*) malloc((datalen + 1) * sizeof(char));
    if (NULL == ret->data) {
        fprintf(stderr, "reverse: malloc failed\n");
        exit(1);
    }
    strcpy(ret->data, data);
    ret->data[datalen] = '\0';

    ret->prev = node == NULL ? NULL : *node;
    return ret;
}

int main(int argc, char* argv[]) {
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    FILE* infile;
    FILE* outfile;
    if (argc == 3) {
        const char* outfilename = argv[2];
        outfile = fopen(outfilename, "w");
        if (NULL == outfile) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", outfilename);
            exit(1);
        }
    } else {
        outfile = stdout;
    }
    if (argc >= 2) {
        const char* infilename = argv[1];
        infile = fopen(infilename, "r");
        if (NULL == infile) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", infilename);
            exit(1);
        }
    } else {
        infile = stdin;
    }

    if (argc == 3 && same_file(infile, outfile)) {
        fprintf(stderr, "reverse: input and output file must differ\n");
        exit(1);
    }

    char* lineptr = NULL;
    size_t n = 0;
    ssize_t linelen = 0;

    Node* prev = NULL;
    Node* curr = NULL;
    while (EOF != (linelen = getline(&lineptr, &n, infile))) {
        curr = append(&prev, lineptr);
        prev = curr;
    }
    prev = NULL;
    free(lineptr);

    Node* root = curr;
    while (curr != NULL) {
        fprintf(outfile, "%s", curr->data);
        curr = curr->prev;
    }

    while (root != NULL) {
        curr = root->prev;

        free(root->data);
        root->prev = NULL;
        free(root);

        root = curr;
    }

    fclose(infile);
    fclose(outfile);
    return 0;
}

