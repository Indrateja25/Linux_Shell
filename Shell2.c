//#include "shell.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

char buffer_g[500][2000];
int buffer_i[500];
int buffer_p=0;
int buffer_s=500;
int history_s=50;
char h[50][2000];
int history_i[50]; 
int history_p=0;

#define clear() printf("")
void start_shell();
void printDir();
int getInput(char* str);
void addHistory(char *buffer);
int processString(char* str, char** parsed, char** parsedpipe);
int parseSpace(char* str, char** parsed);
int ownCmdHandler(char** parsed);
void execArgs(char** parsed);
int which(char input[]);
void kil(char input[]);
void job();
void process(char Input[]);
void cd(char input[]);
void hst();
void h_number();
char* cwd();
//void clear()


char buf[10][1000];
int childern[1000];
int bufLocation=0;
int chID=0;

int main()
{
    char Input[1000], *parsedArgs[100];
    char* parsedArgsBG[100];
    int execFlag = 0,i=0,n=0;
    start_shell();
    l:
    while (1) {
        printDir();
        // if (!getInput(Input))
        //     continue;

        // n=process(Input);
        // if(n == -1)
        //     break;
        char input[2000];
        printf(">>");
        if(scanf("%[^\n]%*c",input)==0); 
            fflush(stdin);
        // if(strlen(input)<=0)
        //     continue;
        // printf("%s:%d",input,strlen(input));      
        if(history_p>history_s) history_p=0;
        strcpy(h[history_p], input);
        history_p++;
        if(history_p>=history_s) history_p=0;
        h_number();
        process(input);
    }
    return 0;
}


void start_shell()
{
	clear();
	printf("\n\n\n\n\t\tWELCOME!\n\n\n\n");
	sleep(1);
	clear();
}

void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\033[0;32m");
    printf("\n%s$%s",getenv("USER"), cwd);
    printf("\033[0m");
}

int getInput(char* str)
{
    char* buffer;
 
    buffer = readline("\n>>>.");
    //scanf("%[^\n]%*c",buffer);
    //printf("%d",strlen(buffer));
    if (strlen(buffer) != 0) {
        strcpy(str, buffer);
        addHistory(str);
        return 1;
    } else {
        return 0;
    }
}

void addHistory(char *buffer)
{
	add_history(buffer);
	strcpy(buf[bufLocation%10],buffer);
	bufLocation++;
}

int processString(char* str, char** parsed, char** parsedpipe)
{
	int num;
 	if(str[0]=='!')
 	{
 		bufLocation=bufLocation-1;
 		num = atoi(&str[1]);
 		if(num>bufLocation-1 || num<0)
 			strcpy(str,buf[(bufLocation-1)%10]);
 		else	
 			strcpy(str,buf[num%10]);
 		// printf("%d\t%s\t%s",num,str,buf[num%10]);
 	}
    num=parseSpace(str, parsed);

 
    if (ownCmdHandler(parsed))
        return 0;
    else
        return 1+num;
}

int parseSpace(char* str, char** parsed)
{
    int i;
 
    for (i = 0; i < 100; i++) {
        parsed[i] = strsep(&str, " ");
 
        if (parsed[i] == NULL)
            break;
        if(strcmp(parsed[i],">")==0 || strcmp(parsed[i],"<")==0)
        {
        	printf("\nfound at %d\n",i);
        	return 1;
        }
        if (strlen(parsed[i]) == 0)
            i--;
    }
    return 0;
}

