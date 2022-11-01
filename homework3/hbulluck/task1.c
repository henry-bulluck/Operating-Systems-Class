#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char* env[]){
	//prints all arguments
	for(int i=0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
	
	int end = 0; //boolean if the end of env is reached or not
	int i=0;
	//while not at the end
	while(!end){
		printf("%s\n",env[i]); //print variable
		i++; 
		end = (env[i]==NULL); //check if next variable is the end
	}
	//prints process IDs
	printf("Parent Process ID: %d\n",(int) getppid());
	printf("Child Process ID: %d\n",(int) getpid());
}
