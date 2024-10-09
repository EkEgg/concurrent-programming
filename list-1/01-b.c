#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define min(a, b) (((a) <= (b)) ? (a) : (b))

typedef struct
{
    size_t begin;
    size_t elements;
    double* output;
}
PiArgument;

void* piRoutine(void* voidArgument)
{
    PiArgument* argument = (PiArgument*) voidArgument;
    size_t begin = argument->begin;
    size_t elements = argument->elements;
    double* output = argument->output;

    double result = 0;
    for (size_t k = begin; k < begin + elements; k++)
    {
        double element = 0;
        element += 4 / (8*k + 1);
        element -= 2 / (8*k + 4);
        element -= 1 / (8*k + 5);
        element -= 1 / (8*k + 6);
        element *= 1 / pow(16, k);
        result += element;
    }
    *output = result;

    pthread_exit(NULL);
}

double piThreaded(size_t n, size_t m)
{
    m = min(n, m);
    pthread_t threadIDs[m];
    PiArgument arguments[m];
    double partialResults[m];

    size_t elementsPerThread = n / m;
    size_t remainderElements = n % m;

    size_t begin = 0;
    for (size_t i = 0; i < m; i++)
    {
        size_t elements = elementsPerThread;
        if (i < remainderElements)
        {
            elements++;
        }
        
        arguments[i].begin = begin;
        arguments[i].elements = elements;
        arguments[i].output = &partialResults[i];
        int pthreadError = pthread_create(&threadIDs[i], NULL, piRoutine, (void*) &arguments[i]);

        if (pthreadError)
        {
            printf("Error: Error upon creating thread %zu.\n", i);
            exit(1);
        }

        begin += elements;
    }

    double result = 0;
    for (size_t i = 0; i < m; i++)
    {
        pthread_join(threadIDs[i], NULL);
        result += partialResults[i];
    }
    
    return result;
}
