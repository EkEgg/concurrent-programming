#include "RWLock.h"

int InitializeRWLock(RWLock* lock)
{
    int returnCode = 0;

    if (pthread_mutex_init(&lock->mutex, NULL) != 0)
    {
        returnCode |= 1;
    }

    lock->nReaders = 0;
    lock->nWriters = 0;

    if (pthread_cond_init(&lock->readersQueue, NULL) != 0)
    {
        returnCode |= 2;
    }

    if (pthread_cond_init(&lock->writersQueue, NULL) != 0)
    {
        returnCode |= 4;
    }
    
    return 0;
}

int DestroyRWLock(RWLock* lock)
{
    int returnCode = 0;

    if (pthread_mutex_destroy(&lock->mutex) != 0)
    {
        returnCode |= 1;
    }

    if (pthread_cond_destroy(&lock->readersQueue) != 0)
    {
        returnCode |= 2;
    }

    if (pthread_cond_destroy(&lock->writersQueue) != 0)
    {
        returnCode |= 4;
    }

    return returnCode;
}

void LockForRead(RWLock* lock)
{
    pthread_mutex_lock(&lock->mutex);

    while (lock->nWriters > 0)
    {
        pthread_cond_wait(&lock->readersQueue, &lock->mutex);
    }

    lock->nReaders++;

    pthread_mutex_unlock(&lock->mutex);
}

void UnlockForRead(RWLock *lock)
{
    pthread_mutex_lock(&lock->mutex);

    lock->nReaders--;

    if (lock->nReaders == 0)
    {
        pthread_cond_signal(&lock->writersQueue);
    }

    pthread_mutex_unlock(&lock->mutex);
}

void LockForWrite(RWLock* lock)
{
    pthread_mutex_lock(&lock->mutex);

    while (lock->nReaders > 0 || lock->nWriters > 0)
    {
        pthread_cond_wait(&lock->writersQueue, &lock->mutex);
    }

    lock->nWriters++;

    pthread_mutex_unlock(&lock->mutex);
}

void UnlockForWrite(RWLock* lock)
{
    pthread_mutex_lock(&lock->mutex);

    lock->nWriters--;

    if (lock->nWriters == 0)
    {
        pthread_cond_signal(&lock->writersQueue);
        pthread_cond_broadcast(&lock->readersQueue);
    }

    pthread_mutex_unlock(&lock->mutex);
}
