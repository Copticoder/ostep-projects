#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 64
char error_message[30] = "An error has occurred\n";
size_t inpsize;
FILE *file;
char *commands[SIZE];
char **PATH;
int PATHSZ = 2;
char *command[SIZE];
char *operation[SIZE]; 
int redir;
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
            strcat(command, commands[0]);
            if (access(command, X_OK) == 0)
            {
                execv(command, commands);
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
    if (strcmp(commands[0], "exit") == 0)
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
    else if (strcmp(commands[0], "cd") == 0)
    {
        if (count == 1 || count > 2 || chdir(commands[1]) != 0)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
    else if (strcmp(commands[0], "path") == 0)
    {
        PATHSZ = count;
        for (int i = 1; i < count; i++)
        {
            PATH = realloc(PATH, count * sizeof(char *));
            int length = strlen(commands[i]);
            PATH[i - 1] = malloc(sizeof(char) * length);
            strcpy(PATH[i - 1], commands[i]);
        }
    }
    else
    {
        // redirexist(count);
        process(count);
    }
}

int checkredir(char com[])
{   
    int n=0;
    for(int i=0;i<strlen(com);i++){
        if(com[i]=='>'){
            n++;
            if(n>1 || (n>=1 && (i==0||i==strlen(com)-2))){
                return -1;
            }
            
        }
    }
    return n;
}
int separate_commands(char *commandd[],char *cmd[],char *delim)
{
    char *c;
    int count = 0;

    while ((c = strsep(cmd, delim)) != NULL)
    {
        if (strcmp(c, "") == 0)
        { 
            continue;
        }
        commandd[count] = c;
        printf("%s\n",commandd[count]);
        count++;
    }
    command[count] = NULL;
    return count;
}
void make_spaces(char line[],char *newLine){
    int c = 0;
    for(int i = 0; i < strlen(line); i++) {
        if(line[i] == '>' || line[i] == '|') {
            newLine[c++] = ' ';
            newLine[c++] = line[i];
            newLine[c++] = ' ';
            continue;
        }
        newLine[c++] = line[i];
    }
    newLine[c++] = '\0';

}
void rread(int mode)
{
    int count;
    char *line;
    if (mode == 1)
    {
        if (getline(&line, &inpsize, stdin) != -1)
        {   
            char *newLine = (char *) malloc ((strlen(line) + SIZE) * sizeof(char));
            make_spaces(line,newLine);
            
            count = separate_commands(commands,&newLine,"&");
            for(int i=0;i<count;i++){
                redir=checkredir(commands[i]);
                int ndcount = separate_commands(command,&commands[i]," \t\n\r");
                if(redir==-1){
                  write(STDERR_FILENO, error_message, strlen(error_message));
                  return;
                }
                if(redir == 1){
                    for(int l=0;l<ndcount;l++){
                        if(strcmp(command[l],">")==0){
                            if(l<ndcount-2){
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                return;
                            }
                        }
                    }
                }
            }
            // }
            // builtin(count);
        }
    }
    else
    {
        while (getline(&line, &inpsize, file) != -1)
        {
            // count = separate_commands(,&line," \n\t\r");
            // builtin(count);
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
        //     printf("%s\n",commands[count]);
        // }
        // argv_commands[count]=NULL;
        // if(strcmp(argv_commands[0],"exit")==0){
        //     exit(0);
        // }else if(strcmp(argv_commands[0],"cd")==0){
        //         if(count>2 || chdir(argv_commands[1])!=0){
        //              write(STDERR_FILENO, error_message, strlen(error_message));
        // }
        // }

        // int a= fork();
        // if(a==0){
        //     if(strcmp(argv_commands[0],"ls")==0){

        //         execvp(argv_commands[0],argv_commands);
        //         printf("%s",error_message);
        //     }
        // }else{
        //   a= (int) wait(NULL);
        // }
    }

    return 0;
}

// ls -l -a > > x.txt