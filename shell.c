#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void custom_signal(int signo){
	
	signal(signo, SIG_IGN);
	printf("\nAGAIN : ");
}

void print_env(char* ch){ // getenv 환경변수 출력 및 화면 clear로 지우기
  if(strcmp(ch, "clear")==0) {
    printf("\033[0;0H\033[2J"); //이 문자열을 출력하여 콘솔 창 지우기
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

char** get_env(void){ // 환경변수 설정 함수
	char *ptr, *ch, *next_ptr;
	char **tok = malloc(sizeof(char*) * 20); // 이중 포인터(포인터 배열) 생성
	int i = 0;

	ch = getenv("PATH"); // PATH 환경변수 가져오기
	for(i=0, ptr=ch; ;ptr=NULL,i++){ // strtok_r을 사용하기 위해 문자열 복사
		tok[i] = strtok_r(ptr, ":", &next_ptr); //문자열 : 기준으로 잘라서 tok 배열에 보관
		if(tok[i] == NULL) break; // 남아있는 문자열이 없으면 break
	}
	tok[i] = NULL; //환경변수 배열 마지막에는 null이 있어야 함
	return tok;
}

int cd(char *path){
	return chdir(path);
}

int main(void) {
	char **e = get_env(); // 환경변수 가져오기
	char *env[20]; //복사할 포인터 배열
	pid_t pid;
	int stat_loc;

	signal(SIGINT, custom_signal); //컨트롤 C에 의해 종료되지 않도록 함

	for(int i=0;i<20;i++){
		if(e[i]==NULL){
			env[i]=NULL;
			break;
		}
		env[i] = e[i];
	}

	free(e); //malloc으로 할당된 e 포인터 free

	while(1) {
		int i = 0;
		char *nextptr, *parsed, *ptr;
		char *command[20]; //명령어 배열 보관
		char input[20]; //입력된 명령어를 임시 보관할 배열
		char keyword[30] = "/bin/"; //명령어는 bin 폴더에 보관되어 있음
		char buf[255]; //현재 위치 받아올 문자열(배열)
		
		printf("custom shell | %s $>", getcwd(buf, 255));
		fgets(input,19,stdin); 

		if(strlen(input)>19){
			printf("TOO LONG COMMAND \n");
			continue;
		}
		
		ptr = strchr(input, '\n');
		if(ptr) *ptr=0; // 문자열 맨 끝 개행 문자 없애기

		if(strcmp("exit",input) == 0) { //exit 명령어 뜨면 종료하도록
			printf("Exiting...\n");
			break;
		}
		if(strcmp(input, "clear")==0 || strcmp(input, "home")==0 || strcmp(input, "shell")==0 || strcmp(input, "hostname")==0){
			print_env(input);
			continue;
		} // clear 입력시 화면 지울 수 있고 또한 환경변수 출력 가능

		parsed = strtok_r(input, " ",&nextptr);
	       	while(parsed) {
			command[i] = parsed;
			i++;
			parsed = strtok_r(NULL, " ", &nextptr);
		}
		command[i] = NULL;

		if(strcmp("cd", command[0]) == 0) { // cd 명령어를 입력받으면 따로 수행하도록
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
			if(execve(keyword, command, env) == -1) { //keyword에는 명령어 파일의 전체 경로를 명시해야 함
				perror("FAILED");
				exit(0);
			}
		}
		if(pid > 0){
			wait(&stat_loc);
		}
	}
	return 0;
}

