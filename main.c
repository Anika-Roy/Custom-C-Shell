#include "headers.h"
#include "prompt.h"
#include "handle_input_commands.h"

int main()
{
    // Keep accepting commands
    char store_calling_directory[1024];
    getcwd(store_calling_directory, sizeof(store_calling_directory));

    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(store_calling_directory);
        char input[4096];
        fgets(input, 4096, stdin);
        // printf("%s", input);
        handle_input_commands(input);
    }
}
