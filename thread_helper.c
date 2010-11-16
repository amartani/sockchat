#include <pthread.h>

void rlock(pthread_rwlock_t *lock)
{
    int ret;

#ifdef DEBUG_THREADS
    puts("R-Lock");
    fflush(stdout);
#endif

    ret = pthread_rwlock_rdlock(lock);
    if (ret) {
        error("ERROR on rlock");
    }

#ifdef DEBUG_THREADS_OK
    puts("OK");
    fflush(stdout);
#endif

}

void wlock(pthread_rwlock_t *lock)
{
    int ret;

#ifdef DEBUG_THREADS
    puts("W-Lock");
    fflush(stdout);
#endif

    ret = pthread_rwlock_wrlock(lock);
    if (ret) {
        error("ERROR on wlock");
    }

#ifdef DEBUG_THREADS_OK
    puts("OK");
    fflush(stdout);
#endif

}

void unlock(pthread_rwlock_t *lock)
{
    int ret;

#ifdef DEBUG_THREADS
    puts("Unlocked");
    fflush(stdout);
#endif

    ret = pthread_rwlock_unlock(lock);
    if (ret) {
        error("ERROR on unlock");
    }

#ifdef DEBUG_THREADS_OK
    puts("OK");
    fflush(stdout);
#endif

}

pthread_rwlock_t *new_lock()
{
    pthread_rwlock_t *lock;
    int ret;

    lock = (pthread_rwlock_t*) malloc(sizeof(pthread_rwlock_t));
    ret = pthread_rwlock_init(lock, NULL);
    if (ret) {
        error("ERROR on new_lock");
    }

#ifdef DEBUG_THREADS
    puts("New Lock");
    fflush(stdout);
#endif

    return lock;
}

