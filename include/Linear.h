#pragma once

#include "Medium.h"
#include <vector>

class Linear : public Medium
{
public:
    Linear(glm::vec3 _direction, double _slope, double _init);
    virtual ~Linear();

    double f(glm::dvec3 p);
    glm::dvec3 gradient(glm::dvec3 p);
    std::vector<double *> *arguments();
    std::vector<double> *arg_max();
    std::vector<double> *arg_min();

protected:
    glm::dvec3 direction;
    double slope;
    double init;

    std::vector<double *> args;
    std::vector<double> _arg_max;
    std::vector<double> _arg_min;
};