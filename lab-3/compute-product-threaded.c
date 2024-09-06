#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define Min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct
{
    float* matrixA;
    float* matrixB;
    size_t matrixAColumns;
    size_t matrixBColumns;
    size_t rowIndex;
    size_t rowsCount;
    float* destination;
}
MatrixProductArgument;

void* MatrixProductRoutine(void* voidArgument)
{
    MatrixProductArgument* argument = (MatrixProductArgument*) voidArgument;
    float* matrixA = argument->matrixA;
    float* matrixB = argument->matrixB;
    size_t matrixAColumns = argument->matrixAColumns;
    size_t matrixBColumns = argument->matrixBColumns;
    size_t rowIndex = argument->rowIndex;
    size_t rowsCount = argument->rowsCount;
    float* output = argument->destination;
    for (size_t i = rowIndex; i < rowIndex + rowsCount; i++)
    {
        for (size_t j = 0; j < argument->matrixBColumns; j++)
        {
            *(output + i*matrixBColumns + j) = 0;
            for (size_t k = 0; k < matrixAColumns; k++)
            {
                *(output + i*matrixBColumns + j)
                    += *(matrixA + i*matrixBColumns + k)
                    * *(matrixB + k*matrixAColumns + j);
            }
        }
    }
    pthread_exit(voidArgument);
    return voidArgument;
}

int MatrixProductThreaded(float *destination, float* matrixA, float* matrixB,
                          size_t matrixARows, size_t matrixAColumns, size_t matrixBColumns,
                          size_t threadsCount)
{
    threadsCount = Min(matrixARows, threadsCount);

    pthread_t threadIDs[threadsCount];
    MatrixProductArgument arguments[threadsCount];
    double results[threadsCount];

    size_t rowsPerThread = matrixARows / threadsCount;
    size_t extraRows = matrixARows % threadsCount;

    size_t iMatrixA = 0;
    size_t iThread = 0;
    while (iMatrixA < matrixARows)
    {
        size_t rowsForThisThread = rowsPerThread;
        if (extraRows > 0)
        {
            rowsForThisThread++;
            extraRows--;
        }

        arguments[iThread].matrixA = matrixA;
        arguments[iThread].matrixB = matrixB;
        arguments[iThread].matrixAColumns = matrixAColumns;
        arguments[iThread].matrixBColumns = matrixBColumns;
        arguments[iThread].rowIndex = iMatrixA;
        arguments[iThread].rowsCount = rowsForThisThread;
        arguments[iThread].destination = destination;
        
        int pthreadCreateCode = pthread_create(
            &threadIDs[iThread],
            NULL,
            MatrixProductRoutine,
            &arguments[iThread]
        );
        if (pthreadCreateCode != 0)
        {
            return pthreadCreateCode;
        }

        iMatrixA += rowsForThisThread;
        iThread++;
    }

    for (size_t i = 0; i < threadsCount; i++)
    {
        pthread_join(threadIDs[i], NULL);
    }

    return 0;
}

