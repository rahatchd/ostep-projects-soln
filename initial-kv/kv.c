#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const char* DATADIR = "database";

enum CommandType {
    PUT = 'p',
    GET = 'g',
    DELETE = 'd',
    CLEAR = 'c',
    ALL = 'a'
};

static char* _next_cmd_token(char** cmd_str) {
    return strsep(cmd_str, ",");
}

static char* _get_key_fn(const char* key) {
    int fnlen = strlen(DATADIR) + strlen(key) + 2;
    char* fn = (char*) malloc(fnlen);
    snprintf(fn, fnlen, "%s/%s", DATADIR, key);
    return fn;
}

static FILE* _open_key(const char* key, const char* mode) {
    char* fn = _get_key_fn(key);
    FILE* fp = NULL;
    fp = fopen(fn, mode);
    free(fn);
    return fp;
}

static void _handle_put(const char* key, const char* value) {
    FILE* fp = _open_key(key, "w");
    if (fp == NULL) return;
    fprintf(fp, "%s", value);
    (void)fclose(fp);
}

static void _handle_get(const char* key) {
    FILE* fp = _open_key(key, "r");
    if (fp == NULL) {
        fprintf(stdout, "%s not found\n", key);
        return;
    }

    char* linep = NULL;
    ssize_t linelen;
    size_t linecap;
    if ((linelen = getline(&linep, &linecap, fp)) == -1) {
        fprintf(stderr, "kv: could not read file\n");
        return;
    }

    printf("%s,%s\n", key, linep);
    free(linep);
    (void)fclose(fp);
}

static void _handle_delete(const char* key) {
    char* key_fn = _get_key_fn(key);
    if (unlink(key_fn) == -1) {
        fprintf(stderr, "kv: could not delete file\n");
    }
    free(key_fn);
}

static void _handle_clear(void) {
    DIR* dirp = NULL;
    if ((dirp = opendir(DATADIR)) != NULL) {
        struct dirent* dp;
        while ((dp = readdir(dirp)) != NULL) {
            if (dp->d_type != DT_REG) continue;
            _handle_delete(dp->d_name);
        }
    }
    (void)closedir(dirp);
}

static void _handle_all(void) {
    DIR* dirp = NULL;
    if ((dirp = opendir(DATADIR)) != NULL) {
        struct dirent* dp;
        while ((dp = readdir(dirp)) != NULL) {
            if (dp->d_type != DT_REG) continue;
            _handle_get(dp->d_name);
        }
    }
    (void)closedir(dirp);
}

static void _handle_command(char* cmd_str) {
    char* cmdp = _next_cmd_token(&cmd_str);

    if (strlen(cmdp) != 1) {
        fprintf(stderr, "kv: invalid command\n");
    }

    char cmd = *cmdp;
    switch (cmd) {
        case PUT: {
            const char* key = _next_cmd_token(&cmd_str);
            if (key == NULL) {
                fprintf(stderr, "kv: invalid command\n");
                return;
            }
            const char* value = _next_cmd_token(&cmd_str);
            if (value == NULL) {
                fprintf(stderr, "kv: invalid command\n");
                return;
            }
            _handle_put(key, value);
            return;
        }
        case GET: {
            const char* key = _next_cmd_token(&cmd_str);
            if (key == NULL) {
                fprintf(stderr, "kv: invalid command\n");
                return;
            }
            _handle_get(key);
            return;
        }
        case DELETE: {
            const char* key = _next_cmd_token(&cmd_str);
            if (key == NULL) {
                fprintf(stderr, "kv: invalid command\n");
                return;
            }
            _handle_delete(key);
            return;
        }
        case CLEAR:
            _handle_clear();
            return;
        case ALL:
            _handle_all();
            return;
        default:
            fprintf(stderr, "kv: invalid command\n");
    }
}

int main(int argc, char* argv[]) {
    struct stat st;
    if (stat(DATADIR, &st) == -1) {
        mkdir(DATADIR, 0755);
    }

    for (int c = 1; c < argc; c++) {
        _handle_command(argv[c]);
    }
    return 0;
}
