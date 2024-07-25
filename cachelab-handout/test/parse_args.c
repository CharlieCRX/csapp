#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Function to parse command line arguments
 */
void parse_args(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "a:b:c:")) != -1) {
        switch (opt) {
            case 'a':
                printf("Option a with value %s\n", optarg);
                break;
            case 'b':
                printf("Option b with value %s\n", optarg);
                break;
            case 'c':
                printf("Option c with value %s\n", optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -a value -b value -c value\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);

    // Other code to run the program
    printf("Program is running...\n");

    return 0;
}
