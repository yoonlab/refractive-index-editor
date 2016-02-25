#pragma once

#include "VertexTypes.h"
#include "Medium.h"
#include <vector>

class Lightpath
{
public:
    Lightpath(Medium *_pMedium);
    virtual ~Lightpath();

    void solve(double s_0, double step, double s_max, glm::dvec3 x_0, glm::dvec3 w_0);
    double dist(glm::dvec3 p);

protected:
    inline glm::dvec3 dwds(glm::dvec3 x, glm::dvec3 w);

    std::vector<std::pair<double, glm::dvec3>> path;
    Medium *pMedium;
};