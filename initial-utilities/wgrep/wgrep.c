#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 512
char *line = NULL;
size_t len = 0;
FILE *fp;
int main(int argc, char *argv[])
{
    if(argc==1){
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    if(argc==2){
        while(getline(&line, &len, stdin)!=-1){
          if(strstr(line,argv[1])!=NULL){
            printf("%s",line);
          }
        }
        exit(0);
    }
    int count = 1;
    while (argv[++count] != NULL)
    {
        fp= fopen(argv[count], "r");
        if (fp == NULL)
        {
            printf("wgrep: cannot open file\n");
            exit(1);
        }
        while (getline(&line, &len, fp) != -1)
        {
          if(strstr(line,argv[1])!=NULL){
            printf("%s",line);
          }
        }
        fclose(fp);
    }
    free(line);
    

    return 0;
}