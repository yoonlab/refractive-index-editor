#include "Linear.h"
#include <glm/glm.hpp>

Linear::Linear(glm::vec3 _direction, double _slope, double _init)
    : direction(_direction), slope(_slope), init(_init)
{
    args.push_back(&direction.x);
    _arg_min.push_back(-1e100);
    _arg_max.push_back(1e100);
    args.push_back(&direction.y);
    _arg_min.push_back(-1e100);
    _arg_max.push_back(1e100);
    args.push_back(&direction.z);
    _arg_min.push_back(-1e100);
    _arg_max.push_back(1e100);
    args.push_back(&slope);
    _arg_min.push_back(-1e100);
    _arg_max.push_back(1e100);
    args.push_back(&init);
    _arg_min.push_back(-1e100);
    _arg_max.push_back(1e100);
}

Linear::~Linear()
{
}

double Linear::f(glm::dvec3 p)
{
    return init + slope * glm::dot(p, direction);
}

glm::dvec3 Linear::gradient(glm::dvec3 p)
{
    return slope * direction;
}

std::vector<double *> *Linear::arguments()
{
    return &args;
}

std::vector<double> *Linear::arg_max()
{
    return &_arg_max;
}

std::vector<double> *Linear::arg_min()
{
    return &_arg_min;
}