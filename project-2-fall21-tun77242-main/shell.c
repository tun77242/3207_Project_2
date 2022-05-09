#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

char pwd[100];

typedef struct Command
{
    int argC;
    char **args;
    
    char* name;

    int inFromFile;
    char *inputFile;

    int redOutRep;
    int redOutApp;
    char *outputFile;

    int runInBackground;    // cmd executed in background
    int isPipeline;

}Command;

Command *initCommand();

void parseCommand(char *tokens[], int numTokens);
void displayCommand(Command *thisCommand);
void cd(char **args, int argC);
void dir(char *directory);
void clr();
void echo(char **args, int argc);
void environment(char *varName);
void myPause();
void help();
void path();
void myExit();

int main(int argc, char *argv[], char *envp[])
{
    char *path = getenv("PATH");
    strncat(path, "/myshell", 8);
    setenv("shell", path, 1);
    getcwd(pwd, 100);

    int batchMode = 0;
    FILE *fp;

    if (argc == 2) 
    { 
        batchMode = 1;
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));   
            exit(1);
        } 
    }
    else if(argc > 2)
    {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    size_t bufsize = 50;
    char *buffer = malloc(bufsize * sizeof(char));
    char *savePtr;

    while(1)
    {
        if(batchMode)
        {   
            if(!feof(fp))
            {
                fgets(buffer, 32, fp);
            }
        }
        else
        {
            char cwd[100];
            printf("%s-hamsa> ", getcwd(cwd,100));
            getline(&buffer, &bufsize, stdin);      
            
            if(!strcmp(buffer, "\n"))
            {
                continue;
            }
        }

        char **strings = (char **)malloc(100 * sizeof(char *));
        char *commandString;
        int stringCount = 0;

        commandString = strtok_r(buffer, " \t\n", &savePtr);
        
        while (commandString != NULL) 
        {
            strings[stringCount] = (char *)malloc(100 * sizeof(char));
            strcpy(strings[stringCount], commandString);
            commandString = strtok_r(NULL, " \t\n", &savePtr);
            stringCount++;
        }

        parseCommand(strings, stringCount);
        
        int i;
        for (i = 0; i < stringCount; i++)
        {
            free(strings[i]);
        }
        
        free(strings);

        if(batchMode && feof(fp))
        {
            break;
        }
    }
    
    if(batchMode)
    {
        fclose(fp);
    }
}

