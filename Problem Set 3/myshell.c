/* Sara Huang
 * myshell.c
 * Simple Shell Program
 */

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<assert.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<fcntl.h>
#include<time.h>
#include<limits.h>
#include<sys/resource.h>
#include<sys/types.h>
#include<sys/stat.h>
#define mysh_rl_bufsize 1024
#define mysh_tok_bufsize 64
#define mysh_tok_delim " \t\r\n\a"

char *mysh_read_line(void) {
	int bufsize = mysh_rl_bufsize;
	int position = 0;
	char * buffer = malloc(sizeof(char) * bufsize);
	int c;
	if(!buffer) {
	fprintf(stderr, "mysh: allocation error: %s\n",strerror(errno));
	exit(EXIT_FAILURE);
	}
	while(1) { //read a character
	c = getchar();
	if (c == EOF || c == '\n') {
		buffer[position] = '\0';
		return buffer;
		}
	else {
		buffer[position] = c;
		}
	if (position >= bufsize) {
		bufsize += mysh_rl_bufsize;
		buffer = realloc(buffer,bufsize);
		if (!buffer) {
			fprintf(stderr,"mysh: allocation error: %s\n",strerror(errno));
			exit(EXIT_FAILURE);
			}
		}
	}
}

void ioRedirect(char token[]) {
	int i, id, strm, fd, strt;
	char path[100];
	if (id == 0) {
		fd = open(path,O_RDONLY,0777);
		if (fd < 0) {
			fprintf(stderr,"Error: cannot open file for reading: %s\n",strerror(errno));		
		}
	}
	else if (id == 1) {
		fd = open(path,O_WRONLY|O_CREAT|O_TRUNC,0777);
		if (fd < 0) {
			fprintf(stderr,"Error: cannot open file for reading: %s\n",strerror(errno));
		}
	}
	else if (id == 2) {
		fd = open(path,O_WRONLY|O_CREAT|O_APPEND,0777);
		if (fd < 0) {
			fprintf(stderr,"Error: cannot open file for reading: %s\n",strerror(errno));
		}	
	}
	if (token[0] == '<') {
		id = 0;
		strm = 0;
		strt = 1;
	} //open filename and redirect stdin
	if ((token[0] == '>') && (token[1] != '>')) {
		id = 1;
		strm = 1;
		strt = 1;
	} //open/create/truncate filename and redirect stdout
	if ((token[0] == '2') && (token[1] == '>')) {
 		id = 1;
		strm = 2;
		strt = 2;
	} //open/create/truncate filename and redirect stderr
	if ((token[0] == '>') && (token[1] == '>')) {
		id = 2;
		strm = 1;
		strt = 2;
	} //open/create/append filename and redirect stdout
	if ((token[0] == '2') && (token[1] == '>') && (token[2] == '>')) {
		id = 2;
		strm = 2;
		strt = 3;
	} //open/create/append filename and redirect stderr	
}
	
char **mysh_split_line(char *line) {
	int bufsize = mysh_tok_bufsize, position = 0;
	char ** tokens = malloc(bufsize * sizeof(char*));
	char *token;
	if (!tokens) {
		fprintf(stderr,"mysh: allocation error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
		}
	token = strtok(line,mysh_tok_delim);
	while(token != NULL) {
		tokens[position] = token;
		position++;
		if (position >= bufsize) {
			bufsize += mysh_tok_bufsize;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens) {
				fprintf(stderr,"mysh: allocation error: %s\n",strerror(errno));
						exit(EXIT_FAILURE);
						}
					}
		else if (token == '#') {
			return 1;
			}
		else if ((token[0] == '>') || (token[0] == '<') || (token[0] == '2' && token[1] == '>')) {
			ioRedirect(token);
			}
			token = strtok(NULL,mysh_tok_delim);
		}
		tokens[position] = NULL;
	return tokens;
}

int mysh_launch(char **args) {
	pid_t pid, wpid;
	int status;
	char token[100];
	pid = fork();
	if (pid == 0) { //child process
		if (execvp(args[0],args) == -1) {
			fprintf(stderr,"mysh: error in child process: %s\n",strerror(errno));
			}
		else if (status != 0) {
			ioRedirect(token);
		}
	exit(EXIT_FAILURE);
	}
	else if (pid < 0) {
	fprintf(stderr, "mysh: error in forking: %s\n",strerror(errno));
	}
	else { //parent process
		do {
			wpid = waitpid(pid,&status,WUNTRACED);
		}
		while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

int mysh_cd(char **args);
int mysh_exit(char **args);
char *builtin_str[] = {
	"cd", 
	"exit",
};
int (*builtin_func[])(char **) = {
	&mysh_cd,
	&mysh_exit,
};
int mysh_num_builtins() {
	return sizeof(builtin_str)/sizeof(char *);
}

int mysh_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr,"mysh: expected argument to \"cd\":%s\n",strerror(errno));
	}
	else {
		if (chdir(args[1]) != 0) {
			fprintf(stderr,"mysh: error in changing directory: %s\n",strerror(errno));
		}
	}
	fprintf(stderr,"Changed directory to %s\n",args[1]);
	return 1;
}

int mysh_exit(char **args) {
	fprintf(stderr, "end of file read, exiting shell with exit code 0");
	return 0;
}

int mysh_execute(char **args) {
	int i;
	if (args[0] == NULL) {
		return 1;
	} //empty command
	for (i = 0; i < mysh_num_builtins(); i++) {
		if (strcmp(args[0],builtin_str[i]) == 0) { 
			fprintf(stderr,"Executing command %s with arguments '%s'",args[0],args[1]);
			return (*builtin_func[i])(args);
		}
	}
	return mysh_launch(args);
}

void mysh_loop(void) {
	char *line;
	char **args;
	int status;
	do {
		printf("$ ");
		line = mysh_read_line();
		args = mysh_split_line(line);
		status = mysh_execute(args);
		free(line);
		free(args);
	}
	while(status);
}

int main(int argc, char **argv) {
	if (argc > 2) {
		fprintf(stderr, "Too many arguments to mysh: see syntax for command {argument {argument...} } {redirection_operation {redirection_operation...}}");
		exit(EXIT_FAILURE);
	}
	else if (argc < 2) {
	struct rusage ru;
	struct timeval starter, end;
	gettimeofday(&starter,NULL);
	gettimeofday(&end,NULL);
	time_t start, stop;
	time(&start);
	mysh_loop();
	time(&stop);
	fprintf(stderr,"consuming %f real seconds, %ld user, %ld system",difftime(stop,start),ru.ru_utime.tv_sec,ru.ru_utime.tv_sec/1000000);
	return EXIT_SUCCESS;
	}
}
