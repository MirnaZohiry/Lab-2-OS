/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <Mirna Zohiry, Mohannad Abdo, Luisa Rojas-Garcia>
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
 #include <sys/stat.h> 
#include <string.h>

// Constants
#define BUFFER_LEN 256

// Global environment variables here
extern char **environ;

// Define functions declared in myshell.h here
int main(int argc, char *argv[]) {
	
	// Variable declaration
    char *dir;
    char buffer[BUFFER_LEN] = { 0 };
    char command[BUFFER_LEN] = { 0 };
    char arg[BUFFER_LEN] = { 0 };
    char directory[BUFFER_LEN] = {"/home"};
    FILE *filepnt;
    filepnt = stdin;
    getcwd(directory, BUFFER_LEN);
    struct stat sb;
	int err = stat(buffer, &sb);
    
    // First prompt
    printf("%s/[myshell]> # ", directory);

    // Infinite loop getting command input from users
    while (fgets(buffer, BUFFER_LEN, filepnt) != NULL) {

        // Perform string tokenization to get the command and argument
		memcpy(command, buffer , BUFFER_LEN);
		strtok_r(command, "\n ", &dir);
		strcpy(buffer, dir);
		strtok(buffer, "\n");

        // Check the command and execute the operations for each command
        // cd command -- Change the current directory
        if (strcmp(command, "cd") == 0) {
        	// No directory specified
            if (strcmp(buffer, "") == 0) {
                fputs("ERROR: Please enter a directory.\n", stderr);
            }
            // Directory entered doesn't exist
            else if (err == -1) {
            	fputs("ERROR: Please enter a VALID directory.\n", stderr);
            }
            // If there are no errors,
            else {
            	// Change the current directory to the one provided
                chdir(buffer);
            }
        }
		// pause command -- Pause operation of the shell until 'Enter' is pressed
        else if (strcmp(command, "pause") == 0) {
            char s[2];
            printf("System paused. Press ENTER to continue.\n");
            // While there is no new line (ENTER),
            while(strcmp(s, "\n")!= 0) {
            	// Accept user unput
            	fgets(s, 2, stdin);
            }
            printf("System resumed.\n");
	   }
        // quit command -- Exit the shell
        else if (strcmp(command, "quit") == 0) {
            return EXIT_SUCCESS;
        }
        // environ command -- Llst all environment strings
        else if (strcmp(command, "environ") == 0) {
        	// Display strings using the env system call
            system("env");
        }
        // clr command -- Clear the terminal
        else if (strcmp(command, "clr") == 0) {
        	// Clear the terminal 
            system("clear");
        }
        // dir command -- List contents of directory given
        else if (strcmp(command, "dir") == 0) {
        	// No directory specified
            if(buffer[0] == NULL) {
                fputs("ERROR: Please enter a directory.\n", stderr);
            }
            else {
                memcpy(directory, buffer, BUFFER_LEN);
                // Directory provided doesn't exist
                if (err == -1) {
                	fputs("ERRPR: Please enter a VALID directory.\n", stderr);
                }
                else {
                	// Make currentdir the directory provided
                	DIR *currentdir = opendir(buffer);
                	// Create and initialize struct for every item in the directory
                	struct dirent *entry = NULL;
                	// Display all
                	while((entry = readdir(currentdir))) {
                    	printf("%s\n", entry->d_name);
                	}
                	// Close directory stream
                	closedir(currentdir);
            	}	
            }
        }
        // echo command -- Display input
        else if (strcmp(command, "echo") == 0) {
            printf("%s\n", buffer);
        }
        // myshell command -- Take command like input from file (batch)
        else if (strcmp(command, "myshell") == 0) {	
        	// Open batch file for reading only
        	filepnt = fopen(buffer, "r");
        }
        // help command -- Display user manual (readme)
        else if (strcmp(command, "help") == 0) {

            // Displays the readme file using the more filter
            char *args[] = {"more", "readme", 0};
            execvp("more", args);

        }
        // Command not defined - doesn't exist
        else {
            fputs("ERROR: Unsupported command, use \"help\" to display the manual\n", stderr);
        }
        
        // If reaches end of file, stop
        if(feof(filepnt)) {
            filepnt = stdin;
        }

        // Loop for "error" message to print before next prompt
        int x = 0;
        while(x < 1000000) {
            x++;
        }

        // Always display prompt, ready to take input from user
        getcwd(directory, BUFFER_LEN);
        printf("%s/[myshell]> # ", directory);
    }
    return EXIT_SUCCESS;
}
