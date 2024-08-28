#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double InnerProduct(float* vectorA, float* vectorB, size_t vectorSize)
{
    double result = 0;
    for (size_t i = 0; i < vectorSize; i++)
    {
        result += vectorA[i] * vectorB[i];
    }
    return result;
}

float GenerateRandomFloat(float center, float maximumOffset)
{
    float multiplier = 2.0*((float) rand() / (float) RAND_MAX) - 1.0;
    return center + multiplier * maximumOffset;
}

void GenerateRandomFloatArray(float* array, size_t arraySize, float center, float maximumOffset)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        array[i] = GenerateRandomFloat(center, maximumOffset);
    }
}

void PrintArray(FILE* stream, float* array, size_t arraySize)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        if (i > 0) putc(' ', stream);
        fprintf(stream, "%f", array[i]);
    }
}

int main(int argc, char** args)
{
    if (argc != 3)
    {
        fprintf(stderr, "Error: Expected 2 argument, but %d were given.\n", argc - 1);
        fprintf(stderr, "Usage: %s [FilePath] [VectorSize]\n", args[0]);
        return 1;
    }

    char* filePath = args[1];

    long long vectorSize = atoll(args[2]);
    if (vectorSize <= 0)
    {
        fprintf(stderr, "Error: [VectorSize] must be a valid positive integer.\n");
        return 2;
    }

    FILE* outputStream = fopen(filePath, "wb");
    if (outputStream == NULL)
    {
        fprintf(stderr, "Error: File could not be opened.\n");
        return 3;
    }

    size_t elementsWritten = fwrite((size_t*) &vectorSize, sizeof (size_t), 1, outputStream);
    if (elementsWritten < 1)
    {
        fprintf(stderr, "Error: Vector size could not be written.\n");
        return 4;
    }

    float vectorA[vectorSize];
    float vectorB[vectorSize];

    srand(time(NULL));

    GenerateRandomFloatArray(vectorA, vectorSize, 0.0, 100.0);
    printf("Generated vector A: ");
    PrintArray(stdout, vectorA, vectorSize);
    putchar('\n');

    elementsWritten = fwrite(vectorA, sizeof (float), vectorSize, outputStream);
    if (elementsWritten < vectorSize)
    {
        fprintf(stderr, "Error: Vector A could not be fully written.\n");
        fclose(outputStream);
        return 5;
    }

    GenerateRandomFloatArray(vectorB, vectorSize, 0.0, 100.0);
    printf("Generated vector B: ");
    PrintArray(stdout, vectorB, vectorSize);
    putchar('\n');

    elementsWritten = fwrite(vectorB, sizeof (float), vectorSize, outputStream);
    if (elementsWritten < vectorSize)
    {
        fprintf(stderr, "Error: Vector B could not be fully written.\n");
        fclose(outputStream);
        return 6;
    }

    double innerProduct = InnerProduct(vectorA, vectorB, vectorSize);
    printf("Inner product: %lf\n", innerProduct);

    elementsWritten = fwrite(&innerProduct, sizeof (double), 1, outputStream);
    if (elementsWritten < 1)
    {
        fprintf(stderr, "Error: Inner product could not be written.\n");
        fclose(outputStream);
        return 4;
    }

    fclose(outputStream);

    return 0;
}
