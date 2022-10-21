#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 64
char error_message[30] = "An error has occurred\n";
size_t inpsize;
FILE *file;
char *inputs[SIZE];
char **PATH;
int PATHSZ = 2;
char *line;

void process(int count)
{
    int rc = fork();
    if (rc == 0)
    {
        if (PATHSZ == 1)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
        for (int i = 0; i < PATHSZ - 1; i++)
        {
            char *command = malloc(sizeof(char) * SIZE);
            strcpy(command, PATH[i]);
            if (command[strlen(command) - 1] != '/')
            {
                strcat(command, "/");
            }
            strcat(command, inputs[0]);
            if (access(command, X_OK) == 0)
            {
                execv(command, inputs);
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
            else
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
        }
    }
    else if (rc < 0)
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else
    {
        rc = (int)wait(NULL);
    }
}
void builtin(int count)
{
    if (strcmp(inputs[0], "exit") == 0)
    {
        if (count >= 2)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else
        {
            exit(0);
        }
    }
    else if (strcmp(inputs[0], "cd") == 0)
    {
        if (count == 1 || count > 2 || chdir(inputs[1]) != 0)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
    else if (strcmp(inputs[0], "path") == 0)
    {
        PATHSZ = count;
        for (int i = 1; i < count; i++)
        {
            PATH = realloc(PATH, count * sizeof(char *));
            int length = strlen(inputs[i]);
            PATH[i - 1] = malloc(sizeof(char) * length);
            strcpy(PATH[i - 1], inputs[i]);
        }
    }
    else
    {
        redirexist(count);
        // process(count);
    }
}

int checkredir(int numchar)
{
    int dir = 0;
    for (int r = 0; r < numchar; r++)
    {
        if (line[r] == '>')
        {
            printf("X");
            dir++;
            if (dir > 1 || r == 0)
            {
                return -1;
            }
        }
    }
    if (dir == 1)
    {
        return 1;
    }
    return 0;
}
int separate(char **line)
{
    char *c;
    int count = 0;

    while ((c = strsep(line, " \n\t\r>")) != NULL)
    {
        if (strcmp(c, "") == 0)
        {
            continue;
        }
        inputs[count] = c;
        count++;
    }
    inputs[count] = NULL;
    return count;
}
int checkrflag(int rflag)
{
    if (rflag == -1)
    {
    }
}
void rread(int mode)
{
    int count;
    int numchar;
    int rflag;
    if (mode == 1)
    {
        if ((numchar = getline(&line, &inpsize, stdin)) != -1)
        {
            rflag = checkredir(numchar);
            count = separate(&line);
            if (count == 0)
            {
                return;
            }
            builtin(count);
        }
    }
    else
    {
        while ((numchar = getline(&line, &inpsize, file)) != -1)
        {
            rflag = checkredir(numchar);
            count = separate(&line);
            builtin(count);
        }
    }
}
int main(int argc, char *argv[])
{
    PATH = malloc(sizeof(char *));
    PATH[0] = malloc(sizeof(char) * SIZE);
    strcpy(PATH[0], "/bin");
    if (argc > 1)
    {
        file = fopen(argv[1], "r");
        if (file == NULL)
        {
            printf("FILE NOT FOUND");
            exit(0);
        }
        rread(0);
        return 0;
    }
    while (1)
    {

        printf("wish> ");
        rread(1);
        // printf("%d",count);
        // while(count--){
        //     printf("%s\n",inputs[count]);
        // }
        // argv_inputs[count]=NULL;
        // if(strcmp(argv_inputs[0],"exit")==0){
        //     exit(0);
        // }else if(strcmp(argv_inputs[0],"cd")==0){
        //         if(count>2 || chdir(argv_inputs[1])!=0){
        //              write(STDERR_FILENO, error_message, strlen(error_message));
        // }
        // }

        // int a= fork();
        // if(a==0){
        //     if(strcmp(argv_inputs[0],"ls")==0){

        //         execvp(argv_inputs[0],argv_inputs);
        //         printf("%s",error_message);
        //     }
        // }else{
        //   a= (int) wait(NULL);
        // }
    }

    return 0;
}

// abcd > def  ls xyz  cd a