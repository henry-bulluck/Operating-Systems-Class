#define _GNU_SOURCE //this is needed to be able to use execvpe 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <fcntl.h>

//#define DEBUG

typedef struct {
  char* binary_path;
  char* arguments;
  char* extra_environment;
  short copy_environment;
  short use_path;
  short niceness;
  short wait;
  unsigned int timeout;
} command;

//function prototypes
void print_parsed_command(command);
short parse_command(command*, char*);
void free_command(command);
void process_command(command);

//You might need to add more function prototypes (e.g., processcommand)

//global variables here
//Hint: You might need to save information (like a counter) for children processes

short getlinedup(FILE* fp, char** value){
  char* line = NULL;
  size_t n = 0;
  //get one line
  int ret = getline(&line, &n, fp);

  if(ret == -1){
    //the file ended
    return 0;
  }
  //remove \n at the end
  line[strcspn(line, "\n")] = '\0';
  //duplicate the string and set value
  *value = strdup(line);
  free(line);

  return 1;
}

//parse a command_file and set a corresponding command data structure
short parse_command(command* parsed_command, char* cmdfile){
  FILE* fp = fopen(cmdfile, "rb");
  if(!fp){
    //the file does not exist
    return 0;
  }

  char* value;
  short ret;
  int intvalue;

  ret = getlinedup(fp,&value);
  if(!ret){
    fclose(fp); return 0;
  }
  parsed_command->binary_path = value;


  ret = getlinedup(fp,&value);
  if(!ret){
    fclose(fp); return 0;
  }
  parsed_command->arguments = value;

  ret = getlinedup(fp,&value);
  if(!ret){
    fclose(fp); return 0;
  }
  parsed_command->extra_environment = value;


  ret = getlinedup(fp,&value);
  if(!ret){
    fclose(fp); return 0;
  }
  intvalue = atoi(value);
  if(intvalue != 0 && intvalue != 1){
    fclose(fp); return 0;
  }
  parsed_command->copy_environment = (short)intvalue;


  ret = getlinedup(fp,&value);
  if(!ret){
    fclose(fp); return 0;
  }
  intvalue = atoi(value);
  if(intvalue != 0 && intvalue != 1){
    fclose(fp); return 0;
  }
  parsed_command->use_path = (short)intvalue;


  ret = getlinedup(fp,&value);
  if(!ret){
    fclose(fp); return 0;
  }
  intvalue = atoi(value);
  if(intvalue <-20 || intvalue >19){
    fclose(fp); return 0;
  }
  parsed_command->niceness = (short)intvalue;

  ret = getlinedup(fp,&value);
  if(!ret){
    fclose(fp); return 0;
  }
  intvalue = atoi(value);
  if(intvalue != 0 && intvalue != 1){
    fclose(fp); return 0;
  }
  parsed_command->wait = (short)intvalue;

  ret = getlinedup(fp,&value);
  if(!ret){
    fclose(fp); return 0;
  }
  intvalue = atoi(value);
  if(intvalue < 0){
    fclose(fp); return 0;
  }
  parsed_command->timeout = (unsigned int)intvalue;

  return 1;
}

//useful for debugging
void print_parsed_command(command parsed_command){
  printf("----------\n");
  printf("binary_path: %s\n", parsed_command.binary_path);
  printf("arguments: %s\n", parsed_command.arguments);
  printf("extra_environment: %s\n", parsed_command.extra_environment);
  printf("copy_environment: %d\n", parsed_command.copy_environment);
  printf("Niceness: %d\n", parsed_command.niceness);
  printf("wait: %d\n", parsed_command.wait);
  printf("timeout: %d\n", parsed_command.timeout);
}

void free_command(command cmd){
  free(cmd.binary_path);
  free(cmd.arguments);
  free(cmd.extra_environment);
}

void process_command(command parsed_command){
  /*
  process_command will:
  - get a parsed_command variable
  - create a child process
  - set niceness, arguments, envirionment variables, ...
  - call a proper variant of execve based on the use_path value
  - print when a child process is created and when any child process is terminated
  - if necessary (depending on ``wait'' argument), wait for the termination of the program
  */
  int rc = fork();
  if (rc < 0){
    fprintf(stderr, "fork failed.\n");
    exit(1);
  }
  else if (rc == 0){
    //
    id_t pid = getpid();
    printf("New child process started %d\n", (int) pid);
    //turns arguments into list
    char *myargs[3];
    //adds timeout function
    if(parsed_command.timeout > 0){
    	char cmd[100];
    	sprintf(cmd,"/usr/bin/timeout-perserve-status -k -l %d %s", parsed_command.timeout, parsed_command.binary_path);
    	myargs[0] = strdup(cmd);
    }
    else{
    	myargs[0] = strdup(parsed_command.binary_path);
    }
    myargs[1] = strdup(parsed_command.arguments);
    myargs[2] = NULL;
    
    //Here is where we clear and set up new enviroment
    if(!parsed_command.copy_environment){
    	if(clearenv() !=0){
    	    fprintf(stderr, "clearenv() error.\n");
    	    exit(1);
    	}
    	else{
    	    setenv("HOME", "/", 1);
    	    setenv("PATH", "/bin:/user/bin", 1);
    	}
    }
    
    //parses new enviroment variables
    char * newEnvVars = strtok(parsed_command.extra_environment, "=");
    while(newEnvVars != NULL){
    	setenv(newEnvVars, strtok(NULL, "##"), 1);
	newEnvVars = strtok(NULL, "=");
    }
    
    //sets niceness
    int which = PRIO_PROCESS;
    int ret = setpriority(which, pid, parsed_command.niceness);
    

    //saves enviroment to be passed
    extern char** environ;
    
    //executes program
    execvpe(myargs[0], myargs, environ);
    
  }
  else{
    //
    if (parsed_command.wait){
      int wc = wait(NULL);
    }
  }
  
}

int main(int argc, char *argv[], char* env[]) {

  for(int ncommand=1; ncommand<argc; ncommand++){
    command parsed_command;
    int ret = parse_command(&parsed_command, argv[ncommand]);
    if (!ret){
      printf("command file %s is invalid\n", argv[ncommand]);
      continue;
    }

    //may be useful for debugging
    //print_parsed_command(parsed_command);
    
    process_command(parsed_command);

    //It will be good to free the memory allocated to the data tyes in command structure after command is forked
    free_command(parsed_command);
  }
  
  //remember to wait for the termination of all the child processes, regardless of the value of parsed_command.wait
  



}

