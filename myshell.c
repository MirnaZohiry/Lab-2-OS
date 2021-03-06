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
    // Used for execvp function below - fork()
    //char *args[] = {"more", "readme", 0};
    
    FILE *filepnt;
    filepnt = stdin;

    FILE *ofilepnt;
    ofilepnt = stdout;

    getcwd(directory, BUFFER_LEN);
    
    // First prompt
    fprintf(ofilepnt,"%s/[myshell]> # ", directory);

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
            struct stat sb;
            int err = stat(buffer, &sb);
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
            char s[2]={' '};
            fprintf(ofilepnt, "%s\n", "System paused. Press ENTER to continue.");
            // While there array is not empty
            while(strcmp(s, "\n")!= 0) {
            	// Accept user unput
            	fgets(s, 2, stdin);
            }

            fprintf(ofilepnt,"System resumed.\n");
	   }
        // quit command -- Exit the shell
        else if (strcmp(command, "quit") == 0) {
            return EXIT_SUCCESS;
        }
        // environ command -- List all environment strings
        else if (strcmp(command, "environ") == 0) {
        	
            int i = 0;

            // Displays variables using a loop
            while(environ[i] != NULL) {
                fprintf(ofilepnt, "%s\n", environ[i]);
                i++;
          }
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
                struct stat sb;
                int err = stat(buffer, &sb);
                if (err == -1) {
                	fputs("ERROR: Please enter a VALID directory.\n", stderr);
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
            fprintf(ofilepnt,"%s\n", buffer);
        }
        // myshell command -- Take command like input from file (batch)
        else if (strcmp(command, "myshell") == 0) {	
        	// Open batch file for reading only
        	filepnt = fopen(buffer, "r");
        }
        // help command -- Display user manual (readme)
        else if (strcmp(command, "help") == 0) {
            
            char current_char;
            FILE *readme_file;
            readme_file = fopen("readme", "r");
            // File exists in current directory
            if(readme_file) {
                // End of file hasn't been reached
                while ((current_char = fgetc(readme_file)) != EOF)
                    // Read current characted
                    putchar(current_char);
                // Close file stream
                fclose(readme_file);
            fprintf(ofilepnt, "\n\n");
            }
            // File does not exist in current directory
            else {
                fputs("An error has occured. Please try again later.\n", stderr);
            }

            /*
            // Displays the readme file using the more filter - but need fork()
            pid_t pid = fork();
            
            if (pid==0) {
                execvp("more", args);
                exit(0);
            
            }
            else if(pid>0){
                wait(NULL);
            }*/
        }
        else if (strcmp(command, "io") == 0) {
            // No arguments are provided
           if (buffer[0] == NULL) {
                fprintf(ofilepnt, "Please specify input and output files.");
            } else {

                // Create tokens array and int to keep track of its size
                char * tokens[3];
                size_t n = 0;

                // Tokenize buffer into "<", "input", ">", and "output"
                for (char * p = strtok(buffer, " "); p; p = strtok(NULL, " ")) {
                    tokens[n++] = p;
                }

                // The user has provided more than what is needed
                if (n > 4) {
                    fprintf(ofilepnt, "Too many arguments. Use \"help\" to display the manual.\n");
                }
                else {
                    if (strcmp(tokens[0], "<") == 0 &&
                       (strcmp(tokens[2], ">") || strcmp(tokens[2], ">>"))) {
                        
                        // Set up streams
                        FILE *curr_filepnt = filepnt;
                        FILE *curr_ofilepnt = ofilepnt;

                        // Open file to read form: token[0]
                        curr_filepnt = filepnt;
                        filepnt = fopen(tokens[1], "r");

                        // The output file name corresponds to the second token in the array
                        if (strcmp(tokens[2], ">") == 0) {
                            // Write/overwrite output to file
                            curr_ofilepnt = ofilepnt;
                            // Open file to write to: token[3]
                            ofilepnt = fopen(tokens[3], "w");
                        }
                        else if (strcmp(tokens[2], ">>") == 0) {
                            // Append to file
                            curr_ofilepnt = ofilepnt;
                            ofilepnt = fopen(tokens[3], "a");
                        }                           
                    }
                    else {
                        // Incorrect syntax
                        fprintf(ofilepnt, "Please check your syntax. Use \"help\" to display the manual.\n");
                    }

                }
            }

        }

        else if (strcmp(buffer, "&") == 0) {

        	char *str1 = "open -a "; // For Mac
        //	char *str1 = "./" // For Linux
            // Concatinate into program string
        	char *program = (char *) malloc(1 + strlen(str1)+ strlen(command));
      		strcpy(program, str1);
      		strcat(program, command);

        	fprintf(ofilepnt, "Executing program: %s\n", command);
            // Execute program using system call
        	system(program);
        }
        // Command not defined - doesn't exist
        else {
            fputs("ERROR: Unsupported command, use \"help\" to display the manual.\n", stderr);
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