int main(int argc, char** args)
{
    if (argc != 5)
    {
        fprintf(stderr, "Error: Expected 4 argument, but %d were given.\n", argc - 1);
        fprintf(stderr, "Usage: %s [MatrixAFilePath] [MatrixBFilePath] "
                                  "[ProductFilePath] [ThreadsCount]\n", args[0]);
        return 1;
    }

    double readingStartTime;
    GetNow(readingStartTime);

    long long threadsCount = atoll(args[4]);
    if (threadsCount <= 0)
    {
        fprintf(stderr, "Error: [ThreadsCount] must be a valid positive integer.\n");
        return 2;
    }

    char* matrixAFilePath = args[1];
    FILE* matrixAStream = fopen(matrixAFilePath, "rb");
    if (matrixAStream == NULL)
    {
        fprintf(stderr, "Error: Matrix A file could not be opened.\n");
        return 3;
    }

    char* matrixBFilePath = args[2];
    FILE* matrixBStream = fopen(matrixBFilePath, "rb");
    if (matrixBStream == NULL)
    {
        fprintf(stderr, "Error: Matrix B file could not be opened.\n");
        fclose(matrixAStream);
        return 4;
    }

    char* productFilePath = args[3];
    FILE* productStream = fopen(productFilePath, "wb");
    if (productStream == NULL)
    {
        fprintf(stderr, "Error: Product file could not be opened.\n");
        fclose(matrixAStream);
        fclose(productStream);
        return 5;
    }

    size_t matrixARows;
    size_t elementsRead = fread(&matrixARows, sizeof (size_t), 1, matrixAStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix A rows could not be read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 6;
    }

    size_t matrixAColumns;
    elementsRead = fread(&matrixAColumns, sizeof (size_t), 1, matrixAStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix A columns could not be read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 7;
    }

    size_t matrixBRows;
    elementsRead = fread(&matrixBRows, sizeof (size_t), 1, matrixBStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix B rows could not be read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 8;
    }

    size_t matrixBColumns;
    elementsRead = fread(&matrixBColumns, sizeof (size_t), 1, matrixBStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix B columns could not be read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 9;
    }

    if (matrixAColumns != matrixBRows)
    {
        fprintf(stderr, "Error: Matrix A columns and matrix B rows do not match.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 10;
    }

    size_t elementsWritten = fwrite(&matrixARows, sizeof (size_t), 1, productStream);
    if (elementsWritten < 1)
    {
        fprintf(stderr, "Error: Product rows could not be written.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 11;
    }

    elementsWritten = fwrite(&matrixBColumns, sizeof (size_t), 1, productStream);
    if (elementsWritten < 1)
    {
        fprintf(stderr, "Error: Product columns could not be written.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 12;
    }

    size_t matrixASize = matrixARows * matrixAColumns;
    float matrixA[matrixASize];

    elementsRead = fread(matrixA, sizeof (float), matrixASize, matrixAStream);
    if (elementsRead < matrixASize)
    {
        fprintf(stderr, "Error: Matrix A could not be fully read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 13;
    }

    fclose(matrixAStream);

    size_t matrixBSize = matrixBRows * matrixBColumns;
    float matrixB[matrixBSize];

    elementsRead = fread(matrixB, sizeof (float), matrixBSize, matrixBStream);
    if (elementsRead < matrixBSize)
    {
        fprintf(stderr, "Error: Matrix B could not be fully read.\n");
        fclose(matrixBStream);
        fclose(productStream);
        return 14;
    }

    fclose(matrixBStream);

    double readingEndTime;
    GetNow(readingEndTime);
    printf("Reading time: %lf\n", readingEndTime - readingStartTime);

    double computingStartTime;
    GetNow(computingStartTime);

    size_t productSize = matrixARows * matrixBColumns;
    float product[productSize];

    int pthreadReturnCode = MatrixProductThreaded(product, matrixA, matrixB,
                                                  matrixARows, matrixAColumns, matrixBColumns,
                                                  threadsCount);
    if (pthreadReturnCode != 0)
    {
        fprintf(stderr, "Error: Code %d upon creating a thread.\n", pthreadReturnCode);
        fclose(productStream);
        return 15;
    }

    double writingStartTime;
    GetNow(writingStartTime);

    double computingEndTime;
    GetNow(computingEndTime);
    printf("Computing time: %lf\n", computingEndTime - computingStartTime);

    elementsWritten = fwrite(product, sizeof (float), productSize, productStream);
    if (elementsWritten < productSize)
    {
        fprintf(stderr, "Error: Product could not be fully written.\n");
        fclose(productStream);
        return 16;
    }

    fclose(productStream);

    double writingEndTime;
    GetNow(writingEndTime);
    printf("Writing time: %lf\n", writingEndTime - writingStartTime);

    return 0;
}