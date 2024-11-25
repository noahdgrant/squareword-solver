#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "parser.h"

static void args_init(Args *args) {
    args->solve = true;
    args->find_minimum = true;
}

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [options]\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help              Display this help message and exit\n");
    fprintf(stderr, "  -l, --log <log_level>   Set the log level (TRACE, DEBUG, INFO, WARNING, ERROR, FATAL)\n");
    fprintf(stderr, "  -s, --solve             Only find all possible solutions, do not try to find minimum solutions\n");
    fprintf(stderr, "  -m, --find-minimum      Only try to find minimum solutions, do not find all possible solutions\n");
}

Args parse_args(int argc, char* argv[]) {
    int opt;
    Args args;

    static struct option long_options[] = {
        {"log", required_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {"solve", no_argument, 0, 's'},
        {"find-minimum", no_argument, 0, 'm'},
        {0, 0, 0, 0}
    };

    args_init(&args);

    while ((opt = getopt_long(argc, argv, "l:hsm", long_options, NULL)) != -1) {
        switch (opt) {
            case 'l':
                logger_set_level(logger_level_from_string(optarg));
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            case 's':
                // Don't want to try and find minimum solutions
                args.find_minimum = false;
                break;
            case 'm':
                // Don't want to solve
                args.solve = false;
                break;
            default:
                print_usage(argv[0]);
                exit(1);
        }
    }

    return args;
}
