#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yyparse(FILE *infile);

static int str_endswith(const char *str, const char *suffix) {
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    return (lenstr >= lensuffix) && (strcmp(str + lenstr - lensuffix, suffix) == 0);
}

static int repl() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("sophia> ");
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 1) {
            FILE *infile = fmemopen(line, read, "r");
            if (!infile) {
                fprintf(stderr, "Error: Could not open memory stream\n");
                free(line);
                return 1;
            }
            // fprintf(stderr, "Parsing stdin line: %s\n", line);
            yyparse(infile);
            fclose(infile);
        }
        printf("sophia> ");
    }
    free(line);
    return 0;
}

int main(int argc, char *argv[]) {
    int i;
    FILE *infile;
    int num_translation_units = 0;
    for (i = 1; i < argc; i++) {
        if (str_endswith(argv[i], ".s")) {
            infile = fopen(argv[i], "r");
            if (!infile) {
                fprintf(stderr, "Error: Could not open file %s\n", argv[i]);
                return 1;
            }
            yyparse(infile);
            fclose(infile);
            num_translation_units++;
        } else {
            fprintf(stderr, "Error: Invalid file type %s\n", argv[i]);
            return 1;
        }
    }
    if (num_translation_units == 0) {
        return repl();
    }
    return 0;
}
