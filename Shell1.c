#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <dirent.h> 
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

char command[1024],history[10][256],arguments[256][256],background[50][256];
int hist_curr = 0,hist_max = 10,bg_curr = 0,bg_max = 50,arg_pointer = 0,start = 0,bg_pid[50];

void printPrompt();
void readCommandLine();
void parseCommand();
void record_command_history();
int  isBuiltInCommand();
void executeBuiltInCommand();
void cd();
void ls();
void print_history();
void show_jobs();
void executeCommand();
int  isBackgroundJob();
void remove_and();
void Record_in_list_of_background_jobs(int pid);
void freeArguments();
void kill_proc();


int main(){
       printf("\n\nWelcome! starting shell......\n\nType 'help' to know commands\n\n");
       while(1){
        //printf("running in loop\n\n");
        int childPid;
        printPrompt();
        readCommandLine(command); //printf("%s\n", command);
        record_command_history();
        parseCommand(command);
        //execute command
        if ( isBuiltInCommand())
        {
             executeBuiltInCommand();
        }
        else{
             childPid = fork();
             
             if (childPid == 0)
             {  
                 //printf("pid = ",getpid());  
                  //printf("\n\nis not a built-in command\t%s\n\n", command);
                 if(isBackgroundJob()) 
                 {   //printf("\n\n is a bg job\n\n");
                     remove_and();
                     //printArguments();
                     //printArguments();
                     Record_in_list_of_background_jobs(childPid);
                 }
                  
                  executeCommand(); //calls execvp
             } 
             else
             {    
                //printf("pid = ",getpid());
                 if(isBackgroundJob()) 
                 {   //printf("\n\n is a bg job\n\n");
                     
                     //printArguments();
                     //Record_in_list_of_background_jobs(childPid);
                 }
               else{
                 int status; 
                 //printf("started waiting \n");
                 wait(&status);
                 //printf("stopped waiting \n");
                 }
             }
        }
        //printPrompt();
        //printArguments();
        freeArguments();
       }
     return 0; 		
}

void printPrompt(){
    char prompt[256];
    getcwd(prompt,256);
    printf("\n%s:~$ ",prompt);
    return;
}

void readCommandLine(){
     //scanf("%s", command);
     //l:printPrompt();
     scanf("%[^\n]%*c", command);
     //if(strcmp(command,"\0")==0)
       //    goto l;
     printf("\n");
     fflush(stdin);
     return;
}

void parseCommand(){
   char *cla = strtok(command," ");
   while(cla){
     strcpy(arguments[arg_pointer],cla);
     //printf("%s\n",arguments[arg_pointer]);
     cla = strtok(NULL," ");
     arg_pointer++;
   }
   return;
}

void record_command_history(){
    if(strcmp(command,"history")==0)
      return;
    strcpy(history[hist_curr], command);
    start = start%hist_max;
    hist_curr++;
    if(hist_curr == hist_max)
       start++;
    hist_curr = hist_curr%hist_max;
    
    return;
}

int isBuiltInCommand(){
    if(strcmp(command, "cd")==0 || strcmp(command, "ls")==0 || strcmp(command, "jobs")==0 || strcmp(command, "history")==0 || strcmp(command, "kill")==0 || strcmp(command, "exit")==0 || strcmp(command, "help")==0) 
      return 1;
    return 0;    
}

void executeBuiltInCommand(){
   if(strcmp(arguments[0], "cd")==0){
      cd(arguments[1]);
   }
   else if(strcmp(command, "ls")==0){
      ls();
   }
   else if(strcmp(command, "jobs")==0){
      show_jobs();
   }
   else if(strcmp(command, "history")==0){
      print_history();
   }
   else if(strcmp(arguments[0], "kill")==0){
      kill_proc(command);
   }
   else if(strcmp(command, "exit")==0){
      printf("\n\nGoodbye! Have a nice day\n\n");
      sleep(1);
      exit(0);
   }
   else if(strcmp(command, "help")==0){
      printf( "Commands supported by the shell:"
                "\n-->cd"
                "\n-->ls"
                "\n-->jobs"
                "\n-->history"
                "\n-->kill"
                "\n-->exec"
                "\n-->exit"
                "\n-->all other commands available in LINUX/UNIX shell\n\n");
   }
   return;
}

