#include <stdio.h>
#include "timer.h"

void MatrixProduct(float* destination, float* matrixA, float* matrixB,
                   size_t matrixARows, size_t matrixAColumns, size_t matrixBColumns)
{
    for (size_t i = 0; i < matrixARows; i++)
    {
        for (size_t j = 0; j < matrixBColumns; j++)
        {
            float sum = 0.0;
            for (size_t k = 0; k < matrixAColumns; k++)
            {
                sum += *(matrixA + i*matrixAColumns + k) * *(matrixB + k*matrixBColumns + j);
            }
            *(destination + i*matrixBColumns + j) = sum;
        }
    }
}

int main(int argc, char** args)
{
    if (argc != 4)
    {
        fprintf(stderr, "Error: Expected 3 argument, but %d were given.\n", argc - 1);
        fprintf(stderr, "Usage: %s [MatrixAFilePath] [MatrixBFilePath] [ProductFilePath]\n", args[0]);
        return 1;
    }

    double readingStartTime;
    GetNow(readingStartTime);

    char* matrixAFilePath = args[1];
    FILE* matrixAStream = fopen(matrixAFilePath, "rb");
    if (matrixAStream == NULL)
    {
        fprintf(stderr, "Error: Matrix A file could not be opened.\n");
        return 2;
    }

    char* matrixBFilePath = args[2];
    FILE* matrixBStream = fopen(matrixBFilePath, "rb");
    if (matrixBStream == NULL)
    {
        fprintf(stderr, "Error: Matrix B file could not be opened.\n");
        fclose(matrixAStream);
        return 3;
    }

    char* productFilePath = args[3];
    FILE* productStream = fopen(productFilePath, "wb");
    if (productStream == NULL)
    {
        fprintf(stderr, "Error: Product file could not be opened.\n");
        fclose(matrixAStream);
        fclose(productStream);
        return 4;
    }

    size_t matrixARows;
    size_t elementsRead = fread(&matrixARows, sizeof (size_t), 1, matrixAStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix A rows could not be read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 5;
    }

    size_t matrixAColumns;
    elementsRead = fread(&matrixAColumns, sizeof (size_t), 1, matrixAStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix A columns could not be read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 6;
    }

    size_t matrixBRows;
    elementsRead = fread(&matrixBRows, sizeof (size_t), 1, matrixBStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix B rows could not be read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 7;
    }

    size_t matrixBColumns;
    elementsRead = fread(&matrixBColumns, sizeof (size_t), 1, matrixBStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix B columns could not be read.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 8;
    }

    if (matrixAColumns != matrixBRows)
    {
        fprintf(stderr, "Error: Matrix A columns and matrix B rows do not match.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 9;
    }

    size_t elementsWritten = fwrite(&matrixARows, sizeof (size_t), 1, productStream);
    if (elementsWritten < 1)
    {
        fprintf(stderr, "Error: Product rows could not be written.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 10;
    }

    elementsWritten = fwrite(&matrixBColumns, sizeof (size_t), 1, productStream);
    if (elementsWritten < 1)
    {
        fprintf(stderr, "Error: Product columns could not be written.\n");
        fclose(matrixAStream);
        fclose(matrixBStream);
        fclose(productStream);
        return 11;
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
        return 12;
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
        return 13;
    }

    fclose(matrixBStream);

    double readingEndTime;
    GetNow(readingEndTime);
    printf("Reading time: %lf\n", readingEndTime - readingStartTime);

    double computingStartTime;
    GetNow(computingStartTime);

    size_t productSize = matrixARows * matrixBColumns;
    float product[productSize];

    MatrixProduct(product, matrixA, matrixB, matrixARows, matrixAColumns, matrixBColumns);

    double computingEndTime;
    GetNow(computingEndTime);
    printf("Computing time: %lf\n", computingEndTime - computingStartTime);

    double writingStartTime;
    GetNow(writingStartTime);

    elementsWritten = fwrite(product, sizeof (float), productSize, productStream);
    if (elementsWritten < productSize)
    {
        fprintf(stderr, "Error: Product could not be fully written.\n");
        fclose(productStream);
        return 14;
    }

    fclose(productStream);

    double writingEndTime;
    GetNow(writingEndTime);
    printf("Writing time: %lf\n", writingEndTime - writingStartTime);

    return 0;
}