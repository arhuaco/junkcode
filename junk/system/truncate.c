#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
 * truncate: truncate a file
 *
 * nelsoneci@gmail.com.
 *
 * What a program! :)
 *
 * TODO: Allow file_name to be specified with  "k", "M", "G", "ik", "iM", "iG".
 *
 */

#define PROGRAM_NAME "truncate"

int main(int argc, char *argv[])
{
    char *num_str;
    char *file_name;
    long int new_size;
    char *end_ptr;

    if (argc != 3) {
        fprintf(stderr, "usage: %s file new-size-in-bytes\n",
                PROGRAM_NAME);
        exit(EXIT_FAILURE);
    }

    num_str = argv[2];
    file_name = argv[1];

    new_size = strtol(num_str, &end_ptr, 10);

    if (!*num_str || *end_ptr) {
        fprintf(stderr, "'%s' is not a valid file size\n", num_str);
        exit(EXIT_FAILURE);
    }

    if (truncate(file_name, new_size)) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
