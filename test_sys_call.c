#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define AVANZATECH_SYS_CALL 451
#define DEST_BUFFER_SIZE 64

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("The format of input is avanzatech <number> <name>\n");
        return EXIT_FAILURE;
    }

    // Convert number
    int number = atoi(argv[1]);

    // Copying the name to new buffer
    size_t name_length = strlen(argv[2]) + 1; // plus one due to null character \0
    char *name = (char *)malloc(name_length);
    if (name == NULL)
    {
        printf("Error allocating memory\n");
        return EXIT_FAILURE;
    }
    strcpy(name, argv[2]);

    // Allocating memory for destinaton
    char dest_buffer[DEST_BUFFER_SIZE];
    size_t dest_len = DEST_BUFFER_SIZE;

    long result = syscall(AVANZATECH_SYS_CALL, number, name, name_length, dest_buffer, dest_len);

    if (result != 0)
    {
        perror("Error in syscall");
        free(name);
        return EXIT_FAILURE;
    }
    printf("%s\n", dest_buffer);
    free(name);
    return EXIT_SUCCESS;
}
