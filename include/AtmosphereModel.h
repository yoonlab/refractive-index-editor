#pragma once

#include "Medium.h"
#include <vector>

class AtmosphereModel : public Medium
{
public:
    AtmosphereModel(double mpu, double idt, double ia, double ih,
        double hst, double hsd);
    virtual ~AtmosphereModel();

    double f(glm::dvec3 p);
    glm::dvec3 gradient(glm::dvec3 p);
    std::vector<double *> *arguments();

protected:
    double meterPerUnit;

    double scale = 1.0;

    const double M = 0.0289644;
    const double R = 8.31432;
    const double Navg = 1.000293;
    const double MoverRbyNavgMinusOne = 1.020717e-6;

    // temp derivative of US standard atmosphere model
    const double tempDeriv = -0.0065;

    // all altitudes below are in unit of meter
    inline double usStdAtmTemp(const double &altitude) const;
    inline double usStdAtmPressure(const double &altitude) const;
    inline double usStdAtmPressureDeriv(const double &altitude) const;

    inline double inversionLayer(const double &altitude) const;
    inline double inversionLayerDeriv(const double &altitude) const;

    inline double hotspot(const double &altitude) const;
    inline double hotspotDeriv(const double &altitude) const;

    // args
    double inversionDeltaT;
    double inversionA;
    double inversionHciso;
    double hotspotT;
    double hotspotDropoff;

    std::vector<double *> args;
};