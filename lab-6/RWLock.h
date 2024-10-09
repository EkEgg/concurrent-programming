#pragma once

#include <pthread.h>

typedef struct
{
    pthread_mutex_t mutex;
    size_t nReaders;
    size_t nWriters;
    pthread_cond_t readersQueue;
    pthread_cond_t writersQueue;
}
RWLock;

int InitializeRWLock(RWLock* lock);

int DestroyRWLock(RWLock* lock);

void LockForRead(RWLock* lock);

void UnlockForRead(RWLock* lock);

void LockForWrite(RWLock* lock);

void UnlockForWrite(RWLock* lock);
