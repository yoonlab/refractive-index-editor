#pragma once

#include <glm/vec3.hpp>

class Medium
{
public:
    Medium();
    virtual ~Medium();

    virtual double f(glm::vec3 p) = 0;
    virtual glm::vec3 gradient(glm::vec3 p) = 0;
};