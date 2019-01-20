#include <pthread.h>
#include "error.h"
#include "rwlock.h"

int read_turn = 0;

void rCleanup(void *arg){

	rwlock *lock = (rwlock *)arg;

	lock->r_wait--;
	pthread_mutex_unlock(&lock->mutex);

}

int rwlock_rdlock(rwlock* lk){

	int status;

	if(lk->is_dead != 0){

		return 0;

	}

	status = pthread_mutex_lock(&lk->mutex);
	if(status != 0){

		return status;

	}

	if(lk->w_active || lk->r_active){

		lk->r_wait++;
		pthread_cleanup_push(rCleanup, (void*)lk);
		
		while(lk->w_active || (lk->w_wait > 0 && read_turn == 0)){
		
			status = pthread_cond_wait(&lk->readcond, &lk->mutex);
			
			if(status != 0){

				break;			

			}

		}

		pthread_cleanup_pop(0);
		lk->r_wait--;

	}

	if(status == 0){

		lk->r_active++;

	}

	pthread_mutex_unlock(&lk->mutex);

	return status;

}

void wCleanup(void *arg){

	rwlock *lock = (rwlock *)arg;

	lock->w_wait--;
	pthread_mutex_unlock(&lock->mutex);

}

int rwlock_wrlock(rwlock* lk) {
	
	int status;

	if(lk->is_dead != 0){

		return 0;

	}

	status = pthread_mutex_lock(&lk->mutex);
	if(status != 0){

		return status;

	}

	if(lk->w_active || lk->r_active > 0){

		lk->w_wait++;
		pthread_cleanup_push(wCleanup, (void*)lk);

		while(lk->w_active || lk->r_active > 0){

			status = pthread_cond_wait(&lk->writecond, &lk->mutex);
			if(status != 0){

				break;

			}

		}

		pthread_cleanup_pop(0);
		lk->w_wait--;

	}

	if(status == 0){

		lk->w_active = 1;

	}

	pthread_mutex_unlock(&lk->mutex);
	return status;
}

int rwlock_rdunlock(rwlock* lk) {
	
	int status, check;

	if(lk->is_dead != 0){

		return 1;

	}

	status = pthread_mutex_lock(&lk->mutex);
	if(status != 0){

		return status;

	}

	lk->r_active--;
	if(lk->r_active == 0){

		status = pthread_cond_signal(&lk->writecond);

	}

	check = pthread_mutex_unlock(&lk->mutex);
	read_turn = 0;

	if(check == 0){

		return status;

	} else {

		return check;

	}

}

int rwlock_wrunlock(rwlock* lk) {
	
	int status;

	if(lk->is_dead != 0){

		return 1;

	}

	status = pthread_mutex_lock(&lk->mutex);
	if(status != 0){

		return status;

	}

	lk->w_active = 0;
	if(lk->r_wait > 0){

		status = pthread_cond_broadcast(&lk->readcond);
		if(status != 0){

			pthread_mutex_unlock(&lk->mutex);
			read_turn = 1;
			return status;

		}

	} else if(lk->w_wait > 0){

		status = pthread_cond_signal(&lk->writecond);
		if(status != 0){

			pthread_mutex_unlock(&lk->mutex);
			read_turn = 1;
			return status;

		}

	}

	read_turn = 1;
	status = pthread_mutex_unlock(&lk->mutex);
	return status;
}

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
