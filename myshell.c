/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                                                             *
 *	  MyShell - basic C shell that emulates the unix bash      *
 *    Author - Danju Visvanthan                                *
 *    SID - 312095252                                          *
 *    Written for COMP3520 Assignment 1 2015 Semester 1.       *
 *															   *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define MAX_BUFFER 1024					// max line buffer
#define MAX_ARGS 64					// max # args
#define SEPARATORS " \t\n"				// token sparators

extern int errno;
int bg = 0;
static char * shell_path [1024];

void syserr(char*);

void to_wait(pid_t pid)
{
	//waits if program is operating in foreground.
	if(bg == 0)
	{
		waitpid(pid, NULL, WUNTRACED);
	}
	bg = 0;
	
}

void pipe_io(FILE * fp, int io)
{
	/**
		pipes file stream stdin = 1, stdout = 2
		supports redirecting back to stdin.
	**/
	if(fp != NULL)
	{
		dup2(fileno(fp), io);
		close(fileno(fp));
	}
}

void catch_signal(int num)
{
	printf("");
}


int main (int argc, char ** argv) {
	signal(SIGINT, catch_signal);
	signal(SIGTSTP, catch_signal);
	FILE * fp_in = 0;
	FILE * fp_out = 0;
	FILE * batch = 0;

    char * args[MAX_ARGS];				// pointers to arg strings
    char ** arg;					// working pointer thru args

    getcwd(shell_path, sizeof(shell_path));

    char * shell[256];
    strcpy(shell, "SHELL=");
    strcat(shell, getenv("PWD"));
    strcat(shell, "/myshell"); // set env variable to executed directory;
    putenv(shell);


    pid_t pid;
    char * prompt = " ==> " ;

    if(argv[1])
    {
    	//check if batch file exists
    	if(access(argv[1], F_OK) == -1)
    	{
    		fprintf(stderr, "ERROR: opening script file; %s; No such file or directory\n", argv[1]);
    		//proceed normally if it doesn't.
    	}

    	else
    	{
    		batch = fopen(argv[1], "r");
    	}

    }
/* keep reading input until "quit" command or eof of redirected input */
    
    while (1) 
    {

/* get command line from input */
    // prompt = getenv("PWD")
    char line[MAX_BUFFER];
    if(batch == NULL)
    {
    	char cwd[1024];
    	getcwd(cwd, sizeof(cwd));
    	fputs(cwd, stdout);
		fputs (prompt, stdout);				// write prompt
		fgets (line, MAX_BUFFER, stdin);
	}

	else
	{
		//supress command prompt if batch file
		if(feof(batch))
		{
			fclose(batch);
			exit(1);
		}
		fgets(line, MAX_BUFFER, batch); //next command in batch file
		//printf("%s\n", line);
	}

	if (line) {		// read a line

/* tokenize the input into args array */
	    arg = args;
	    *arg++ = strtok (line,SEPARATORS);		// tokenize input
	    while ((*arg++ = strtok (NULL,SEPARATORS)));
							// last entry will be NULL

	    if (args[0]) {				// if there's anything there

		if (!strcmp (args[0], "clr")) {		// "clr" command
		    system ("clear");
		}

		else if (!strcmp (args[0], "quit")) {	// "quit" command
		    break;				// break out of "while" loop to quit
		}

		else if (!strcmp(args[0], "pause"))
		{	
			// only need to supress input
			// doesn't matter how
			getpass("Press Enter to continue...");
			break;
		}


/* else pass command onto OS (or in this instance, print them out) */

		else {

		    int i = 0;
		    char * input = 0;
		    char * output = 0;

		    for(i = 0; args[i] != NULL; i++)
		    {
		    	//check for i/o redirection or bg execution
		    	if(!strcmp(args[i], "<"))
		    	{
		    		args[i] = NULL; // "delete" the element
		    		input = args[++i];
		    		fp_in = fopen(input, "r"); //only read
		    	}

		    	else if(!strcmp(args[i], ">"))
		    	{
		    		args[i] = NULL;
		    		output = args[++i];
		    		fp_out = fopen(output, "w"); //write and replace
		    		
		    	}

		    	else if(!strcmp(args[i], ">>"))
		    	{
		    		args[i] = NULL;
		    		output = args[++i];
		    		fp_out = fopen(output, "a"); //write and append
		    	}

		    	else if(!strcmp(args[i], "&"))
		    	{
		    		args[i] = '\0';
		    		bg = 1;
		    	}
		    }
		    arg  = args;
		    while (*arg) {

		    	if (!strcmp(*arg, "cd"))
		    	{

		    		char * cd = *arg++;
		    		static char pwd[1024];
		    		static char final_pwd[1024];
		    		getcwd(pwd, sizeof(pwd));
		    		if(!args[1]) {
		    			printf("%s\n", pwd);
		    		}
		    		while(*arg)
		    		{
		    			// strcat(cd, " ");
		    			// strcat(cd, *arg
		    			if(chdir(*arg++) == 0) //change directory
		    			{
		    				getcwd(pwd, sizeof(pwd));
		    				if(pwd != NULL)
		    				{
		    					setenv("PWD", pwd, 1); //update PWD env
		    					break;
		    				}
		    			}
		    			
		    			else
		    			{
		    				printf("ERROR: change directory failed; No such file or directory\n");
		    			}
		    		}
		    	}

		    	else if (!strcmp (*arg, "environ"))
		    	{
		    		int std_out = dup(STDOUT_FILENO);
		    		pipe_io(fp_out, 1);
		    		extern char ** environ;
		    		char ** env = environ;
		    		while(*env) {
		    			printf("%s\n", *env++);
		    		}
		    		pipe_io(fdopen(std_out, "r"), 1);
    				fp_out = 0;
    				break;

		    	}

		    	else if(!strcmp(*arg, "echo"))
		    	{
		    		int std_out = dup(STDOUT_FILENO);
    				pipe_io(fp_out, 1);
    				int i = 1;
    				while(1)
    				{
    					if(args[i] == NULL)
    					{
    						printf("\n"); //blank line if no arguments or \n if new element
    						break;
    					}
    					printf("%s ", (char *)args[i]);
    					i++;

    				}
    				pipe_io(fdopen(std_out, "r"), 1);
		    		fp_out = 0;
		    		break;
		    	}

		    	else if (!strcmp(*arg, "dir"))
		    	{
		    		
		    		static char parent[1024];

		    		char * eargs[1024];
		    		eargs[0] = "ls";
		    		eargs[1] = "-al"; //hardcode values that are const for every dir

		    		int i = 1;
		    		while(1)
		    		{
		    			// create execution array for each directory specified in dir.
		    			if(args[i]==NULL)
		    			{
		    				//no directory specified
		    				eargs[i+1] = NULL;
		    				break;
		    			}
		    			eargs[i+1] = args[i];
		    			i++;
		    		}

		    		switch (pid = fork())
		    		{
		    			case -1:
		    				fprintf(stderr, "bad fork");
		    			case 0:
		    				;
		    				setenv("PARENT", getenv("SHELL"), 1);
		    				int std_out = dup(STDOUT_FILENO);
		    				pipe_io(fp_out, 1);
				    		if(!args[1])
				    		{
				    			//system("ls -al");

				    			char * earg[] = {"ls", "-al", ".", NULL };
				    			execvp("ls", earg);

				    		}
				    		else
				    		{
					    			execvp("ls", eargs);
					    			continue;
					    		
				    		}	

				    		pipe_io(fdopen(std_out, "r"), 1);
		    				
		    				exit(1);
				    	default:
				    		//printf("Test\n");
				    		to_wait(pid);
		    		}
		    	
		    		//fputs(directory, stdout);
		    		fp_out = NULL;
		    		*arg++;
		    		break;

		    	}


		    	else if(!strcmp(*arg, "help"))
		    	{
		    		char * std_out = STDOUT_FILENO;
		    		switch(pid=fork())
		    		{
		    			case -1:
		    				fprintf(stderr, "bad fork");

		    			case 0:
		    				setenv("PARENT", getenv("SHELL"), 1);
		    				pipe_io(fp_out, 1);
				    		char tmp[1024];
				    		strcat(tmp, "/readme");
				    		execlp("more", "more", "/home/danju/CShell/readme", NULL); //load readme with more filter
				    		
				    		pipe_io(fdopen(std_out, "r"), 1);
		    				exit(1);
		    			default:
		    				to_wait(pid);
    				}
    				fp_out = 0;
    				break;

		    	}

		    	else
		    	{
		    		//external commands, feed to execvp
		    		switch(pid = fork())
		    		{
		    			case -1:
		    				fprintf(stderr, "bad fork");

		    			case 0:
		    			;
		    				setenv("PARENT", getenv("SHELL"), 1);
		    				int std_in = dup(STDIN_FILENO);
		    				int std_out = dup(STDOUT_FILENO);
			    			pipe_io(fp_out, 1);
			    			pipe_io(fp_in, STDIN_FILENO);
		    				execvp(args[0], args);
		    				exit(1);
		    			default:
		    				to_wait(pid);

		    		}
		    		fp_in = NULL;
		    		fp_out = NULL;
		    		break;
		    	}
		    }
		    //fputs ("\n", stdout);

		}
	    }
	}

	else {						// user presses control-D or EOF has been reached
	    puts ("");					// ensure that there is a newline after shell exits
	    break;					// break out of "while" loop to quit
	}
    }
    return 0;
}
