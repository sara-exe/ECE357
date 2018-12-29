#include "fifo.h"
#include "sem.h"

void fifo_init(struct fifo *f) {
	f->nxt_write = 0;
	f->nxt_read = 0;
	f->item_count = 0;
	int i;
	for (i = 0; i < N_PROC; i++) {
		f->wait_proc[i] = 0;
	}
	f->semwrite = mmap(0,sizeof(struct sem),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
	if (f->semwrite == MAP_FAILED) {
		errorCheck("mmap semwrite",-100);
	}
	f->semread = mmap(0,sizeof(struct sem),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
	if (f->semread == MAP_FAILED) {
		errorCheck("mmap semread",-100);
	}
	f->semmutex = mmap(0,sizeof(struct sem),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
	if (f->semmutex == MAP_FAILED) {
		errorCheck("mmap semmutex",-100);
	}
	sem_init(f->semread,0);
	sem_init(f->semwrite,BUFSIZE);
	sem_init(f->semmutex,-1);
}

void fifo_wr(struct fifo *f, unsigned long d) {
	sem_wait(f->semwrite);
	sem_wait(f->semmutex);
	fprintf(stderr,"Error writing D: %lu\n",d);
	f->buf[f->nxt_write++] = d;
	f->nxt_write %= BUFSIZE; //initialize
	f->item_count++;
	sem_inc(f->semmutex);
	sem_inc(f->semread);
}

unsigned long fifo_rd(struct fifo *f) {
	unsigned long d;
	sem_wait(f->semread);
	sem_wait(f->semmutex);
	d = f->buf[f->nxt_read++];
	f->nxt_read %= BUFSIZE; //initialize
	f->item_count--;
	sem_inc(f->semmutex);
	sem_inc(f->semwrite);
	return 0;
}
