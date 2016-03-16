#include "Linear.h"
#include <glm/glm.hpp>

Linear::Linear(glm::vec3 _direction, double _slope, double _init)
    : direction(_direction), slope(_slope), init(_init)
{
    args.push_back(&direction.x);
    args.push_back(&direction.y);
    args.push_back(&direction.z);
    args.push_back(&slope);
    args.push_back(&init);
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
