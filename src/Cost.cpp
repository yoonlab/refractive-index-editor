#include "Cost.h"

Cost::Cost()
{
}

double Cost::cost() const
{
    double sum = 0;
    for (Lightpath *path : paths)
    {
        path->solve2();
        sum += path->distToTarget();
    }
    return sum;
}
