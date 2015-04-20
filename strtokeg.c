/*
    strtokeg - skeleton shell using strtok to parse command line

    usage:

	strtokeg

	reads in a line of keyboard input at a time, parsing it into
	tokens that are separated by white spaces (set by #define
	SEPARATORS).

	can use redirected input

	if the first token is a recognized internal command, then that
	command is executed. otherwise the tokens are printed on the
	display.

	internal commands:
	    clear - clears the screen

	    quit - exits from the program

 ********************************************************************
    version:	     1.1
    date:	     Originally written in December 2003,
		     updated in March 2015
    original author: Ian G Graham
		     School of Information Technology
		     Griffith University, Gold Coast
		     ian.graham@griffith.edu.au

    ** This code has been updated by USYD SIT teaching staff in 2015 **

    Copyright (C) Ian G Graham, 2003. All rights reserved.

    This code can be used for teaching purposes, but no warranty,
    explicit or implicit, is provided.
 *******************************************************************/

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

//void syserr(char* );

int main (int argc, char ** argv) {
    char buf[MAX_BUFFER];				// line buffer
    char * args[MAX_ARGS];				// pointers to arg strings
    char ** arg;					// working pointer thru args
    char * prompt = getenv("PWD");				// shell prompt
    FILE * fp_in = 0;
    FILE * fp_out = 0;
    int s_in = dup(fileno(stdin));
    int s_out = dup(STDOUT_FILENO);
    pid_t pid;
    strcat(prompt, " ==> ");

/* keep reading input until "quit" command or eof of redirected input */

    while (1) {

/* get command line from input */
    // prompt = getenv("PWD")
	fputs (prompt, stdout);				// write prompt
	if (fgets (buf, MAX_BUFFER, stdin)) {		// read a line

/* tokenize the input into args array */

	    arg = args;
	    *arg++ = strtok (buf,SEPARATORS);		// tokenize input
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
		    arg = args;
		    int i = 0;
		    char * input = 0;
		    char * output = 0;
		    for(i = 0; args[i] != NULL; i++)
		    {
		    	if(!strcmp(args[i], "<"))
		    	{
		    		strcpy(args[i], "\0");
		    		input = malloc(strlen(args[i+1]) * sizeof(args[i]));
		    		strcpy(input, args[++i]);
		    		//fp_in = fopen(input, "r", stdout);
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
		    }
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
				    		waitpid(pid, NULL, WUNTRACED);
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
		    				waitpid(pid, NULL, WUNTRACED);


		    		}
		    		fp_out = 0;
		    		break;
		    	}
		    	else
		    	{
		    		char * cmd = *arg++;
		    		// strcpy(cmd, *arg++);
		    		while(*arg)
		    		{
		    			strcat(cmd, " ");
		    			strcat(cmd, *arg++);

		    		}
		    		printf("%s\n", "Command not found. Executing default unix command.");
		    		system(cmd);
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
