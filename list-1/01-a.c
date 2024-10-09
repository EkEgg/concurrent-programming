#include <stddef.h>
#include <math.h>

double pi(size_t n)
{
    double result = 0;
    for (size_t k = 0; k < n; k++)
    {
        double element = 0;
        element += 4 / (8*k + 1);
        element -= 2 / (8*k + 4);
        element -= 1 / (8*k + 5);
        element -= 1 / (8*k + 6);
        element *= 1 / pow(16, k);
        result += element;
    }
    return result;
}
