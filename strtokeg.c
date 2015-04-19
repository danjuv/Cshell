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
    pid_t pid;
    strcat(prompt, " ==> ");

/* keep reading input until "quit" command or eof of redirected input */

    while (1) {

/* get command line from input */

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
		else if (!strcmp (args[0], "env"))
		{
			extern char ** environ;
			char ** env = environ;
			while(*env) {
				printf("%s\n", *env++);
			}

		}


/* else pass command onto OS (or in this instance, print them out) */

		else {
		    arg = args;
		    int i = 0;
		    int truncate = 0;
		    char * input = 0;
		    char * output = 0;
		    for(i = 0; args[i] != NULL; i++)
		    {
		    	if(!strcmp(args[i], "<"))
		    	{
		    		strcpy(args[i], "\0");
		    		input = malloc(strlen(args[i+1]) * sizeof(args[i]));
		    		strcpy(input, args[++i]);
		    	}
		    	else if(!strcmp(args[i], ">"))
		    	{
		    		output = malloc(strlen(args[i+1]) * sizeof(args[i]));
		    		strcpy(output, args[++i]);
		    	}
		    	else if(!strcmp(args[i], ">>"))
		    	{
		    		strcpy(output, args[++i]);
		    		truncate = 1;
		    	}
		    }
		    while (*arg) {
		    	if (!strcmp(*arg, "dir"))
		    	{
		    		char * directory = malloc(7 + sizeof(args[1]));
		    		switch (pid = fork())
		    		{
		    			case -1:
		    				//syserr("fork");
		    			case 0:
				    		if(!args[1])
				    		{
				    			//system("ls -al");
				    			char * eargs[] = {"ls", "-al", ".", NULL };
				    			execvp("ls", eargs);
				    			break;
				    		}
				    		*arg++;
				    		while(*arg)
				    		{
				    			//strcpy(directory, "ls -al ");
				    			strcpy(directory, *arg++);
				    			//*arg++;
				    			//getcwd(directory, sizeof(directory));
				    			printf("directory: %s\n", directory);
				    			char * eargs[] = {"ls", "-al", directory, NULL };
				    			execvp("ls", eargs);
				    			//syserr("exec");
				    		}
				    		
				    		//free(directory);
				    		printf("directory");
				    	default:
				    		waitpid(pid, NULL, WUNTRACED);
		    		}
		    		//fputs(directory, stdout);
		    		pid = getpid();
		    		break;

		    	}
		    	else if (!strcmp(*arg, "cd"))
		    	{
		    		char * cd = malloc(7 + sizeof(args[1]));
		    		static char pwd[1024];
		    		static char final_pwd[1024];
		    		strcpy(cd, *arg++);
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
		    				if(pwd != NULL)
		    				{
		    					strcpy(final_pwd,"PWD=");
		    					strcat(final_pwd, pwd);
		    					putenv(final_pwd);
		    					prompt = getenv("PWD");
		    					strcat(prompt, " ==> ");
		    				}
		    			}
		    		}

		    		//free(cd);
		    	}
		    	else if(!strcmp(*arg, "echo"))
		    	{
		    		printf("%s\n", (char *)args[1]);
		    		if(input != NULL)
		    		{
		    			printf("input: %s\n", input);
		    		}
		    		if(output != NULL)
		    		{
		    			printf("output: %s\n", output);
		    		}
		    		
		    		break;
		    	}
		    	else
		    	{
		    		char * cmd = malloc(7 + sizeof(args));
		    		strcpy(cmd, *arg++);
		    		while(*arg)
		    		{
		    			strcat(cmd, " ");
		    			strcat(cmd, *arg++);

		    		}
		    		printf("%s\n", "Command not found. Executing default unix command.");
		    		system(cmd);
		    		free(cmd);
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
