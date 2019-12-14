// C Program to design a shell in Linux 
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 	
#include<readline/readline.h> 
#include<readline/history.h> 

#define MAXCOM 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 

// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

// Greeting shell during startup 
void init_shell() 
{ 
	clear(); 
	printf("\n\n\n\n******************"
		"************************"); 
	printf("\n\n\n\t****MY SHELL****"); 
	printf("\n\n\t-USE AT YOUR OWN RISK-"); 
	printf("\n\n\n\n*******************"
		"***********************"); 
	char* username = getenv("USER"); 
	printf("\n\n\nUSER is: @%s", username); 
	printf("\n"); 
	sleep(1); 
	clear(); 
} 

// Function to take input 
int takeInput(char* str) 
{ 
	char* buf; 

	buf = readline("\n>>> "); 
	if (strlen(buf) != 0) { 
		add_history(buf); 
		strcpy(str, buf); 
		return 0; 
	} else { 
		return 1; 
	} 
} 

// Function to print Current Directory. 
void printDir() 
{ 
	char cwd[1024]; 
	getcwd(cwd, sizeof(cwd)); 
	printf("\nDir: %s", cwd); 
} 

// Function where the system command is executed 
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

// Function where the piped system commands is executed 
void execArgsPiped(char** parsed, char** parsedpipe) 
{ 
	printf("pipe\n");
	// 0 is read end, 1 is write end
	int pipefd[2];
	pid_t p1, p2;

	if (pipe(pipefd) < 0) {
		printf("\nPipe could not be initialized");
		return;
	}
	p1 = fork();
	if (p1 < 0) {
		printf("\nCould not fork");
		return;
	}

        
	int flag1;
    if(fork()==0)
    {
		close(1);  
		dup(pipefd[1]);
		close(pipefd[0]);
		execvp(parsed[0], parsed) ;
    }
    else
    {
       if(fork()==0)
       {
			close(0);
			dup(pipefd[0]);

			close(pipefd[1]);
				 
			execvp(parsedpipe[0], parsedpipe);
        }
        else
          wait(&flag1);
     
    }
    close(pipefd[0]);
    close(pipefd[1]);
   
   
  wait(&flag1); 
} 

void execArgsDirected(char** parsed, char** parseddirect,int k)//------------------------------
{
	if(k==3)//------------redirection
	{
		char file1[100];
		char file2[100];
		printf("Heyy i am sahi\n");
		
		strcpy(file1,parsed[0]);
		printf("file1 :%s\n",file1);
		strcpy(file2,parseddirect[0]);
		printf("file2 :%s\n",file2);
		FILE *fp = fopen(file2, "w");
		int f2 = fileno(fp);
		
		if(fork()==0)
		{
		    dup2(f2,1);
		    execlp(file1,file1,NULL);
		}
		else
		{
		    printf("this is parent\n"); 
		}
		fclose(fp);
		printf("output is given to the file you gave instead of monitor\n");
	}
	else if(k==4)//---------------indirection
	{
		char file1[100];
		char file2[100];
		
		
		strcpy(file1,parsed[0]);
		printf("file1 :%s\n",file1);
		strcpy(file2,parseddirect[0]);
		printf("file2 :%s\n",file2);
		FILE *fp = fopen(file2, "r");
		int f2 = fileno(fp);
		
		if(fork()==0)
		{
		    printf("this is child\n");        
		    dup2(f2,0);
		    execlp(file1,file1,NULL);
		}
		else
		{
		    printf("this is parent\n"); 
		}
		fclose(fp);
	}
}


// Help command builtin 
void openHelp() 
{ 
	puts("\n***WELCOME TO MY SHELL HELP***"
		"\nList of Commands supported:"
		"\n>cd"
		"\n>ls"
		"\n>exit"
		"\n>all other general commands available in UNIX shell"
		"\n>pipe handling"
		"\n>improper space handling"); 
	
	return; 
} 

// Function to execute builtin commands 
int ownCmdHandler(char** parsed) 
{ 
	int NoOfOwnCmds = 4, i, switchOwnArg = 0; 
	char* ListOfOwnCmds[NoOfOwnCmds]; 
	char* username; 

	ListOfOwnCmds[0] = "exit"; 
	ListOfOwnCmds[1] = "cd"; 
	ListOfOwnCmds[2] = "help"; 
	ListOfOwnCmds[3] = "hello"; 

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
		chdir(parsed[1]); 
		return 1; 
	case 3: 
		openHelp(); 
		return 1; 
	case 4: 
		username = getenv("USER"); 
		printf("\nHello %s.\nMind that this is "
			"not a place to play around."
			"\nUse help to know more..\n", 
			username); 
		return 1; 
	default: 
		break; 
	} 

	return 0; 
} 

