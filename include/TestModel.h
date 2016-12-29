#pragma once

#include "Medium.h"
#include <vector>

class TestModel : public Medium
{
public:
    TestModel(double bgn, double bgf, double idn, double ia, 
        double ih, double hsn, double hsd, double scale);
    virtual ~TestModel();

    double f(glm::dvec3 p);
    glm::dvec3 gradient(glm::dvec3 p);
    std::vector<double *> *arguments();
    std::vector<double> *arg_max();
    std::vector<double> *arg_min();

    void reinitialize();

protected:
    // L/(1+exp(-(x-xNaught)/invK))
    inline double logistic(const double &L, const double &invK, 
        const double &xNaught, double x) const;
    inline double logisticDeriv(const double &L, const double &invK,
        const double &xNaught, double x) const;

    // all altitudes below are in unit of meter
    inline double background(const double &altitude) const;
    inline double backgroundDeriv(const double &altitude) const;

    inline double inversionLayer(const double &altitude) const;
    inline double inversionLayerDeriv(const double &altitude) const;

    inline double hotspot(const double &altitude) const;
    inline double hotspotDeriv(const double &altitude) const;

    // args
    double backgroundMaxN;
    double backgroundFlatness;
    double inversionDeltaN;
    double inversionA;
    double inversionHciso;
    double hotspotN;
    double hotspotDropoff;
    double scale;

    std::vector<double *> args;
    std::vector<double> _arg_max;
    std::vector<double> _arg_min;
    std::vector<double> _arg_init;
};