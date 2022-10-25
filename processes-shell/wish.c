#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE 64

char error_message[30] = "An error has occurred\n";
size_t inpsize;
FILE *file;
char **PATH;
int PATHSZ = 2;

void execute_command(char *operation, char *command[], char file[])
{
    if (access(operation, X_OK) == 0)
    {
        // child
        if (file != NULL)
        {
            FILE *fi = fopen(file, "w");
            int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

            dup2(fd, 1); // make stdout go to file
            dup2(fd, 2); // make stderr go to file - you may choose to not do this
            close(fd);   // fd no longer needed - the dup'ed handles are sufficient
        }
        execv(operation, command);
        printf("leh");
    }
}
void process(char *command[], int count, char file[])
{
    int rc = fork();
    if (rc == 0)
    {
        execute_command(command[0], command, file);
        for (int i = 0; i < PATHSZ - 1; i++)
        {
            char *operation = malloc(sizeof(char) * SIZE);
            strcpy(operation, PATH[i]);
            strcat(operation, command[0]);
            execute_command(operation, command, file);
        }
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(0);
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
void builtin(int count, char *command[])
{
    if (strcmp(command[0], "exit") == 0)
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
    else if (strcmp(command[0], "cd") == 0)
    {
        if (count == 1 || count > 2 || chdir(command[1]) != 0)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
    else if (strcmp(command[0], "path") == 0)
    {
        PATHSZ = count;
        for (int i = 1; i < count; i++)
        {
            PATH = realloc(PATH, count * sizeof(char *));
            int length = strlen(command[i]);

            PATH[i - 1] = malloc(sizeof(char) * (length + 2));

            strcat(PATH[i - 1], command[i]);
            if (command[i][strlen(command[i]) - 1] != '/')
            {
                strcat(PATH[i - 1], "/");
            }
        }
    }
    else
    {
        process(command, count, NULL);
    }
}

int check_redir(char com[])
{
    int n = 0;
    for (int i = 0; i < strlen(com); i++)
    {
        if (com[i] == '>')
        {
            n++;
            if (n > 1 || (n >= 1 && (i == 1 || i == strlen(com) - 3)))
            {
                return -1;
            }
        }
    }
    return n;
}
int separate_commands(char *input[], char *cmd, char *delim)
{
    char *c;
    int count = 0;

    while ((c = strsep(&cmd, delim)) != NULL)
    {
        if (strcmp(c, "") == 0)
        {
            continue;
        }
        input[count] = c;
        count++;
    }
    input[count] = NULL;
    return count;
}
void make_spaces(char line[], char *newLine)
{
    int c = 0;
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] == '>' || line[i] == '|')
        {
            newLine[c++] = ' ';
            newLine[c++] = line[i];
            newLine[c++] = ' ';
            continue;
        }
        newLine[c++] = line[i];
    }
    newLine[c++] = '\0';
}

int separate_on_redir(char **command, char **sepCommand, char *fileName, int ndcount)
{
    for (int l = 0; l < ndcount; l++)
    {
        if (strcmp(command[l], ">") == 0)
        {
            if (l < ndcount - 2)
            {
                return 0;
            }
            else
            {
                strcpy(fileName, command[l + 1]);
                break;
            }
        }
        sepCommand[l] = malloc(sizeof(char) * strlen(command[l]));
        strcpy(sepCommand[l], command[l]);
    }
    return 1;
}

int commander(int redir, char **commands, char **command, int comnum)
{
    char **sepCommand;
    char fileName[SIZE];
    redir = check_redir(commands[comnum]);
    int ndcount = separate_commands(command, commands[comnum], " \n\t\r\f\v");
    if (ndcount == 0)
    {
        return 0;
    }
    if (redir == -1)
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
        return -1;
    }
    if (redir == 1)
    {
        sepCommand = malloc((ndcount) * sizeof(char *));
        if (!separate_on_redir(command, sepCommand, fileName, ndcount))
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            return -1;
        }
        process(sepCommand, ndcount - 1, fileName);
    }
    else
    {
        builtin(ndcount, command);
    }
}

void read_commands(char *line, int count)
{
    char *command[SIZE];
    char *commands[SIZE];
    int redir;
    char *newLine = (char *)malloc((strlen(line) + SIZE) * sizeof(char));
    make_spaces(line, newLine);
    count = separate_commands(commands, newLine, "&");
    if (count == 0)
    {
        return;
    }
    if (count == 1)
    {
        commander(redir, commands, command, 0);
        return;
    }
    pid_t child_pid[count];
    int stat;
    for (int i = 0; i < count; i++)
    {
        if ((child_pid[i] = fork()) == 0)
        {
            if (commander(redir, commands, command, i) == -1)
            {
                exit(0);
            }
            exit(0);
        }
    }
    for (int i = 0; i < count; i++)
    {
        pid_t wpid = waitpid(child_pid[i], &stat, 0);
    }
}
int rread(int mode)
{
    int count;
    char *line;
    if (mode == 1)
    {
        if (getline(&line, &inpsize, stdin) != -1)
        {
            read_commands(line, count);
        }else if(feof(stdin)){
            return -1;
        }
    }
    else
    {
        while (getline(&line, &inpsize, file) != -1)
        {
            read_commands(line, count);
        }
        if(feof(file)){
            return -1;
        }
    }
}
int main(int argc, char *argv[])
{
    PATH = malloc(sizeof(char *));
    PATH[0] = malloc(sizeof(char) * SIZE);
    strcpy(PATH[0], "/bin/");
    if (argc == 2)
    {
        file = fopen(argv[1], "r");
        if (file == NULL)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        if(rread(0)==-1){
            return 0;
        };
        return 0;
    }
    else if (argc > 2)
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    while (1)
    {

        printf("7ambolash> ");
        if(rread(1)==-1){
            break;
        };
    }

    return 0;
}