int ownCmdHandler(char** parsed)
{
    int NoOfOwnCmds = 7, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];
    char* username;
    char *c;
    char *buffer;
    int b,err;
 
    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "jobs";
    ListOfOwnCmds[5] = "history";
    ListOfOwnCmds[6] = "kill";
 
    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }
 
    switch (switchOwnArg) {
    case 1:
        printf("\nGoodbye\n");
        exit(0);
    case 2:
        err=chdir(parsed[1]);
        if(err<0)
        	printf("\nUnable to Locate the Folder\n");
        return 1;
    case 3:
        printf( "\nList of Commands supported:"
		        "\n>cd"
		        "\n>ls"
		        "\n>jobs"
		        "\n>history"
		        "\n>kill"
		        "\n>exit"
		        "\n>all other general commands available in UNIX shell");
        return 1;
    case 4:
        username = getenv("USER");
        printf("\nHello %s.\nMind that this is "
            "not a place to play around."
            "\nUse help to know more..\n",
            username);
        return 1;
    case 6:
    	bufLocation=bufLocation-1;
    	b = bufLocation-1;
    	for(;b>=bufLocation-10 && b>=0;b--)
    		printf("\n>%d:%s",b,buf[b%10]);
    	printf("\n");
    	return 1;
    default:
        break;
    }
 
    return 0;
}

void execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork(); 
 
    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL); 
        return;
    }
}


int which(char input[])
{
    int pipe_fd[2];
    pipe(pipe_fd);

    if (fork() == 0)
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], 1);
        close(pipe_fd[1]);

        char* arg[] = {"which",input, NULL};
        execv("/usr/bin/which", arg);
        
    }
    else
    {
        wait(NULL);
        char bufer[100];
        char s_t[100];
        int i=0;
        close(pipe_fd[1]);

        if(read(pipe_fd[0], bufer, sizeof(bufer)) != 0)
        {
            while(bufer[i]!='\n' && bufer[i]!='\0')
            {
                s_t[i]=bufer[i];
                i++;
            }
            fflush(stdout);
            
            char* arg[] = {input, NULL};
            execv(s_t, arg);
        }
        else
        {
            return 0;
        }
        close(pipe_fd[0]);
    }
    return 1;
}

void kil(char input[])
{
    int ilen=strlen(input);
    char s_t[ilen-6];
    for(int i=6;i<ilen;i++)
    {
        s_t[i-6]=input[i];
    }
    s_t[ilen-6]='\0';
    int pid = atoi(s_t);

    printf("\npid:%d\nbuffer:",pid);
    
    for(int i=0;i<buffer_s;i++)
    {
        printf("%d\n",buffer_i[i] );
        if(buffer_i[i]==pid)
        {
            buffer_i[i]=0;
            strcpy(buffer_g[i], "");
        }   
    }
    kill(pid, SIGKILL);
}

void job()
{
    int x=buffer_p+1;
    while(x!=buffer_p)
    {
        if(buffer_i[x]!=0)
        {
            printf("%d: %s\n", buffer_i[x], buffer_g[x]);
        }
        x++;
        if(x>=buffer_s) x=0;
    }
}

