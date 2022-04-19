#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        puts("Not a valid use of ./jem!");
        return 0;
    }
    char *command = argv[1];
    if (!strcmp(command, "add"))
    { // String.h returns 0 when true, so use ! to get 1
        puts("add");
    }
    else if (!strcmp(command, "commit"))
    {
        puts("commit");
    }
    else if (!strcmp(command, "init"))
    {
        puts("init");
    }
    else if (!strcmp(command, "checkout"))
    {
        puts("checkout");
    }
    return 0;
}