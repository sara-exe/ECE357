#include "sem.h"

void errorCheck(char func[],int retVal) {
	if (retVal < 0) {
		fprintf(stderr,"errno: %d\n",retVal);
		fprintf(stderr,"Error with %s: %s\n",func,strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void sem_init(struct sem *s,int count) {
	int i;
	for (i = 0; i < N_PROC -1; i++) {
		s->waitproc[i] = 0;
	}
	s->count = count;
	s->proc = 0;
	s->lock = 0;
}

int sem_try(struct sem *s) {
	if (s->count = 0) {
		s->count--;
		s->lock = 0;
		return 1;
	}
	else {
		s->waitproc[s->count] = getpid();
		s->lock = 0;
		return 0;
	}
}

void sem_wait(struct sem *s) {
	sigset_t oldmask,newmask,mask;
	sigprocmask(SIG_BLOCK,&newmask,&oldmask);
	sigfillset(&newmask);
	sigdelset(&newmask,SIGUSR1);
	sigdelset(&newmask,SIGINT);
	sigprocmask(SIG_BLOCK,&newmask,&oldmask);
	while (tas(&(s->lock))) {
		;
	}
	if (s->count > 0) {
		s->count--;
		s->lock = 0;
		sigprocmask(SIG_SETMASK,&oldmask,NULL);
		return;
	}
	else {
		s->waitproc[s->proc] = getpid();
		s->proc++;
		s->lock = 0;
		sigfillset(&mask);
		sigdelset(&mask,SIGUSR1);
		sigsuspend(&mask);
	}
}

void sem_inc(struct sem *s) {
	sigset_t oldmask,newmask,mask;
	sigfillset(&newmask);
	sigprocmask(SIG_BLOCK,&newmask,&oldmask);
	while(tas(&(s->lock))) {
		;
	}
	if (s->count > 0) {
		errorCheck("Kill sem_inc",kill(s->waitproc[s->proc],SIGUSR1));
		s->proc--;
	}
	else if (s->count == 0) {
		s->count++;
	}
	s->lock = 0;
	sigprocmask(SIG_SETMASK,&oldmask,NULL);
}