void parse(char **tokens, int numTokens)
{
    Command **commands = malloc(sizeof(Command*));
    int sizeCommandsArray = 1;

    Command *thisCommand;
    int numCommands = 0;
    int initNewCommand = 1; 

    char *thisToken;

    int i;
    for (i = 0; i < numTokens; i++)
    {
        thisToken = tokens[i];

        if(initNewCommand)
        { 

            thisCommand = initCommand();
            thisCommand->name = thisToken;

            if (numCommands >= sizeCommandsArray)
            {
                sizeCommandsArray *= 2;
                commands = (Command **)realloc(commands, sizeCommandsArray * sizeof(Command));
            }

            commands[numCommands] = thisCommand;
            numCommands++;

            initNewCommand = 0;
        }

        if (!strcmp(thisToken, ">"))
        {
		    thisCommand->redOutRep = 1;
            strcpy(thisCommand->outputFile, tokens[++i]);

        }
        else if(!strcmp(thisToken, ">>"))
        {
            thisCommand->redOutApp = 1;
            strcpy(thisCommand->outputFile, tokens[++i]);
      
        }
        else if(!strcmp(thisToken, "<"))
        {
            thisCommand->inFromFile = 1;
            strcpy(thisCommand->inputFile, tokens[++i]);
        }
        else if(!strcmp(thisToken, "|"))
        {
            thisCommand->args[thisCommand->argC] = NULL;  
            thisCommand->isPipeline = 1;
            initNewCommand = 1;
        }
        else if(!strcmp(thisToken, "&"))
        {
            thisCommand->args[thisCommand->argC] = NULL;
            thisCommand->runInBackground = 1;
            initNewCommand = 1;
        }
        else
        {
            thisCommand->args[thisCommand->argC] = (char *)malloc(100 * sizeof(char));
            strcpy(thisCommand->args[thisCommand->argC], thisToken);
            thisCommand->argC++;
        }
    }

    thisCommand->args[thisCommand->argC] = NULL;  

    int k;
    for(k = 0; k < numCommands; k++) 
    {
        thisCommand = commands[k];

        
        if (!strcmp(thisCommand->name, "exit"))
        {
            int j;
            for (j = 0; j < numCommands; j++)
            {
                free(commands[j]);
            }
            
            free(commands);
            myExit();
        }
        else if(!strcmp(thisCommand->name, "cd"))
        {
            cd(thisCommand->args, thisCommand->argC);
            continue;
        }
        else if(!strcmp(thisCommand->name, "clr"))
        {
            clr();
            continue;
        }
        else if(!strcmp(thisCommand->name, "dir"))
        {
            if (thisCommand->argC == 1)
            { 
                dir(".");
            } 
            else
            {    
                int a;
                for (a = 1; a < thisCommand->argC; a++)
                {
                    printf("\nDirectory <%s>\n", thisCommand->args[a]);
                    dir(thisCommand->args[a]);
                }
            }
            
            continue;
        } 
        else if(!strcmp(thisCommand->name, "environ"))
        {
            if (thisCommand->argC == 2)
            {
                environment(thisCommand->args[1]);
            }
            else
            {
                environment("noneSpecified");
            }
            
            continue;
        }
        else if(!strcmp(thisCommand->name, "echo"))
        {
            echo(thisCommand->args, thisCommand->argC);
            continue;
        }
        else if(!strcmp(thisCommand->name, "help"))
        {
            if(thisCommand->argC == 1)
            {
                help();
            }
            else
            {
                int i;
                for(i=1; i < thisCommand->argC; i++)
                {
                    printf("\nHelp <%s>\n", thisCommand->args[i]);
                    help(thisCommand->args[i]);
                }
            }
            continue;
        }
        else if(!strcmp(thisCommand->name, "pause"))
        {
            myPause();
            continue;
        }
        else if(!strcmp(thisCommand->name, "path"))
        {
            path();
            continue;
        }
        else
        {    
            if (thisCommand->isPipeline)
            { 
                pid_t pipe_pid;
                int pipe_status;

                if((pipe_pid = fork()) == -1)
                {   
                    perror("fork");
                    exit(1);
                }
                else if(pipe_pid == 0)
                {       
                    pid_t child_pid;
          
                    int fd[2];
                    pipe(fd);

                    if((child_pid = fork()) == -1)
                    {  
                        perror("fork");
                        exit(1);
                    }
                    else if(child_pid == 0)
                    {  
                        dup2(fd[1], 1); 
                        close(fd[0]);
                        close(fd[1]);
            
                        if(thisCommand->inFromFile)
                        { 
                            freopen(thisCommand->inputFile, "r", stdin);
                        }
          
                        if(execvp(thisCommand->name, thisCommand->args) < 0)
                        {
                            char error_message[30] = "An error has occurred\n";
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(1);
                        }
                    }
                    else
                    {  
                        thisCommand = commands[++i]; 
                        dup2(fd[0], 0); 
                        close(fd[0]);
                        close(fd[1]);

                        if(thisCommand->redOutRep)
                        {  
                            freopen(thisCommand->outputFile, "w+", stdout);
                        }
                        else if(thisCommand->redOutApp)
                        {
                            freopen(thisCommand->outputFile, "a", stdout);
                        }

                        if(execvp(thisCommand->name, thisCommand->args) < 0)
                        {     
                            char error_message[30] = "An error has occurred\n";
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(1);
                        }
                    }
          
                }
                else
                {  
                    waitpid(pipe_pid, &pipe_status, 0);
                }
            }
            else
            {     
                pid_t child_pid;
                int status; 

                if((child_pid = fork()) == -1)
                {
                    perror("fork");
                    exit(1);
                }
        
                if(child_pid == 0)
                {  
                    if(thisCommand->inFromFile)
                    { 
                        freopen(thisCommand->inputFile, "r", stdin);
                    }
                
                    if(thisCommand->redOutRep)
                    {
                        freopen(thisCommand->outputFile, "w+", stdout);
                    }
                
                    if(thisCommand->redOutApp)
                    {
                        freopen(thisCommand->outputFile, "a", stdout);
                    }
                
                    if(execvp(thisCommand->name, thisCommand->args) < 0)
                    {   
                        char error_message[30] = "An error has occurred\n";
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(1);
                    }
                }
                else
                {  
                    if(!thisCommand->runInBackground)
                    { 
                        waitpid(child_pid, &status, 0);
                    }
                }
            
            } 
        }  
    } 
    
    return;
}

