#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char** get_env(void){
	char *ptr, *ch, *next_ptr;
	char **tok = malloc(sizeof(char*) * 20);
	int i = 0;

	ch = getenv("PATH");
	for(i=0, ptr=ch; ;ptr=NULL,i++){
		tok[i] = strtok_r(ptr, ":", &next_ptr);
		if(tok[i] == NULL) break;
	}
	tok[i] = NULL;
	return tok;
}

char **get_input(char *input){
	char **command = malloc(8 * sizeof(char *));
	char *separator = " ";
	char *parsed, *nextptr;
	int index = 0;

	parsed = strtok_r(input, separator, &nextptr);
	while(parsed != NULL){
		command[index] = parsed;
		index++;
		parsed = strtok_r(NULL, separator, &nextptr);
	}

	command[index] = NULL;
	return command;
}

int main(void) {
	char **env = get_env();
	pid_t pid;
	int stat_loc;

	while(1) {
		int i = 0;
		char *nextptr, *parsed;
		char *command[20];
		char* input = malloc(sizeof(char)*20);
		printf("$>");
		input = fgets(input,19,stdin);
		input[strlen(input) - 1] = '\0';

		if(strcmp("exit",input) == 0) {
			free(input);
			break;
		}

		//command = get_input(input);
		parsed = strtok_r(input, " ",&nextptr);
	       	while(parsed) {
			command[i] = parsed;
			printf("%s \n", command[i]);
			i++;
			parsed = strtok_r(NULL, " ", &nextptr);
		}
		command[i] = NULL;	

		pid = fork();
		if(pid == 0){
			if(execvp(command[0], command) == -1) {
				printf("FAILED \n");
				exit(0);
			}
		}
		if(pid > 0){
			wait(&stat_loc);
		}

		//free(command);
		free(input);
	}



	free(env);
	return 0;
}

