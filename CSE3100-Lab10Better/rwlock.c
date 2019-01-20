#include <pthread.h>
#include "error.h"
#include "rwlock.h"

int rwlock_init(rwlock* lk) {
	int st;
	
	/* Initialize mutex, check for errors */
	st = pthread_mutex_init(&lk->mutex, NULL);
	if(st) {
		pthread_mutex_destroy(&lk->mutex);
		return st;
	}
	
	/* Initialize condition variables, check for errors */
	st = pthread_cond_init(&lk->readcond, NULL);
	if(st) {
		pthread_cond_destroy(&lk->readcond);
		return st;
	}
	st = pthread_cond_init(&lk->writecond, NULL);
	if(st) {
		pthread_cond_destroy(&lk->writecond);
		return st;
	}
	
	/* Initialize all other variables to zero */
	lk->r_active = lk->w_active = 0;
	lk->r_wait = lk->w_wait = 0;
	lk->is_dead = 0;
	
	/* Done! */
	return 0;
}

int rwlock_destroy(rwlock* lk) {
	int st,str,stw,stu;
	
	/* Make sure the lock hasn't already been destroyed */
	if(lk->is_dead) return ERROR_INVALID;
	
	st = pthread_mutex_lock(&lk->mutex);
	if(st) return st;
	
	/* Ensure no one is using this lock */
	if(lk->r_active > 0 || lk->w_active > 0) {
		pthread_mutex_unlock(&lk->mutex);
		return ERROR_BUSY;
	}
	
	/* Ensure no one is waiting to use this lock */
	if(lk->r_wait != 0 || lk->w_wait != 0) {
		pthread_mutex_unlock(&lk->mutex);
		return ERROR_BUSY;
	}
	
	/* Destroy all of the POSIX stuff */
	lk->is_dead = 1;
	str = pthread_cond_destroy(&lk->readcond);
	stw = pthread_cond_destroy(&lk->writecond);
	st = pthread_mutex_unlock(&lk->mutex);
	if(st) return st;
	st = pthread_mutex_destroy(&lk->mutex);
	return (st ? st : (str ? str : stw));
}

int rwlock_rdlock(rwlock* lk) {
	int st;
	
	/* Ensure lock isn't destroyed */
	if(lk->is_dead) return ERROR_INVALID;
	
	/* Acquire lock, need to 'atomically' check/modify rwlock data */
	st = pthread_mutex_lock(&lk->mutex);
	if(st) return st;
	
	/* If a writer is active or waiting, need to wait for it to finish */
	lk->r_wait++;  /* start waiting! */
	while(lk->w_active > 0 || lk->w_wait > 0) {
		st = pthread_cond_wait(&lk->readcond, &lk->mutex);
		if(st) break;
	}
	lk->r_wait--;  /* done waiting! */
	
	/* Reader becomes active! (only if no error occurred while waiting) */
	if(!st) lk->r_active++;
	
	/* Be smart: always unlock when done with critical section */
	st = pthread_mutex_unlock(&lk->mutex);
	
	/* Done! */
	return st;
}

int rwlock_wrlock(rwlock* lk) {
	int st;
	
	/* Ensure lock isn't destroyed yet */
	if(lk->is_dead) return ERROR_INVALID;
	
	/* Acquite lock, need to 'atomically' check/modify rwlock data */
	st = pthread_mutex_lock(&lk->mutex);
	if(st) return st;
	
	/* Need to wait until no other readers or writers are active */
	lk->w_wait++;
	while(lk->w_active > 0 || lk->r_active > 0) {
		st = pthread_cond_wait(&lk->writecond, &lk->mutex);
		if(st) break;
	}
	lk->w_wait--;
	
	/* Writer becomes active! (only if no error occurred while waiting) */
	if(!st) lk->w_active = 1;
	
	/* Don't unlock mutex here. Prevents readers from getting lock if
	 * there is more than one writer! */
	//st = pthread_mutex_unlock(&lk->mutex);
	return st;
}

int rwlock_rdunlock(rwlock* lk) {
	int st, st1;
	
	/* Ensure lock isn't destroyed yet */
	if(lk->is_dead) return ERROR_INVALID;
	
	/* Acquire lock before making changes to rwlock data */
	st = pthread_mutex_lock(&lk->mutex);
	if(st) return st;
	
	/* Remove an active reader (hopefully this one..) */
	lk->r_active--;
	
	/* If there are waiting writers and no more readers, notify writer */
	if(!lk->r_active && lk->w_wait > 0) {
		st = pthread_cond_signal(&lk->writecond);
	}
	
	/* Release lock because we're all responsible adults here */
	st1 = pthread_mutex_unlock(&lk->mutex);
	
	/* Return error if there was one */
	return st1 ? st1 : st;
}

int rwlock_wrunlock(rwlock* lk) {
	int st;
	
	/* Ensure lock isn't destroyed yet */
	if(lk->is_dead) return ERROR_INVALID;
	
	/* Don't lock, we already have the mutex */
	//st = pthread_mutex_lock(&lk->mutex);
	//if(st) return st;
	
	/* Remove active writer */
	lk->w_active = 0;
	
	/* Wake up all readers if there are any */
	st = pthread_cond_broadcast(&lk->readcond);
	if(st) {
		pthread_mutex_unlock(&lk->mutex);
		return st;
	}
	
	/* Wake up any writers, if there are any */
	st = pthread_cond_signal(&lk->writecond);
	if(st) {
		pthread_mutex_unlock(&lk->mutex);
		return st;
	}
	
	/* Again, responsible adults */
	st = pthread_mutex_unlock(&lk->mutex);
	
	/* Done! */
	return st;
}
