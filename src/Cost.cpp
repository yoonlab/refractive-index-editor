#include "Cost.h"

Cost::Cost()
{
}

double Cost::cost() const
{
    double sum = 0;
    for (const auto &path : paths)
    {
        path->solve2();
        sum += pow(path->distToTarget(), 1);
    }
    return sum;
}
