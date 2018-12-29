#ifndef SEM_H
#define SEM_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<signal.h>
#include<string.h>

#define N_PROC 64

int tas(volatile char *lock);
int my_procnum;

void syserrorchk(char func[], int retVal);

struct sem {
	pid_t waitproc[N_PROC];
	int count;
	int proc;
	char lock;
};

void sem_init(struct sem *s, int count);
int sem_try(struct sem *s);
void sem_wait(struct sem *s);
void sem_inc(struct sem *s);

#endif