Command *initCommand()
{
    Command *thisCommand = (Command *)malloc(sizeof(Command));

    thisCommand->name = malloc(100 * sizeof(char));
    thisCommand->argC = 0;
    thisCommand->args = (char **)malloc(100 * sizeof(char *));
    thisCommand->inFromFile = 0;
    thisCommand->inputFile = (char *)malloc(100 * sizeof(char));
    thisCommand->redOutRep = 0;
    thisCommand->redOutApp = 0;
    thisCommand->outputFile = (char *)malloc(100 * sizeof(char));
    thisCommand->isPipeline = 0;
    thisCommand->runInBackground = 0;
}

void displayCommand(Command *thisCommand)
{
    printf("\tName:|\"%s\"\n", thisCommand->name);
    printf("\tArgument Count:| %d\n", thisCommand->argC);

    int i;
    for(i = 0; i < thisCommand->argC; i++)
    {
        printf("\t\tArgument %d:| \"%s\"\n", i+1, thisCommand->args[i]);
    }
    
    printf("\tTakes Input:| %d\n", thisCommand->inFromFile);
    printf("\tInput File Name:| \"%s\"\n", thisCommand->inputFile);
    printf("\tRedOutRep:| %d\n", thisCommand->redOutRep);
    printf("\tRedOutApp:| %d\n", thisCommand->redOutApp);
    printf("\tOutput File Name:| \"%s\"\n", thisCommand->outputFile);
	printf("\tIs Pipeline:| %d\n", thisCommand->isPipeline);
    printf("\tRuns In Background:| %d\n", thisCommand->runInBackground);
}

void cd(char **args, int argc)
{
    if((argc-1) > 1)
    {
        char error_message[30] = "An error has occured\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else if(argc == 1)
    {
        getcwd(pwd, 100);
        printf("%s", pwd);
    }
    else
    {
        chdir(args[1]);
    }
}

void path()
{
    printf("%s\n", getenv("PATH"));
}

void clr()
{
    printf("\033[H\033[2J");
}

void dir(char *directory)
{
    struct dirent *fileName;

        DIR* dp;
        dp = opendir(directory);

    if(dp == NULL)
    {
        char error_message[30] = "Error!\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    while((fileName = readdir(dp)))
    {
        printf("%s\n", fileName->d_name);
    }
    
    closedir(dp);
}

void echo(char **args, int argc)
{
    if(argc == 0)
    {
        exit(1);
    }

    int i;
    for(i = 1; i < argc; i++)
    {
        printf("%s", args[i]);
    }

    printf("\n");
}

void environ(char *varName)
{
    if(argc > 1)
    {
        exit(1);
    }
    else
    {
        if(!strcmp(varName, "noneSpecified"))
        {
            printf("USER:%s\n", getenv("USER"));
            printf("PATH:%s\n", getenv("PATH"));
        printf("HOSTNAME:%s\n", getenv("HOSTNAME"));
        printf("HOME:%s\n", getenv("HOME"));
        }
        else
        {  
            printf("%s:%s\n", varName, getenv(varName));
        }
    }    
}

void help(command cmd)
{
    if(argc > 1)
    {
        exit(1);
    }

    char *catArgs[2] = {"cat", "readMe"};
    int retval = fork();
    int status;

    if(retval == 0)
    {
        execvp(catArgs[0], catArgs);
        char error_message[30] = "Error!\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else
    {
        int child_pid = retval;
        waitpid(child_pid, &status, 0);
    }
}

void myPause()
{
    printf("Press enter to continue...");

    getchar();
}

void myExit()
{
    exit(0);
}