#include <stdio.h>
#include <float.h>
#define POW3(x) (((x) * (x) * (x)) >= DBL_MAX ? DBL_MAX : ((x) * (x) * (x)))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define APPROX_EQUAL(a, b) (ABS((a) - (b)) <= DBL_EPSILON * ABS((a) + (b)))
double root3(double x)
{
    double max = DBL_MAX;
    double min = 0;
    int negative = x < 0;
    double absX = ABS(x);
    while (1)
    {
        double curr = (max - min) / 2 + min;
        if (APPROX_EQUAL(POW3(curr), absX))
            return negative ? -curr : curr;
        if (POW3(curr) > absX)
            max = curr;
        else
            min = curr;
    }
}
// Обяснение :
// • Използва се бинарно търсене върху числов интервал.
// • Работи се с абсолютна стойност.
// • DBL_EPSILON дефинира точността.
// • Сложност O(log n).