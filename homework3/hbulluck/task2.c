#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char* env[]){

	int argFork = fork();
	if(argFork < 0){
		fprintf(stderr, "fork failed\n");
		exit(1);
	}
	else if(argFork == 0){
		//arg shtuff
		printf("Child process created to print arguments: The process id is: %d\n", (int) getpid());
		for(int i=0;i<argc;i++){
			printf("%s\n",argv[i]);
		}
	}
	else{
		int wc = wait(NULL);
		int envFork = fork();
		if(envFork < 0){
			fprintf(stderr, "fork failed\n");
			exit(1);
		}
		else if(envFork == 0){
			//env shtuff
			printf("Child process created to print enviroment variables: The process id is: %d\n", (int) getpid());
			int end = 0; //boolean if the end of env is reached or not
			int i=0;
			//while not at the end
			while(!end){
				printf("%s\n",env[i]); //print variable
				i++; 
				end = (env[i]==NULL); //check if next variable is the end
			}
		}
		else{
			wc = wait(NULL);	
		}
	}

	return 0;
}
