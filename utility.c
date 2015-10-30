/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <Mirna Mohamed, Mohannad Naser Abdo, Luisa Rojas-Garcia>
 * All rights reserved.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include "utility.h"

// Define your utility functions here, these will most likely be functions that you call
// in your myshell.c source file

 /*
-----------------------------------------------------------------
FUNCTION parse:
This function takes an input line and parse it into tokens.
It first replaces all white spaces with zeros until it hits a
non-white space character which indicates the beginning of an
argument. It saves the address to argv[], and then skips all
non-white spaces which constitute the argument.
-----------------------------------------------------------------
*/

extern void parse(char *line, char **argv) {
	i=0;
	while (*line != '\0') {
		/* if not the end of line ....... */
		while (*line == ' ' || *line == '\t' || *line == '\n')
			*line++ = '\0';
			/* replace white spaces with 0*/
		*argv++ = line;
		// save the argument position
		while (*line != '\0' && *line != ' ' &&
			*line != '\t' && *line != '\n')
			line++;
			// skip the argument until ...
		i++;
	}
	*argv = '\0';
}

/* mark the end of argument list
-----------------------------------------------------------------
FUNCTION execute:
This function receives a parsed command line, forks a child
process to execute the command using system call execvp().
-----------------------------------------------------------------
*/

extern void execute(char **argv) {
	int j = 0;
	/* I/O REDIRECTION */
	int ipredirect;
	// input redirection flag
	int opredirect;
	// output redirection flag
	char opredirectmode[20]; // output redirection mode
	char infile[20];
	// buffer for input file
	char outfile[20];
	// buffer for output file
	// input redirection
	while (argv[j]) {
		if (strcmp(argv[j], "<") == 0) {
			ipredirect=1;
			// set flag
			argv[j]=NULL;
			// delimit the < character
			if(argv[++j])
			strcpy(infile, argv[j]); // store the input file to buffer
		}	
		// output redirection
		else if (strcmp(argv[j], ">") == 0 || strcmp(argv[j], ">>") == 0) {
			if (strcmp(argv[j], ">") == 0)
				strcpy(opredirectmode, "w"); // set mode to write
			else if (strcmp(argv[j], ">>") == 0)
				strcpy(opredirectmode, "a"); // set mode to append
			opredirect=1;
			// set flag
			argv[j]=NULL;
			// delimit the > or >> character
			if(argv[++j])
				strcpy(outfile, argv[j]);
				// store the output file to buffer
		}

		// "&" at the end of command line indicates background execution
		else if (strcmp(argv[j], "&") == 0) {
			foreground=0; // foreground execution is set to false
		argv[j]=NULL; // delimit the & character
		}
		j++;
	}	


	/* INTERNAL and EXTERNAL commands */
	// "environ" command
	if (strcmp(argv[0], "environ") == 0) {
		for (j = 0; environ[j] != NULL; j++) // get the environment strings
			printf("%s\n",environ[j]); // output to screen
		}// "cd" and "cd <directory"
	else if (strcmp(argv[0], "cd") == 0) {
		if (!argv[1]) {
			if (getcwd(cwdbuff, sizeof(cwdbuff))) // get current working directory
				printf("%s\n", cwdbuff);
				// output to screen
			else
				printf("%s\n","*** ERROR: retrieving cwd");
		}
		// "cd <directory>"
		else {
			if (!chdir(argv[1])) {
				strcpy(tempbuff, "PWD="); //PWD=<directory>
				getcwd(cwdbuff, sizeof(cwdbuff));
				strcat(tempbuff, cwdbuff);
				if (putenv(strdup(tempbuff))) // change the PWD environment string
					printf("%s\n","*** ERROR: setting of PWD environment failed");
			}
		}
	}	
	
	// "help" command prints out manual to screen
	else if (strcmp(argv[0], "help") == 0) {
		readme = fopen("/Desktop/projects-os/projects/myshell/src/readme", "r"); // open the readme file
		if (!readme)
			printf("%s\n","*** ERROR: error opening file");
		else {
			// get the contents of readme
			while((fgets(help, sizeof(readme), readme))!=NULL)
				fputs(help, stdout); // display contents of readme
			fclose(readme); // close the pointer to file
		}
	}

	// "pause" command
	else if (strcmp(argv[0], "pause") == 0) {
		getpass("Press Enter to continue..."); // wait for ENTER key
		// input is not echoed
	}
	/* pass command on to OS shell */
	else {
		// "clr" command
		if (strcmp(argv[0], "clr") == 0)
			argv[0]="clear";
			// "dir" and "dir <directory>"
		else if (strcmp(argv[0], "dir") == 0) {
			for (j=63;j>1;j--)
				argv[j]=argv[j-1];
		argv[0]="ls";
		argv[1]="-al";
		}		

		/************** FORK and EXEC ******************/
		pid_t pid;
		int status;
		if ((pid = fork()) < 0) {
			/* fork a child process */
			printf("*** ERROR: forking child process failed\n");
			exit(1);
		}
		else if (pid == 0) {
			/* for the child process:*/

			/* --------I/O REDIRECTION-------*/
			// output redirection
			if (opredirect==1) {
				// open output file
				if (!(freopen(outfile, opredirectmode, stdout)))
					printf("%s\n","*** ERROR: error opening file");
			}
			// input redirection
			if (ipredirect==1) {
			// open input file
				if (!(freopen(infile, "r", stdin)))
					printf("%s\n","*** ERROR: error opening file");
			}
			/* Set PARENT environment string */
			strcpy(tempbuff,"PARENT=");
			strcat(tempbuff,getenv("SHELL")); // get SHELL environment string
			if (putenv(strdup(tempbuff))) // set PARENT to shell's full pathname
				printf("%s\n","*** ERROR: setting of PARENT environment failed");
			
			/* COMMANDS and I/O REDIRECTION EXECUTION*/
			if (execvp(*argv, argv) < 0) {
				// execute the command
				printf("*** ERROR: exec failed\n");
				exit(1);
			}
			if (opredirect==1)
				fclose(stdout);
			if (ipredirect==1)
				fclose(stdin);
				// close stream
				// close stream
		}
		// continued execution in parent process
		if (foreground)
			waitpid(pid, &status, WUNTRACED);
	}
}