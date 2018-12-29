#include<stdio.h>
#include<spinlock.h>
#include<stdlib.h>
#include<string.h>
#include<sched.h>

void spin_lock(struct spinlock *l) {
	int temp = 0;
	while ((temp = tas(&(l->primeLock))) != 0) {
		;
	}
}

void spin_unlock(struct spinlock *l) {
	l->primeLock = 0;
}

