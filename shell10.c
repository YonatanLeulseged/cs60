/* Author(s): Yonatan Leulseged
 *            Ricardo Hernandez
 *
 * This is lab9.c the csc60mshell
 * This program serves as a skeleton for doing labs 9 and 10.
 * Student is required to use this program to build a mini shell
 * using the specification as documented in direction.
 * Date: Spring 2020
 */

/* The include files section */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* Define Section */
#define MAXLINE 80
#define MAXARGS 20
#define MAX_PATH_LENGTH 50
#define TRUE 1

/* function prototypes */
int Parse_Input(char *cmdline, char **argv);

//The two function prototypes below will be needed in lab10. 
//Leave them here to be used later.
 void Process_Input(int argc, char **argv); 
 void Handle_Redir(int argc, char *argv[]);


/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
    char cmdline[MAXLINE];
    char *argv[MAXARGS];
    int argc;
    char pathHold[MAX_PATH_LENGTH];
    char *dir;
    int status;
    pid_t pid;

    /* Loop forever to wait and process commands */
    while (TRUE)
    {   
		/* Print your shell name: csc60msh (m for mini shell) */
		printf("csc60msh> ");

		/* Read the command line */
		fgets(cmdline, MAXLINE, stdin);

		/* Call Parse_Input to build argc/argv */

       argc = Parse_Input(cmdline,argv);

		/* If user hits enter key without a command, continue to loop */
		/* again at the beginning */
		/*  Hint: if argc is zero, no command declared */
		/*  Hint: look up for the keyword "continue" in C */

		/* Handle build-in command: exit, pwd, or cd  */
		/* Put the rest of your code here */

        //Print argc and argv
        printf("argc = %i\n", argc);
        int i;
        for(i = 0;i < argc; i++){
           printf("Argv %i = %s \n", i, argv[i]);                         
        }

        if(argc == 0){
            continue;
        }

        if(strcmp(argv[0], "exit") == 0){
            exit(EXIT_SUCCESS);
        }
        else if(strcmp(argv[0], "pwd") == 0){
            getcwd(pathHold, MAX_PATH_LENGTH);
            printf("%s\n",pathHold);
            continue;
        }
        else if(strcmp(argv[0], "cd") == 0){ 
        
            if(argc == 1){
                 dir = getenv("HOME");
             }
            else{
                dir = argv[1];
            }
            if(chdir(dir) != 0){
             perror("Error changing directory\n");
            }
        }
      
 
//.......................IGNORE........................	
	/* Else, fork off a process */
      else {
	    pid = fork();
          switch(pid)
    	    {
	    	case -1:
		    	perror("Shell Program fork error");
	            exit(EXIT_FAILURE);
	   		case 0:
		    	/* I am child process. I will execute the command, */
		    	/* and call: execvp */
		    	Process_Input(argc, argv);
		    	break;
	   		default:
		    	/* I am parent process */
		    	if (wait(&status) == -1)
		    		perror("Parent Process error");
		    	else
		   		printf("Child returned status: %d\n\n",status);
		    	break;
	    } 	/* end of the switch */
	}		/* end of the if-else-if */
  }	/* end of the while */
} 			/* end of main */

/* ----------------------------------------------------------------- */
/*                  Parse_Input                                        */
/* ----------------------------------------------------------------- */
/* Parse input line into argc/argv format */

int Parse_Input(char *cmdline, char **argv)
{
    int argc = 0;
    char *separator = " \n\t"; /* Includes space, Enter, Tab */
 
    /* strtok searches for the characters listed in separator */
    argv[argc] = strtok(cmdline, separator);

    while ((argv[argc] != NULL) && (argc+1 < MAXARGS)){ 
    	argv[++argc] = strtok((char *) 0, separator);
    } 		
    return argc;  
}
/* ----------------------------------------------------------------- */
/*                  Process_Input                                    */
/* ----------------------------------------------------------------- */
void Process_Input(int argc, char **argv) {                        
    int ret;
    /* Step 1: Call Handle_Redir to deal with operators:             */
    /* < , or  >, or both                                            */
     Handle_Redir(argc, argv);
     ret = execvp(argv[0],argv);


    /* Step 2: perform system call execvp to execute command         */
    /* Hint: Please be sure to review execvp.c sample program        */
     if (ret == -1) {                                         
        fprintf(stderr, "Error on the exec call\n");               
        _exit(EXIT_FAILURE);                                       
     }                                                             
  }

/* ----------------------------------------------------------------- */
void Handle_Redir(int argc, char *argv[]){
/* ----------------------------------------------------------------- */
//code goes here

    int in_loc = 0;
    int out_loc = 0;
    int i;
    int fd;
    for(i = 0;i < argc; i++){
        if(strcmp(argv[i], ">") == 0){
            if(out_loc != 0){
                fprintf(stderr, "Exit Failure\n");
                _exit(EXIT_FAILURE);
            
            }else if(i == 0){
                fprintf(stderr,"No command entered\n");
                _exit(EXIT_FAILURE);
             }
             out_loc = i;
        }
        else if(strcmp(argv[i], "<") == 0){
            if(in_loc != 0){
                fprintf(stderr,"Cannot input from more than one file.\n");
                _exit(EXIT_FAILURE);
            }
                else if(i == 0) {
                    fprintf(stderr,"No command entered\n");
                     _exit(EXIT_FAILURE); 
                }
                 in_loc = i;
            }
        }
    //end argc for loop

    if(out_loc != 0){
        if(argv[out_loc + 1] == NULL){
            fprintf(stderr,"There is no file\n");
            _exit(EXIT_FAILURE);
        }
        fd = open(argv[out_loc+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if(fd == -1){
        fprintf(stderr,"Error opening the file\n");
        _exit(EXIT_FAILURE);       
    }
    dup2(fd,1);
        if(close(fd) == -1){
            fprintf(stderr,"Close file error\n");
            _exit(EXIT_FAILURE);
        }
        argv[out_loc] = NULL;
    }//end of out_loc if

    
    if(in_loc != 0){
        if(argv[in_loc + 1] == NULL){
            fprintf(stderr,"There is no file\n");
            _exit(EXIT_FAILURE);
        }
     fd = open(argv[in_loc+1],O_RDONLY);
   
    if(fd == -1){
        fprintf(stderr,"Error opening the file\n");
        _exit(EXIT_FAILURE);       
    }
    dup2(fd,0);
        if(close(fd) == -1){
            fprintf(stderr,"Close file error\n");
            _exit(EXIT_FAILURE);
        }
        argv[in_loc] = NULL;
    }// end in_loc if
}          
    //variables to keep track of location in strong (out_loc, in_loc)


/* ----------------------------------------------------------------- */
