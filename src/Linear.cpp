#include "Linear.h"
#include <glm/glm.hpp>

Linear::Linear(glm::vec3 _direction, double _slope, double _init)
    : direction(_direction), slope(_slope), init(_init)
{
}

Linear::~Linear()
{
}

double Linear::f(glm::vec3 p)
{
    return init + slope * glm::dot(p, direction);
}

glm::vec3 Linear::gradient(glm::vec3 p)
{
    return direction;
}