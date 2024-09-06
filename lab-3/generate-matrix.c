#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float GenerateRandomFloat(float min, float max)
{
    float multiplier = (float) rand() / (float) RAND_MAX;
    return min + multiplier * (max - min);
}

void GenerateRandomFloatArray(float* array, size_t arraySize, float min, float max)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        array[i] = GenerateRandomFloat(min, max);
    }
}

void GenerateRandomFloatMatrix(float* matrix, size_t rows, size_t columns, float min, float max)
{
    GenerateRandomFloatArray(matrix, rows * columns, min, max);
}

int main(int argc, char** args)
{
    if (argc != 4)
    {
        fprintf(stderr, "Error: Expected 3 argument, but %d were given.\n", argc - 1);
        fprintf(stderr, "Usage: %s [FilePath] [MatrixRows] [MatrixColumns]\n", args[0]);
        return 1;
    }

    char* filePath = args[1];

    long long matrixRows = atoll(args[2]);
    if (matrixRows <= 0)
    {
        fprintf(stderr, "Error: [MatrixRows] must be a valid positive integer.\n");
        return 2;
    }

    long long matrixColumns = atoll(args[3]);
    if (matrixColumns <= 0)
    {
        fprintf(stderr, "Error: [MatrixColumns] must be a valid positive integer.\n");
        return 3;
    }

    FILE* outputStream = fopen(filePath, "wb");
    if (outputStream == NULL)
    {
        fprintf(stderr, "Error: File could not be opened.\n");
        return 4;
    }

    size_t elementsWritten = fwrite((size_t*) &matrixRows, sizeof (size_t), 1, outputStream);
    if (elementsWritten < 1)
    {
        fprintf(stderr, "Error: Matrix rows could not be written.\n");
        return 5;
    }

    elementsWritten = fwrite((size_t*) &matrixColumns, sizeof (size_t), 1, outputStream);
    if (elementsWritten < 1)
    {
        fprintf(stderr, "Error: Matrix columns could not be written.\n");
        return 6;
    }

    size_t matrixSize = matrixRows * matrixColumns;
    float matrix[matrixSize];

    srand(time(NULL));

    GenerateRandomFloatMatrix(matrix, matrixRows, matrixColumns, -100.0, 100.0);

    elementsWritten = fwrite(matrix, sizeof (float), matrixSize, outputStream);
    if (elementsWritten < matrixSize)
    {
        fprintf(stderr, "Error: Matrix could not be fully written.\n");
        fclose(outputStream);
        return 7;
    }

    fclose(outputStream);

    return 0;
}