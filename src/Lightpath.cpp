#include "Lightpath.h"
#include <glm/glm.hpp>

Lightpath::Lightpath(Medium *_pMedium) : pMedium(_pMedium)
{
}

Lightpath::~Lightpath()
{
}

// Solving numerically using RK4 for 2nd order ODE
void Lightpath::solve(double s_0, double step, double s_max, glm::dvec3 x_0, glm::dvec3 w_0)
{
    path.clear();

    glm::dvec3 dx1, dx2, dx3, dx4, dw1, dw2, dw3, dw4, dx, dw;
    glm::dvec3 x, w;

    x = x_0;
    w = w_0;

    this->s_0 = s_0;
    this->s_max = s_max;

    for (double s = s_0; s < s_max; s += step)
    {
        path.push_back(std::pair<double, glm::dvec3>(s, x));
        dx1 = step * w;
        dw1 = step * dwds(x, w);
        dx2 = step * (w + dw1 / 2.0);
        dw2 = step * dwds(x + dx1 / 2.0, w + dw1 / 2.0);
        dx3 = step * (w + dw2 / 2.0);
        dw3 = step * dwds(x + dx2 / 2.0, w + dw2 / 2.0);
        dx4 = step * (w + dw3);
        dw4 = step * dwds(x + dx3, w + dw3);
        dx = (dx1 + 2.0 * dx2 + 2.0 * dx3 + dx4) / 6.0;
        dw = (dw1 + 2.0 * dw2 + 2.0 * dw3 + dw4) / 6.0;
        x += dx;
        w += dw;
    }
}

void Lightpath::solve2(double s_0, double step, double s_max, glm::dvec3 x_0, glm::dvec3 v_0)
{
    this->s_0 = s_0;
    this->step = step;
    this->s_max = s_max;
    this->x_0 = x_0;
    this->v_0 = v_0;

    solve2();
}

void Lightpath::solve2()
{
    path.clear();

    glm::dvec3 dx1, dx2, dx3, dx4, dv1, dv2, dv3, dv4, dx, dv;
    glm::dvec3 x, v;

    x = x_0;
    v = v_0;

    for (double s = s_0; s < s_max; s += step)
    {
        path.push_back(std::pair<double, glm::dvec3>(s, x));
        dx1 = step * v / pMedium->f(x);
        dv1 = step * pMedium->gradient(x);
        dx2 = step * (v + dv1 / 2.0) / pMedium->f(x + dx1 / 2.0);
        dv2 = step * pMedium->gradient(x + dx1 / 2.0);
        dx3 = step * (v + dv2 / 2.0) / pMedium->f(x + dx2 / 2.0);
        dv3 = step * pMedium->gradient(x + dx2 / 2.0);
        dx4 = step * (v + dv3) / pMedium->f(x + dx3);
        dv4 = step * pMedium->gradient(x + dx3);
        dx = (dx1 + 2.0 * dx2 + 2.0 * dx3 + dx4) / 6.0;
        dv = (dv1 + 2.0 * dv2 + 2.0 * dv3 + dv4) / 6.0;
        x += dx;
        v += dv;
    }
}

// Brute force
double Lightpath::dist(glm::dvec3 p)
{
    double dist = INFINITY;
    for (auto point : path)
    {
        dist = glm::min(dist, glm::length(point.second - p));
    }
    return dist;
}

double Lightpath::distToTarget()
{
    return dist(targetPoint);
}

void Lightpath::getCurveVertices(std::vector<PosColorVertex>* verticesOut)
{
    verticesOut->clear();
    for (auto point : path)
    {
        PosColorVertex v;
        v.color[0] = 1.0f;
        v.color[1] = 1.0f;
        v.color[2] = (point.first - s_0) / (s_max - s_0);
        v.position[0] = point.second.x;
        v.position[1] = point.second.y;
        v.position[2] = point.second.z;
        verticesOut->push_back(v);
    }
}

// [Harris 1965; Born and Wolf 1999; Pomraning 2005]
inline glm::dvec3 Lightpath::dwds(glm::dvec3 x, glm::dvec3 w)
{
    return (pMedium->gradient(x) - glm::dot(w, pMedium->gradient(x)) * w) / pMedium->f(x);
}
