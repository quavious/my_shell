#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void custom_signal(int signo){
	
	signal(signo, SIG_IGN);
	printf("\nAGAIN : ");
}

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

int cd(char *path){
	return chdir(path);
}

int main(void) {
	char **e = get_env();
	char *env[20];
	pid_t pid;
	int stat_loc;

	signal(SIGINT, custom_signal);

	for(int i=0;i<20;i++){
		if(e[i]==NULL){
			env[i]=NULL;
			break;
		}
		env[i] = e[i];
	}

	free(e);

	while(1) {
		int i = 0;
		char *nextptr, *parsed, *ptr;
		char *command[20];
		char input[20];
		char keyword[30] = "/bin/";
		char buf[255];
		
		printf("custom shell | %s $>", getcwd(buf, 255));
		fgets(input,19,stdin);
		//input[strlen(input) - 1] = '\0';
		
		ptr = strchr(input, '\n');
		if(ptr) *ptr=0;

		if(strcmp("exit",input) == 0) {
			//free(input);
			printf("Exiting...\n");
			break;
		}

		//command = get_input(input);
		parsed = strtok_r(input, " ",&nextptr);
	       	while(parsed) {
			command[i] = parsed;
			i++;
			parsed = strtok_r(NULL, " ", &nextptr);
		}
		command[i] = NULL;

		if(strcmp("cd", command[0]) == 0) {
			if(cd(command[1]) < 0) {
				perror(command[1]);
			}
			continue;
		}

		strcat(keyword, command[0]);
		pid = fork();
		if(pid < 0) {
			perror("fork failed \n");
			exit(1);
		}
		if(pid == 0){
			if(execve(keyword, command, env) == -1) {
				perror("FAILED");
				exit(0);
			}
		}
		if(pid > 0){
			wait(&stat_loc);
		}

		//free(command);
		//free(input);
	}
	return 0;
}

