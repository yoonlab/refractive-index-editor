#pragma once

#include "Medium.h"

class Linear : public Medium
{
public:
    Linear(glm::vec3 _direction, double _slope, double _init);
    virtual ~Linear();

    double f(glm::vec3 p);
    glm::vec3 gradient(glm::vec3 p);

protected:
    glm::vec3 direction;
    double slope;
    double init;
};