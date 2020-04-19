#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void custom_signal(int signo){
	
	signal(signo, SIG_IGN);
	printf("\nINPUT quit TO QUIT COMMAND AGAIN\n");
}

void print_env(char* ch){
  if(strcmp(ch, "clear")==0) {
    printf("\033[0;0H\033[2J");
  }
  else if(strcmp(ch, "home")==0){
    printf("%s\n",getenv("HOME"));
  }
  else if(strcmp(ch, "shell")==0){
    printf("%s\n",getenv("SHELL"));
  }
  else if(strcmp(ch, "hostname")==0){
    printf("%s\n",getenv("HOSTNAME"));
  }
}

char** get_env(void){ // ?섍꼍蹂???ㅼ젙 ?⑥닔
	char *ptr, *ch, *next_ptr;
	char **tok = malloc(sizeof(char*) * 20); // ?댁쨷 ?ъ씤???ъ씤??諛곗뿴) ?앹꽦
	int i = 0;

	ch = getenv("PATH"); // PATH ?섍꼍蹂??媛?몄삤湲?
	for(i=0, ptr=ch; ;ptr=NULL,i++){ // strtok_r???ъ슜?섍린 ?꾪빐 臾몄옄??蹂듭궗
		tok[i] = strtok_r(ptr, ":", &next_ptr); //臾몄옄??: 湲곗??쇰줈 ?섎씪??tok 諛곗뿴??蹂닿?
		if(tok[i] == NULL) break; // ?⑥븘?덈뒗 臾몄옄?댁씠 ?놁쑝硫?break
	}
	tok[i] = NULL; //?섍꼍蹂??諛곗뿴 留덉?留됱뿉??null???덉뼱????
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
	char **e = get_env(); // ?섍꼍蹂??媛?몄삤湲?
	char *env[20]; //蹂듭궗???ъ씤??諛곗뿴
	pid_t pid;
	int stat_loc;

	signal(SIGINT, custom_signal); //而⑦듃濡?C???섑빐 醫낅즺?섏? ?딅룄濡???

	for(int i=0;i<20;i++){
		if(e[i]==NULL){
			env[i]=NULL;
			break;
		}
		env[i] = e[i];
	}

	free(e); //malloc?쇰줈 ?좊떦??e ?ъ씤??free

	while(1) {
		int i = 0;
		char *nextptr, *parsed, *ptr;
		char *command[20]; //紐낅졊??諛곗뿴 蹂닿?
		char input[20]; //?낅젰??紐낅졊?대? ?꾩떆 蹂닿???諛곗뿴
		char keyword[30] = "/bin/"; //紐낅졊?대뒗 bin ?대뜑??蹂닿??섏뼱 ?덉쓬
    //char cls_keyword[30] = "/usr/bin/";
		char buf[255]; //?꾩옱 ?꾩튂 諛쏆븘??臾몄옄??諛곗뿴)
		
		printf("custom shell | %s $>", getcwd(buf, 255));
		fgets(input,19,stdin); 
    
    if(strlen(input)>19){
      printf("TOO LONG COMMAND \n");
      continue;
    }
		//input[strlen(input) - 1] = '\0';
		ptr = strchr(input, '\n');
		if(ptr) *ptr=0; // 臾몄옄??留???媛쒗뻾 臾몄옄 ?놁븷湲?

		if(strcmp("quit",input) == 0 || strcmp("QUIT",input) == 0) { //exit 紐낅졊???⑤㈃ 醫낅즺?섎룄濡?
			//free(input);
			printf("Exiting...\n");
			break;
		}

    if(strcmp(input, "clear")==0 || strcmp(input, "home")==0 || strcmp(input, "shell")==0 || strcmp(input, "hostname")==0){
      print_env(input);
      continue;
    }
		//command = get_input(input);
		parsed = strtok_r(input, " ",&nextptr);
	       	while(parsed) {
			command[i] = parsed;
			i++;
			parsed = strtok_r(NULL, " ", &nextptr);
		}
		command[i] = NULL;

		if(strcmp("cd", command[0]) == 0) { // cd 紐낅졊?대? ?낅젰諛쏆쑝硫??곕줈 ?섑뻾?섎룄濡?
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
      if(execve(keyword, command, env) == -1) { //keyword?먮뒗 紐낅졊???뚯씪???꾩껜 寃쎈줈瑜?紐낆떆?댁빞 ??
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
