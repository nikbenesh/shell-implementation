#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>


// Shell implementation in C

/* This function asks OS to run the external command
*/
void run_external_command(char *args[]) {
	int f = fork();
	if (f == -1) {
		fprintf(stderr, "Failed to create process\n");
	} else if (f == 0) {
		if (execvp(args[0], args) == -1)
			fprintf(stderr, "Failed to execute command\n");
		exit(0);
	} else {
		wait(NULL);
	}
}


/* This function splits the given string into words
   and returns the dynamic array of words
*/
char **split(char *str) {
	char **arr = (char **)malloc(256);
	char *p = strtok(str, " ");
	int i = 0;
	while (p != NULL) {
		arr[i] = p;
		p = strtok(NULL, " ");
		i++;
	}
	arr[i+1] = NULL;
	return arr;
}


/* This function changes the current directory
*/
int cd(char *path) {
	int fd = open(path, O_DIRECTORY | O_RDONLY); // open the directory
	if (fd == -1) { // The directory doesn't exist
		close(fd);
		return -2;
	}
	int r = fchdir(fd);
	close(fd);
	return r;
}

int main(void) {
	char command[256];
	// I assume that a command cannot be more than 256 characters long
	int path_max = 256 - 3;
	// The string 'cd ' is 3 characters long which means that everything
	// that goes after it cannot be more than 253 characters long
	char current_directory[path_max];
	getcwd(current_directory, path_max);
	// Get the current directory where user is

	while (1) {
		printf("%s$mysh > ", current_directory);
		if (fgets(command, 256, stdin) == NULL) {
			fprintf(stderr, "Failed to read command\n");
			continue;
		}

		// Check if the command entered by user contains a new line character
		// And strip it out if so
		if (command[strlen(command) - 1] == '\n')
			command[strlen(command) - 1] = '\0';

		// Check if the command is built-in or external
		if (strcmp(command, "pwd") == 0) {
			// Print the current directory
			printf("%s\n", current_directory);
		} else if (strncmp(command, "cd", 2) == 0) {
			// Change directory
			int r = cd(command + 3);
			// Check the return value and print errors if it is below zero
			if (r == -2) {
				fprintf(stderr, "No such directory\n");
			} else if (r == -1) {
				fprintf(stderr, "Change directory failed\n");
			} else {
				// If no errors occured
				// Update the current directory
				getcwd(current_directory, path_max);
			}
		} else if (strcmp(command, "exit") == 0) {
			// Exit the shell
			return 0;
		} else {
			// The command is external
			run_external_command(split(command));
		}
	}
}