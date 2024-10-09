#include <stdio.h>
#include <pthread.h>

typedef struct
{
    enum { UPDATER, RECEIVER } turn;
    pthread_mutex_t mutex;
    pthread_cond_t queue;
}
ConversationArgument;

void initializeConversationArgument(ConversationArgument* argument)
{
    pthread_mutex_init(&argument->mutex, NULL);
    pthread_cond_init(&argument->queue, NULL);
    argument->turn = UPDATER;
}

void* updaterRoutine(void* voidArgument)
{
    ConversationArgument* argument = (ConversationArgument*) voidArgument;
    pthread_mutex_lock(&argument->mutex);

    puts("A: Hello! Are you accessing the variable now?");
    argument->turn = RECEIVER;
    pthread_cond_signal(&argument->queue);
    
    if (argument->turn != UPDATER)
    {
        pthread_cond_wait(&argument->queue, &argument->mutex);
    }

    puts("A: Right. I'll update it, then, okay?");
    argument->turn = RECEIVER;
    pthread_cond_signal(&argument->queue);

    if (argument->turn != UPDATER)
    {
        pthread_cond_wait(&argument->queue, &argument->mutex);
    }

    puts("A: Just finished updating it.");
    argument->turn = RECEIVER;
    pthread_cond_signal(&argument->queue);

    pthread_mutex_unlock(&argument->mutex);
    pthread_exit(NULL);
}

void* receiverRoutine(void* voidArgument)
{
    ConversationArgument* argument = (ConversationArgument*) voidArgument;
    pthread_mutex_lock(&argument->mutex);

    if (argument->turn != RECEIVER)
    {
        pthread_cond_wait(&argument->queue, &argument->mutex);
    }

    puts("B: Hi! No, I'm not.");
    argument->turn = UPDATER;
    pthread_cond_signal(&argument->queue);

    if (argument->turn != RECEIVER)
    {
        pthread_cond_wait(&argument->queue, &argument->mutex);
    }

    puts("B: Okay.");
    argument->turn = UPDATER;
    pthread_cond_signal(&argument->queue);

    if (argument->turn != RECEIVER)
    {
        pthread_cond_wait(&argument->queue, &argument->mutex);
    }

    puts("B: That's perfect! Got it.");

    pthread_mutex_unlock(&argument->mutex);   
    pthread_exit(NULL);
}

int main(void)
{
    ConversationArgument conversationArgument;
    pthread_t updaterID, receiverID;

    initializeConversationArgument(&conversationArgument);

    int pthreadError = pthread_create(&updaterID, NULL, updaterRoutine, &conversationArgument);
    if (pthreadError)
    {
        puts("Error: Error upon creating updater thread.");
        return 1;
    }

    pthreadError = pthread_create(&receiverID, NULL, receiverRoutine, &conversationArgument);
    if (pthreadError)
    {
        puts("Error: Error upon creating receiver thread.");
        return 2;
    }

    pthread_join(updaterID, NULL);
    pthread_join(receiverID, NULL);

    return 0;
}