// function for finding pipe 
int parsePipe(char* str, char** strpiped) 
{ 
	int i; 
	for (i = 0; i < 2; i++) { 
		strpiped[i] = strsep(&str, "|"); 
		if (strpiped[i] == NULL) 
			break; 
	} 

	if (strpiped[1] == NULL) 
		return 0; // returns zero if no pipe is found. 
	else  
		return 1; 
} 

// function for parsing command words 
void parseSpace(char* str, char** parsed) 
{ 
	int i; 

	for (i = 0; i < MAXLIST; i++) { 
		parsed[i] = strsep(&str, " "); 

		if (parsed[i] == NULL) 
			break; 
		if (strlen(parsed[i]) == 0) 
			i--; 
	} 
} 

int parseDirect(char *str,char** strdirected,int *x)//------------------
{ 
	char * S = str;
	char ** strdirect = strdirected;
	for (int i = 0; i < 2; i++) 
	{ 
		strdirect[i] = strsep(&S, ">"); //------------redirection  2 is returned
		if (strdirected[i] == NULL) 
			break; 
	} 
	if (strdirect[1] != NULL)
	{ 
		*x = 2;
		strdirected = strdirect;
		return 1; 
	}
	else 
	{
		for (int i = 0; i < 2; i++) 
		{ 
			strdirected[i] = strsep(&str, "<"); //-------------------indirection 3 is returned
			if (strdirected[i] == NULL) 
				break; 
		} 
		if (strdirected[1] != NULL)
		{ 
			*x = 3;
			printf("Need some cheer\n");
			return 1; 
		}
		else
		{
			return 0;
		}
	}
}

int processString(char* str, char** parsed, char** parsedpipe,char** parseddirect,int *x) 
{ 

	char* strpiped[2]; 
	char* strdirected[2];
	
	int piped = 0; 
	int directed = 0;//--------------------
	piped = parsePipe(str, strpiped); 
	directed = parseDirect(str,strdirected,x);//-----------------------------
	
	if (piped)
	{ 
		parseSpace(strpiped[0], parsed); 
		parseSpace(strpiped[1], parsedpipe); 

	} 
	else if (directed)//--------------------
	{
		parseSpace(strdirected[0], parsed);
		parseSpace(strdirected[1], parseddirect);	
	}
	else 
	{ 
		parseSpace(str, parsed); 
	} 

	if (ownCmdHandler(parsed)) 
		return 0; 
	else if(piped)
		return 2;
	else if(directed)
		return 1 + (*x);//----------can be 3 or 4
	else
		return 1;
} 

int main() 
{ 
	//printf ("\e[48;2;0;0;0m\n");	//BG black
	//printf ("\e[38;2;255;0;0m\n");	//text red
	printf("\e[0;36m\n");
	printf ("\e[48;2;0;0;0m\n");
	char inputString[MAXCOM], *parsedArgs[MAXLIST]; 
	char* parsedArgsPiped[MAXLIST]; 
	char* parsedArgsDirected[MAXLIST];//-------------------------------
	int execFlag = 0; 
	int k = 1;//------------------can be 2 or 3
	init_shell(); 

	while (1) { 
		// print shell line 
		printDir(); 
		// take input 
		if (takeInput(inputString)) 
			continue; 
		// process 
		execFlag = processString(inputString, 
		parsedArgs, parsedArgsPiped,parsedArgsDirected,&k); 
		// execflag returns zero if there is no command 
		// or it is a builtin command, 
		// 1 if it is a simple command 
		// 2 if it is including a pipe. 

		// execute 
		if (execFlag == 1) 
		{	
			printf("Hey this is sahithi\n");
			execArgs(parsedArgs); 
		}
		else if (execFlag == 2) 
		{
			execArgsPiped(parsedArgs, parsedArgsPiped); 
		}	
		else if (execFlag == 3 || execFlag == 4)
		{
			execArgsDirected(parsedArgs,parsedArgsDirected,execFlag);//--------------------
		}
	} 
	return 0; 
} 

