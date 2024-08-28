/**
 * Activity 3:
 * 
 * Design, implement and assess a concurrent solution to the problem of computing the inner
 * product between two real-number vectors.
 * 
 * Let (a_1, ..., a_N) and (b_1, ..., b_N) be vectors in a N-dimensional space expressed in terms
 * of an orthogonal system of cartesian coordinates. The inner product between these two vectors
 * is a real value given by the equation:
 * 
 *   a_1 * b_1 + ... + a_N * b_N
 * 
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define Min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct
{
    float* beginA;
    float* beginB;
    size_t count;
    double* output;
}
InnerProductArgument;

void* InnerProductRoutine(void* voidArgument)
{
    InnerProductArgument* argument = (InnerProductArgument*) voidArgument;
    double result = 0;
    for (size_t i = 0; i < argument->count; i++)
    {
        result += argument->beginA[i] * argument->beginB[i];
    }
    *argument->output = result;
    pthread_exit(argument->output);
    return argument->output;
}

int InnerProductThreaded(float* vectorA, float* vectorB, size_t vectorSize, size_t threadsCount, double* output)
{
    threadsCount = Min(vectorSize, threadsCount);

    pthread_t threadIDs[threadsCount];
    InnerProductArgument arguments[threadsCount];
    double results[threadsCount];

    size_t elementsPerThread = vectorSize / threadsCount;
    size_t extraElements = vectorSize % threadsCount;

    size_t iArray = 0;
    size_t iThread = 0;
    while (iArray < vectorSize)
    {
        size_t elementsForThisThread = elementsPerThread;
        if (extraElements > 0)
        {
            elementsForThisThread++;
            extraElements--;
        }

        arguments[iThread].beginA = vectorA + iArray;
        arguments[iThread].beginB = vectorB + iArray;
        arguments[iThread].count = elementsForThisThread;
        arguments[iThread].output = &results[iThread];
        
        int pthreadCreateCode = pthread_create(&threadIDs[iThread], NULL, InnerProductRoutine, &arguments[iThread]);
        if (pthreadCreateCode != 0)
        {
            return pthreadCreateCode;
        }

        iArray += elementsForThisThread;
        iThread++;
    }

    double result = 0;
    for (size_t i = 0; i < threadsCount; i++)
    {
        void* argument;
        pthread_join(threadIDs[i], &argument);
        result += *((double*) argument);
    }
    *output = result;

    return 0;
}

double RelativeVariation(double reference, double other)
{
    return fabs((reference - other) / reference);
}

int main(int argc, char** args)
{
    if (argc != 3)
    {
        fprintf(stderr, "Error: Expected 2 arguments, but %d were given.\n", argc - 1);
        fprintf(stderr, "Usage: %s [FilePath] [ThreadsCount]\n", args[0]);
        return 1;
    }

    char* filePath = args[1];

    long long threadsCount = atoll(args[2]);
    if (threadsCount <= 0)
    {
        fprintf(stderr, "Error: [ThreadsCount] must be a valid positive integer.\n");
        return 2;
    }

    FILE* inputStream = fopen(filePath, "rb");
    if (inputStream == NULL)
    {
        fprintf(stderr, "Error: File could not be opened.\n");
        return 3;
    }

    size_t vectorSize;
    size_t elementsRead = fread(&vectorSize, sizeof (size_t), 1, inputStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Vector size could not be read.\n");
        fclose(inputStream);
        return 3;
    }

    float vectorA[vectorSize];
    elementsRead = fread(vectorA, sizeof (float), vectorSize, inputStream);
    if (elementsRead < vectorSize)
    {
        fprintf(stderr, "Error: Vector A could not be fully read.\n");
        fclose(inputStream);
        return 4;
    }

    float vectorB[vectorSize];
    elementsRead = fread(vectorB, sizeof (float), vectorSize, inputStream);
    if (elementsRead < vectorSize)
    {
        fprintf(stderr, "Error: Vector B could not be fully read.\n");
        fclose(inputStream);
        return 5;
    }

    double originalInnerProduct;
    elementsRead = fread(&originalInnerProduct, sizeof (double), 1, inputStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Original inner product could not be read.\n");
        fclose(inputStream);
        return 6;
    }

    double threadedInnerProduct;
    int pthreadReturnCode = InnerProductThreaded(vectorA, vectorB, vectorSize, threadsCount, &threadedInnerProduct);
    if (pthreadReturnCode != 0)
    {
        fprintf(stderr, "Error: Code %d upon creating a thread.\n", pthreadReturnCode);
        fclose(inputStream);
        return 7;
    }

    double relativeVariation = RelativeVariation(originalInnerProduct, threadedInnerProduct);

    printf("Original inner product: %.32lf\n", originalInnerProduct);
    printf("Threaded inner product: %.32lf\n", threadedInnerProduct);
    printf("Relative variation: %.32lf\n", relativeVariation);

    fclose(inputStream);

    return 0;
}
