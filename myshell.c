/*
    MyShell - basic C shell that emulates the unix bash
    Author - Danju Visvanthan
    SID - 312095252
    Written for COMP3520 Assignment 1 2015 Semester 1.
*/

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
	//pipes io (stdin or stdout) to file fp
	return;
}

void set_env(env, val)
{
	//wrapper around putenv to behave like setenv.
}

int main (int argc, char ** argv) {
    char buf[MAX_BUFFER];				// line buffer
    char * args[MAX_ARGS];				// pointers to arg strings
    char ** arg;					// working pointer thru args
    char * prompt = getenv("PWD");				// shell prompt
    FILE * fp_in = 0;
    FILE * fp_out = 0;
    FILE * batch = 0;
    int s_in = dup(fileno(stdin));
    int s_out = dup(STDOUT_FILENO);
    pid_t pid;
    strcat(prompt, " ==> ");
    if(argv[1])
    {
    	if(access(argv[1], F_OK) == -1)
    	{
    		fprintf(stderr, "ERROR: opening script file; %s; No such file or directory\n", argv[1]);
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
		fputs (prompt, stdout);				// write prompt
		fgets (line, MAX_BUFFER, stdin);
	}

	else
	{
		if(feof(batch))
		{
			fclose(batch);
			exit(1);
		}
		fgets(line, MAX_BUFFER, batch); //next line in file;
		//printf("%s\n", line);
	}
	if (line) {		// read a line

/* tokenize the input into args array */

	    arg = args;
	    *arg++ = strtok (line,SEPARATORS);		// tokenize input
	    while ((*arg++ = strtok (NULL,SEPARATORS)));
							// last entry will be NULL

	    if (args[0]) {				// if there's anything there

/* check for internal/external command */

		if (!strcmp (args[0], "clr")) {		// "clr" command
		    system ("clear");
		}

		else if (!strcmp (args[0], "quit")) {	// "quit" command
		    break;				// break out of "while" loop to quit
		}


/* else pass command onto OS (or in this instance, print them out) */

		else {

		    
		    int i = 0;
		    char * input = 0;
		    char * output = 0;
		    for(i = 0; args[i] != NULL; i++)
		    {
		    	if(!strcmp(args[i], "<"))
		    	{
		    		strcpy(args[i], "\0");
		    		input = args[++i];
		    		fp_in = fopen(input, "r");
		    	}
		    	else if(!strcmp(args[i], ">"))
		    	{
		    		output = args[++i];
		    		fp_out = fopen(output, "w");
		    		
		    	}
		    	else if(!strcmp(args[i], ">>"))
		    	{
		    		output = args[++i];
		    		fp_out = fopen(output, "a");
		    	}
		    	else if(!strcmp(args[i], "&"))
		    	{
		    		args[i] = '\0';
		    		bg = 1;
		    	}
		    }
		    arg = args;
		    while (*arg) {
		    	if (!strcmp (args[0], "environ"))
		    	{
		    		int std_out = dup(STDOUT_FILENO);
		    		if(fp_out != NULL)
		    		{
		    			dup2(fileno(fp_out), 1);
		    			close(fileno(fp_out));
		    		}
		    		extern char ** environ;
		    		char ** env = environ;
		    		while(*env) {
		    			printf("%s\n", *env++);
		    		}
    				if(fp_out != NULL)
    				{
	    				dup2(std_out, 1);
	    				close(std_out);
	    				
    				}
    				fp_out = 0;
    				break;

		    	}

		    	else if (!strcmp(*arg, "dir"))
		    	{
		    		
		    		char * directory = *arg++;
		    		
		    		switch (pid = fork())
		    		{
/*		    			    				if(fp_out != NULL)
		    			    				{
		    			    					dup2(fileno(fp_out), 1);
		    			    					close(fileno(fp_out));
		    			    				}

		    			    				printf("%s\n", (char *)args[1]);

		    			    				if(fp_out != NULL)
		    			    				{
		    				    				dup2(std_out, 1);
		    				    				close(std_out);
		    				    				fp_out = 0;
		    			    				}*/
		    			case -1:
		    				//syserr("fork");
		    			case 0:
		    				;
		    				int std_out = dup(STDOUT_FILENO);
			    			if(fp_out != NULL)
			    			{
			    				dup2(fileno(fp_out), 1);
			    				close(fileno(fp_out));
			    			}
				    		if(!args[1] || !strcmp(args[1], "<") || !strcmp(args[1], ">") || !strcmp(args[1], ">>"))
				    		{
				    			//system("ls -al");

				    			char * eargs[] = {"ls", "-al", ".", NULL };
				    			execvp("ls", eargs);

				    		}
				    		else
				    		{
					    		while(*arg)
					    		{
					    			//strcpy(args[1], "ls -al ");
					    			char * directory = *arg++;
					    			//*arg++;
					    			//getcwd(directory, sizeof(directory));
					    			char * eargs[] = {"ls", "-al", directory, NULL };
					    			execvp("ls", eargs);
					    			//syserr("exec");
					    		}
				    		}	

		    				if(fp_out != NULL)
		    				{
			    				dup2(std_out, 1);
			    				close(std_out);
			    				
		    				}
		    				
		    				exit(1);
				    	default:
				    		//printf("Test\n");
				    		to_wait(pid);
		    		}
		    		//fputs(directory, stdout);
		    		fp_out = NULL;
		    		pid = getpid();
		    		break;

		    	}
		    	else if (!strcmp(*arg, "cd"))
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
		    			if(chdir(*arg++) == 0)
		    			{
		    				getcwd(pwd, sizeof(pwd));
		    				if(pwd != NULL)
		    				{
		    					strcpy(final_pwd,"PWD=");
		    					strcat(final_pwd, pwd);
		    					putenv(final_pwd);
		    					prompt = getenv("PWD");
		    					strcat(prompt, " ==> ");
		    					break;
		    				}
		    			}
		    			
		    			else
		    			{
		    				printf("invalid directory.\n");
		    			}
		    		}
		    	}
		    	else if(!strcmp(*arg, "echo"))
		    	{
		    		int std_out = dup(STDOUT_FILENO);
		    		switch(pid = fork())
		    		{
		    			case 0:
		    				//printf("%i", fileno(stdout));
		    				if(fp_out != NULL)
		    				{
		    					dup2(fileno(fp_out), 1);
		    					close(fileno(fp_out));
		    				}
		    				int i = 1;
		    				while(1)
		    				{
		    					if(args[i] == NULL || !strcmp(args[i], "<") || !strcmp(args[i], ">") || !strcmp(args[i], ">>"))
		    					{
		    						printf("\n");
		    						break;
		    					}
		    					printf("%s ", (char *)args[i]);
		    					i++;

		    				}

		    				if(fp_out != NULL)
		    				{
			    				dup2(std_out, 1);
			    				close(std_out);
			    				fp_out = 0;
		    				}
		    				exit(1);
		    			default:
		    				to_wait(pid);


		    		}
		    		fp_out = 0;
		    		break;
		    	}
		    	else
		    	{
		    		switch(pid = fork())
		    		{
		    			case 0:
		    			;
		    				int std_in = dup(STDIN_FILENO);
		    				int std_out = dup(STDOUT_FILENO);
			    			if(fp_out != NULL)
			    			{
			    				dup2(fileno(fp_out), 1);
			    				close(fileno(fp_out));
			    			}
			    			if(fp_in != NULL)
			    			{
			    				dup2(fileno(fp_in), STDIN_FILENO);
			    				close(fileno(fp_in));
			    			}
			    			char * args_f[3];
			    			args_f[0] = args[0];
			    			args_f[1] = args[2];
			    			args_f[2] = '\0';
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
