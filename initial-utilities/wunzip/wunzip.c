#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE *fp;

void unzip(FILE *fp)
{   
    int x;
    char y;
    while (fread(&x, 4, 1, fp) == 1 && fread(&y, 1, 1, fp) == 1)
    {
        for (int i = 0; i < x; i++)
        {
            printf("%c", y);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }
    for(int i=1;i<argc;i++){
    fp = fopen(argv[1], "r");
    if (!fp)
    {
        printf("wunzip: cannot open file\n");
        exit(1);
    }
    unzip(fp);
    fclose(fp);
    }

    return 0;
}
