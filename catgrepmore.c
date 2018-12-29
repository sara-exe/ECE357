/* Sara Huang
 * ECE357 Problem Set 4, Problem 3
 * Fixing your cat
 * catgrepmore.c
 */

#include<stdlib.h>
#include<stdio.h>
#include<signal.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<string.h>
#include<stdarg.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#define STDIN (0)
#define STDERR (-1)
#define STDOUT (1)

int pid;
int pipe1[2];
void handler (int sig, siginfo_t *si);

int main(int argc, char *argv[]) {
	int fd, fdo = STDOUT;
	if (pipe(pipe1) == -1) {
		perror("unable to create pipe1");
		exit(1);
	}
	//fork (grep pattern)
	if ((pid = fork()) == -1) {
		perror("unable to fork");
		exit(1);
	}
	else if (pid == 0) {
		exec1(); //stdin -> grep -> pipe 1
	} //parent
	if ((pid = fork()) == -1) {
		perror("unable to fork");
		exit(1);
	}
	else if (pid == 0) {
		exec2();//pipe1 -> more -> stdout
	}
	close(pipe1[0]);
	close(pipe1[1]);
	if (argc == 1) {
		fprintf(stderr,"Error: not enough input arguments to catgrepmore; needs at least 1 infile specified\n");
	}
	else {
		while (--argc > 0) {
			++argv;
			if (fd = open(*argv,O_RDONLY) == -1) {
				++argv;
				--argc;
				fprintf(stderr,"Error: cannot open infile %s for reading\n",*argv);
			}
			else if ((fdo = open(*argv,O_RDWR|O_CREAT|O_TRUNC,0666)) == -1) {
				++argv;
				--argc;
				fprintf(stderr,"Error: cannot open infile %s for writing\n",*argv);
			}
			else {
				if (close(fd) == -1) {
					fprintf(stderr,"Error: cannot close infile %s\n",fd);
				}
				else {
					close(fd);
				}
			}
		}
	}
	struct sigaction sa = {0};
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGINT);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	if (sigaction(SIGINT,&sa,NULL) < 0) {
		perror("sigaction");
		return -1;
	}
	char *p;
	do {
		p = fgets(argv,sizeof(argv),stdin);
	}
	while (!p && errno == EINTR); {
		fprintf(stderr,"Total number of files processed: %d\n",sizeof(argv));
		fprintf(stderr,"Total number of bytes processed: %d\n",sizeof(argv));
	exit(1);
	}
	while (khbit() == 0 || getch() == 'q');
	return 0;
}

void exec1() {
	//input from stdin
	//output to pipe1
	dup2(pipe1[1],1);
	close(pipe1[0]);
	close(pipe1[1]);
	execlp("grep","grep","%s\n",NULL);
	perror("unable to exec");
	_exit(1);
}

void exec2() {
	//input from pipe1
	dup2(pipe1[0],0);
	//output to stdout
	close(pipe1[0]);
	close(pipe1[1]);
	execlp("more","more","%s\n",NULL);
	perror("unable to execute");
	_exit(1);
}

void handler(int sig, siginfo_t *si) {
	if (sig == SIGINT) {
		fprintf(stderr,"Signal %i received \n",si->si_signo);
	}
}

