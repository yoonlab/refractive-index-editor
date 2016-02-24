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

// [Harris 1965; Born and Wolf 1999; Pomraning 2005]
inline glm::dvec3 Lightpath::dwds(glm::dvec3 x, glm::dvec3 w)
{
    return (pMedium->gradient(x) - glm::dot(w, pMedium->gradient(x)) * w) / pMedium->f(x);
}
