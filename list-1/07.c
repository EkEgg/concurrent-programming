#include <stddef.h>
#include <pthread.h>

typedef struct
{
    void** array;
    size_t capacity;
    size_t size;
    size_t producerI;
    size_t consumerI;
    pthread_mutex_t mutex;
    pthread_cond_t producerQueue;
    pthread_cond_t consumerQueue;
}
ThreadSafeCollection;

void insert(ThreadSafeCollection* collection, void* element)
{
    pthread_mutex_lock(&collection->mutex);

    while (collection->size == collection->capacity)
    {
        pthread_cond_wait(&collection->producerQueue, &collection->mutex);
    }

    collection->array[collection->producerI] = element;
    collection->producerI = (collection->producerI + 1) % collection->capacity;
    collection->size++;

    pthread_mutex_unlock(&collection->mutex);

    pthread_cond_signal(&collection->consumerQueue);
}

void* remove(ThreadSafeCollection* collection)
{
    pthread_mutex_lock(&collection->mutex);

    while (collection->size == 0)
    {
        pthread_cond_wait(&collection->consumerQueue, &collection->mutex);
    }

    void* element = collection->array[collection->consumerI];
    collection->consumerI = (collection->consumerI + 1) % collection->capacity;
    collection->size--;

    pthread_mutex_unlock(&collection->mutex);

    pthread_cond_signal(&collection->producerQueue);
}
