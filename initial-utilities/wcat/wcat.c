#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 512
int main(int argc, char *argv[])
{
    int count = 0;
    while (argv[++count] != NULL)
    {
        FILE *fp = fopen(argv[count], "r");
        if (fp == NULL)
        {
            printf("wcat: cannot open file\n");
            exit(1);
        }
        char buffer[BUFFER_SIZE];
        while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
        {
            printf("%s", buffer);
        }

        fclose(fp);
    }

    return 0;
}