void cd(char *folder){
   if(strcmp(folder,"..")==0 && arg_pointer==2)
    {   
        char path[1024];
        getcwd(path,1024);
        for(int i=strlen(path)-1;i>=0;i--)
        {
            if(path[i]=='/')
            {
                path[i]='\0';
                break;
            }
        }
        chdir(path);
    }
    else if(strcmp(folder,"~")==0 && arg_pointer==2)
    {   
        char path[1024];
        getcwd(path,1024);
        for(int i=1;i<strlen(path);i++)
        {
            if(path[i]=='/')
            {
                path[i]='\0';
                break;
            }
        }
        chdir(path);
    }
    else
    {   char path[1024];
        getcwd(path,1024);
        strcat(path,"/");
        strcat(path,folder);
        chdir(path);	    
    }
    return;
}

void ls(){
    char path[1024];
    getcwd(path,1024);
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
    int iter=0;
    while ((dir = readdir(d)) != NULL) {
    iter++;
	if(iter==1) continue;
      printf("%s\n", dir->d_name);
    }
    
    
    closedir(d);
  }
  return(0);

}

void print_history(){
   //printf("start = %d hist_curr = %d\n",start,hist_curr);
   if(start == 0 && hist_curr == 0){
     printf("No History saved...\n");
     return;
     }
   int j = start-1;
   if(start < hist_curr)
   {
       for(j++;j<hist_curr;j++)
            printf("%d:%s\n",j,history[j]);//[i]);
   }
   else{
       for(j;j<hist_max;j++)
         printf("%d:%s\n",j,history[j]);
       for(j = 1;j<start;j++)
         printf("%d:%s\n",j,history[j]);
   }
   return;
}

void show_jobs(){
   if(bg_curr == 0)
     printf("No Background jobs running...\n");
   for(int i=0;i<bg_curr;i++)
     printf("%d)%s.\n",i,background[i]);
   return;
}

void executeCommand(){
    //printf("\n\n%s %c\n\n",command,command[0]);
    //printf("\n\n%s %s\n\n",arguments[0],arguments);
   // printArguments();
    char *poi[arg_pointer];
    for(int i=0;i<arg_pointer;i++){
       poi[i] = arguments[i];
       //printf("%d: %s\n",i,poi[i]);
    }
    
    poi[arg_pointer] = NULL;
    int c = execvp(arguments[0], poi);
    //printf("\n\n%s %c %d\n\n",command,command[0],c);
    if(c==-1){
       //printf("error %s! could not execute command.\n\nexiting.....\n\n",strerror(errno));
      char errmsg[64];
      sleep(1);
      snprintf( errmsg, sizeof(errmsg), "exec '%s' failed", arguments[0] );
      perror( errmsg );
    }
    return;
}

int isBackgroundJob(){
   if(strcmp(arguments[arg_pointer-1],"&")==0)
      return 1;
   return 0;
}

void remove_and(){
  for(int i=strlen(command)-1;i>=0;i--)
        {
            if(command[i] =='&')
            {
                command[i]='\0';
                command[i-1] = '\0';
                break;
            }
        }
        strcpy(arguments[--arg_pointer] ,"\0");
       return;
}

void Record_in_list_of_background_jobs(int pid){
    strcpy(background[bg_curr],command);
    bg_pid[bg_curr] = pid;
    bg_curr = (bg_curr+1)%bg_max;
    return;
}

void freeArguments(){
   for(int i=0;i<arg_pointer;i++){
      memcpy(arguments[i],"/0",256);
   }
   arg_pointer = 0;
   return;
}

void kill_proc()
{   
   int a = atoi(arguments[2]);
   printf("killing %d %d..\n",a,bg_pid[a]);	
    kill(bg_pid[a], SIGKILL);
   strcpy(background[a],"\0");
   return;
}
void printArguments(){
  printf("printing arguments...\n\n");
  for(int i=0;i<arg_pointer;i++)
     printf("%d: %s, ",i,arguments[i]);
     printf("\n\n");
}
