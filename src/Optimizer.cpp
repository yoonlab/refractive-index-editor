#include "Optimizer.h"

#include <vector>
#include <iostream>

void Optimizer::optimize(Cost * cost)
{
    Medium *pMedium = cost->paths.at(0)->getMediumPtr();
    std::vector<double *> *args = pMedium->arguments();
    std::vector<double> *arg_max = pMedium->arg_max();
    std::vector<double> *arg_min = pMedium->arg_min();
    int size = args->size();
    colVec initVals(size);
    int argCount = 0;
    for (const auto &argPtr : *args)
    {
        initVals(argCount) = *argPtr;
        argCount++;
    }

    Cost_Dlib *cost_dlib = (Cost_Dlib *)cost;

    std::cout << initVals << std::endl;
    try
    {
        dlib::find_min_bobyqa
            (
                *cost_dlib,
                initVals,
                size * 2 + 1,
                dlib::uniform_matrix<double>(size, 1, -1e100),
                dlib::uniform_matrix<double>(size, 1, 1e100),
                100,
                1e-6,
                iterLimit
                );
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    std::cout << initVals << std::endl;
}

double Cost_Dlib::operator()(const colVec & arg) const
{
    std::vector<double *> *args = paths.at(0)->getMediumPtr()->arguments();
    int argCount = 0;
    for (const auto &argPtr : *args)
    {
        *argPtr = arg(argCount);
        argCount++;
    }
    return cost();
}
