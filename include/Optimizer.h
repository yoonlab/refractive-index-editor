#pragma once
#include <dlib/optimization.h>
#include "Cost.h"

typedef dlib::matrix<double, 0, 1> colVec;

static class Optimizer
{
public:
    static void optimize(Cost *cost);
protected:
    static const int iterLimit = 1000;
};

class Cost_Dlib : Cost
{
public:
    double operator() (const colVec& arg) const;
};