#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

int str_endswith(const char *str, const char *suffix) {
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    return (lenstr >= lensuffix) && (strcmp(str + lenstr - lensuffix, suffix) == 0);
}

int main(int argc, char *argv[]) {
    int i = 0;
    FILE *infile = NULL;
    int num_translation_units = 0;
    // char *output_file = NULL;
    struct VM *vm = vm_new();
    int exit_code = EXIT_SUCCESS;
    int r = 0;
    for (i = 1; i < argc; i++) {
        if (str_endswith(argv[i], ".sophia")) {
            infile = fopen(argv[i], "r");
            if (!infile) {
                fprintf(stderr, "Error: Could not open file %s\n", argv[i]);
                return 1;
            }
            r = vm_eval_file(vm, infile);
            if (r != 0) {
                fprintf(stderr, "Error: Failed to evaluate file %s\n", argv[i]);
                exit_code = 2;
            }
            fclose(infile);
            num_translation_units++;
        }
        else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                vm_translate(vm, argv[++i]);
            } else {
                fprintf(stderr, "Error: -o option requires an argument\n");
                return EXIT_FAILURE;
            }
        }
        else {
            fprintf(stderr, "Error: Invalid file type %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }
    if (num_translation_units == 0) {
        return vm_repl(vm);
    }
    return exit_code;
}
