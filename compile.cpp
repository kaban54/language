#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
    for (int index = 1; index < argc; index++)
    {   
        char cmd [128] = "front.exe ";
        strcat (cmd, argv[index]);
        strcat (cmd, " treesave");

        system (cmd);
        system ("back.exe treesave asm.txt");
    }
    return 0;
}