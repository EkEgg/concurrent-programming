/**
 * Activity 5:
 * 
 * Write a concurrent program with M threads that adds 1 to each element of an integer array with
 * N elements. The task must be divided among the M threads in the most balanced way possible. The
 * values of N and M must be provided through the program's arguments.
 * 
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define New(type) ((type*) malloc(sizeof (type)))
#define Min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct
{
    int* begin;
    size_t count;
    int value;
}
AddToRangeArgument;

void* AddToRangeRoutine(void* voidArgument)
{
    AddToRangeArgument* argument = (AddToRangeArgument*) voidArgument;
    for (size_t i = 0; i < argument->count; i++)
    {
        argument->begin[i] += argument->value;
    }
    pthread_exit(voidArgument);
    return voidArgument;
}

int AddToAllElementsThreaded(int* array, size_t arraySize, int value, size_t threadsCount)
{
    threadsCount = Min(arraySize, threadsCount);

    pthread_t threadIDs[threadsCount];

    size_t elementsPerThread = arraySize / threadsCount;
    size_t extraElements = arraySize % threadsCount;

    size_t iArray = 0;
    size_t iThread = 0;
    while (iArray < arraySize)
    {
        size_t elementsForThisThread = elementsPerThread;
        if (extraElements > 0)
        {
            elementsForThisThread++;
            extraElements--;
        }

        AddToRangeArgument* argument = New(AddToRangeArgument);
        if (argument == NULL)
        {
            return -1;
        }
        argument->begin = array + iArray;
        argument->count = elementsForThisThread;
        argument->value = value;
        
        int pthreadCreateCode = pthread_create(&threadIDs[iThread], NULL, AddToRangeRoutine, argument);
        if (pthreadCreateCode != 0)
        {
            return pthreadCreateCode;
        }

        iArray += elementsForThisThread;
        iThread++;
    }

    for (size_t i = 0; i < threadsCount; i++)
    {
        void* argument;
        pthread_join(threadIDs[i], &argument);
        free(argument);
    }

    return 0;
}

void GenerateRandomArrayMod(int* array, size_t arraySize, int mod)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        array[i] = rand() % mod;
    }
}

void CopyArray(int* source, int* destination, size_t arraySize)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        destination[i] = source[i];
    }
}

void PrintArray(FILE* stream, int* array, size_t arraySize)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        if (i > 0) putc(' ', stream);
        fprintf(stream, "%d", array[i]);
    }
}

int main(int argc, char** args)
{
    if (argc != 3)
    {
        fprintf(stderr, "Error: Expected 2 arguments, but %d were given.\n", argc - 1);
        fprintf(stderr, "Usage: %s [ArraySize] [ThreadsCount]\n", args[0]);
        return 1;
    }

    long long arraySize = atoll(args[1]);
    if (arraySize <= 0)
    {
        fprintf(stderr, "Error: [ArraySize] must be a valid positive integer.\n");
        return 2;
    }

    long long threadsCount = atoll(args[2]);
    if (threadsCount <= 0)
    {
        fprintf(stderr, "Error: [ThreadsCount] must be a valid positive integer.\n");
        return 3;
    }

    int array[arraySize];

    srand(time(NULL));
    GenerateRandomArrayMod(array, arraySize, 100);
    printf("Generated array: ");
    PrintArray(stdout, array, arraySize);
    putchar('\n');

    int originalArray[arraySize];
    CopyArray(array, originalArray, arraySize);

    int pthreadReturnCode = AddToAllElementsThreaded(array, arraySize, 1, threadsCount);
    if (pthreadReturnCode != 0)
    {
        fprintf(stderr, "Error: Code %d upon creating a thread. Program is terminating.\n", pthreadReturnCode);
        return 4;
    }

    printf("Array + 1: ");
    PrintArray(stdout, array, arraySize);
    putchar('\n');

    bool procedureWorked = true;
    for (size_t i = 0; i < arraySize; i++)
    {
        if (array[i] != originalArray[i] + 1)
        {
            procedureWorked = false;
            break;
        }
    }

    if (procedureWorked)
    {
        puts("Procedure worked!");
    }
    else
    {
        puts("Procedure did not work!");
    }

    return 0;
}
