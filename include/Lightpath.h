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
    void solve2(double s_0, double step, double s_max, glm::dvec3 x_0, glm::dvec3 v_0);
    void solve2();
    double dist(glm::dvec3 p);
    double distToTarget();
    void getCurveVertices(std::vector<PosColorVertex> *verticesOut);
    void setTargetPoint(glm::dvec3 p) { targetPoint = glm::dvec3(p); }
    Medium *getMediumPtr() { return pMedium; }

protected:
    inline glm::dvec3 dwds(glm::dvec3 x, glm::dvec3 w);

    double s_0, step, s_max;
    glm::dvec3 x_0, v_0;
    std::vector<std::pair<double, glm::dvec3>> path;
    Medium *pMedium;
    glm::dvec3 targetPoint;
};