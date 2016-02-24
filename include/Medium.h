#pragma once

#include <glm/vec3.hpp>

class Medium
{
public:
    Medium();
    virtual ~Medium();

    virtual double f(glm::dvec3 p) = 0;
    virtual glm::dvec3 gradient(glm::dvec3 p) = 0;
};