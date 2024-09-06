#include <stdio.h>

void PrintArray(FILE* stream, float* array, size_t arraySize)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        if (i > 0) putc(' ', stream);
        fprintf(stream, "%f", array[i]);
    }
}

void PrintMatrix(FILE* stream, float* matrix, size_t rows, size_t columns)
{
    for (size_t i = 0; i < rows; i++)
    {
        PrintArray(stream, matrix + i*columns, columns);
        putc('\n', stream);
    }
}

int main(int argc, char** args)
{
    if (argc != 2)
    {
        fprintf(stderr, "Error: Expected 1 argument, but %d were given.\n", argc - 1);
        fprintf(stderr, "Usage: %s [MatrixFilePath]\n", args[0]);
        return 1;
    }

    char* matrixFilePath = args[1];
    FILE* matrixStream = fopen(matrixFilePath, "rb");
    if (matrixStream == NULL)
    {
        fprintf(stderr, "Error: Matrix file could not be opened.\n");
        return 2;
    }

    size_t matrixRows;
    size_t elementsRead = fread(&matrixRows, sizeof (size_t), 1, matrixStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix rows could not be read.\n");
        fclose(matrixStream);
        return 3;
    }

    size_t matrixColumns;
    elementsRead = fread(&matrixColumns, sizeof (size_t), 1, matrixStream);
    if (elementsRead < 1)
    {
        fprintf(stderr, "Error: Matrix columns could not be read.\n");
        fclose(matrixStream);
        return 4;
    }

    size_t matrixSize = matrixRows * matrixColumns;
    float matrix[matrixSize];

    elementsRead = fread(matrix, sizeof (float), matrixSize, matrixStream);
    if (elementsRead < matrixSize)
    {
        fprintf(stderr, "Error: Matrix could not be fully read.\n");
        fclose(matrixStream);
        return 5;
    }

    fclose(matrixStream);

    PrintMatrix(stdout, matrix, matrixRows, matrixColumns);

    return 0;
}