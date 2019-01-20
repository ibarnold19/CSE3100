#ifndef __RWLOCK_H__
#define __RWLOCK_H__

#include <pthread.h>

#define ERROR_INVALID -1
#define ERROR_BUSY    -2

typedef struct {
	pthread_mutex_t    mutex;  /* for controlling access to all the other data */
	pthread_cond_t  readcond;  /* condition variable to wait for read */
	pthread_cond_t writecond;  /* condition variable to wait for write */
	int             r_active;  /* # readers active */
	int             w_active;  /* is writer active? */
	int               r_wait;  /* # readers waiting */
	int               w_wait;  /* # writers waiting */
	int              is_dead;  /* 0 if alive, 1 if destroyed */
} rwlock;

/* Constructor and destructor */
int rwlock_init(rwlock* lk);
int rwlock_destroy(rwlock* lk);

/* Lock and unlock functions */
int rwlock_rdlock(rwlock* lk);
int rwlock_wrlock(rwlock* lk);
int rwlock_rdunlock(rwlock* lk);
int rwlock_wrunlock(rwlock* lk);

#endif