void process(char input[])
{   
    int ilen = strlen(input);
    int pid = fork();
    if(pid==0)
    {
        if(input[ilen-1]=='&' && input[ilen-2]==' ')
        {
            pid=fork(); 
            if(pid==0)
            {
                char s_t[ilen-2];
                for(int i=0;i<ilen-2;i++)
                {
                    s_t[i]=input[i];
                }
                s_t[ilen-2]='\0';
                
                if(buffer_p>buffer_s)
                {
                    printf("Memory Full\n");
                }
                buffer_i[buffer_p]=getpid();
                strcpy(buffer_g[buffer_p], s_t);
                buffer_p++;
                
                process(s_t);
            }
            else
            {
                printf("\nin parent:\n");
                int stat=0;
                int n=(int)waitpid(-1,&stat,WCONTINUED);
                printf("\n%d\n",n);
                //fflush(stdout);
            }
        } 
        else if(strcmp(input,"help")==0)
        {
             printf( "\nList of Commands supported:"
                "\n>cd"
                "\n>ls"
                "\n>jobs"
                "\n>history"
                "\n>kill"
                "\n>exit"
                "\n>all other general commands available in UNIX shell");
        }      
        else if(strcmp(input, "history")==0)
        {
            hst();
        }
        else if(strcmp(input, "jobs")==0)
        {
            job();
        }
        else if(strcmp(input, "exit")==0)
        {   
            int flag=0;
            for(int i=0;i<buffer_s;i++)
            {
                if(buffer_i[i]!=0)
                {
                    flag=1;
                    printf("%d\n",buffer_i[i]);
                }
            }
            if(flag==1) printf("Kill processes to proceed\n");
            else exit(0);
        }
        else if(input[0]=='k' && input[1]=='i' && input[2]=='l' && input[3]=='l' && input[4]==' ' && input[5]=='%')
        {   
            kil(input);
        }
        else if(input[0]=='c' && input[1]=='d' && input[2]==' ')
        {   
            cd(input);
        }
        else if(input[0]=='!')
        {
            if(input[1]=='-' && input[2]=='1')
            {
                char nm[ilen-1];
                for(int i=2;i<ilen;i++)
                {
                    nm[i-2]=input[i];
                }
                nm[ilen-1]='\0';
                int num = atoi(nm);

                int max=0;
                int ind=0;
                for(int i=0;i<history_s;i++)
                {
                    if(history_i[i]>max) max=history_i[i];
                }
                int max2=0;
                for(int i=0;i<history_s;i++)
                {
                    if(history_i[i]>max2 && history_i[i]<max) 
                    {
                        max2=history_i[i];
                        ind=i;
                    }
                }
                if(max!=0) 
                {
                    if(strcmp(h[ind],"!-1")==0) printf("Infinite Callback \n");
                    else process(h[ind]);
                }
                else printf("Cmd history not sufficient\n");
            }
            else
            {
                char nm[ilen];
                for(int i=1;i<ilen;i++)
                {
                    nm[i-1]=input[i];
                }
                nm[ilen]='\0';
                int num = atoi(nm);
                
                int flag=0;
                for(int i=0;i<history_s;i++)
                {
                    if(history_i[i]==num)
                    {
                        process(h[i]);
                        flag=1;
                        break;
                    }
                }
                if(flag==0) printf("Cmd history invalid\n");
            }
        }
        else
        {   
            int error = which(input);
            if(error==0)
            {
                system(input);
            }
        }
    }
    else
    {
        wait(NULL);
        if(strcmp(input, "exit")==0)
        {   
            exit(0);
        }
    }
}

// void clear()
// {
//     int pid=fork();
//     int stat=0;
//     if(pid==0)
//     {
//         system("clear"); 
//         exit(0);
//     }
//     else waitpid(-1,&stat,WCONTINUED);
// }

char* cwd()
{
    char *pointer;
    char *bufer;
    long size;

    size = pathconf(".", _PC_PATH_MAX);
    bufer = (char *)malloc((size_t)size);

    if (bufer!=NULL) 
    pointer = getcwd(bufer, (size_t)size);
    return pointer;
}

void h_number()
{
    int x=history_p+1;
    
    int i=1;
    while(x!=history_p)
    {
        if(strcmp(h[x], "")!=0)
        {
            history_i[x]=i;
            i++;
        }
        else history_i[x]=0;
        x++;
        if(x>=history_s) x=0;
    }
}
void hst()
{
    int x=history_p+1;
    while(x!=history_p)
    {
        if(strcmp(h[x], "")!=0)
        {
            printf("%d: %s\n", history_i[x], h[x]);
        }
        x++;
        if(x>=history_s) x=0;
    }
}

void cd(char input[])
{
    if(input[3]=='.' && input[4]=='.' && strlen(input)==5)
    {
        char* p=cwd();
        for(int i=strlen(p)-1;i>=0;i--)
        {
            if(p[i]=='/')
            {
                p[i]='\0';
                break;
            }
        }
        chdir(p);
    }
    else if(input[3]=='/')
    {   
        int ilen=strlen(input);
        char s_t[ilen-3];
        for(int i=3;i<ilen;i++)
        {
            s_t[i-3]=input[i];
        }
        s_t[ilen-3]='\0';
        chdir(s_t);
    }
    else
    {
        int ilen=strlen(input);
        char s_t[ilen-3];
        for(int i=3;i<ilen;i++)
        {
            s_t[i-3]=input[i];
        }
        s_t[ilen-3]='\0';
        chdir(strcat(strcat(cwd(),"/"),s_t));
    }
}
