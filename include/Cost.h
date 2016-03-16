#pragma once
#include <vector>
#include "Lightpath.h"

class Cost
{
public:
    std::vector<Lightpath *> paths;

    Cost();
    double cost() const;
};