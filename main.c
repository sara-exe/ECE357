#include "sem.h"
#include "fifo.h"
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/resource.h>
#include<setjmp.h>
#include<string.h>
#include<signal.h>

void signalhandler(int sig) {
}

int main() {
	struct fifo *fifocat;
	struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_handler = signalhandler;
	int i;
	int status;
	errorCheck("sigaction",sigaction(SIGUSR1,&sigact,NULL));
	fifocat = mmap(0,sizeof(struct fifo),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,0,0);
	if (fifocat == MAP_FAILED) {
		errorCheck("bad cat",-100);
	}
	fifo_init(fifocat);
	int fd;
	errorCheck("file",fd = open("test.txt",O_WRONLY|O_CREAT|O_TRUNC,0666));
	int fd2;
	errorCheck("file",fd2 = open("test2.txt",O_WRONLY|O_CREAT|O_TRUNC,0666));
	int ppid = getpid();
	int pids[6] = {0};
	int cpid;
	int myprocess;
	int j, k;
	char buf[32] = {0};
	char buf2[32] = {0};
	int n = 5;
	for (i = 0; i < n; i++) {
		if (ppid == getpid()) {
			errorCheck("fork",cpid = fork());
			fprintf(stderr, "Forking error %d\n",cpid);
			myprocess = i;
		}
		if (cpid != 0) {
			pids[i] = cpid;
		}
		else {
			sleep(2);
		}
	}
	if (ppid == getpid()) {
		for (i = 0; i < n; i++) {
			waitpid(pids[i],&status,0);
			fprintf(stderr,"Error in pid[%d] &d\n",i,pids[i]);
		}
	}
	else {
		fprintf(stderr,"Error in myprocess: %d\n",myprocess);
		if (myprocess == 2) {
			for (j = 0; j < (n-1)*10; j++) {
				sprintf(buf,"%lu\n",fifo_rd(fifocat));
				write(fd,buf,sizeof(buf));
			}
		}
		else {
			for (k = 0; k < 10; k++) {
				fifo_wr(fifocat,myprocess*10 + k);
				sprintf(buf2,"%d\n",myprocess*10 + k);
				write(fd2,buf2,sizeof(buf2));
			}
		}
	}
}

