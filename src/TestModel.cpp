#include "TestModel.h"
#include <glm/glm.hpp>

TestModel::TestModel(double bgn, double bgf, double idn, double ia,
    double ih, double hsn, double hsd, double _scale)
    : backgroundMaxN(bgn), backgroundFlatness(bgf),
    inversionDeltaN(idn), inversionA(ia), inversionHciso(ih), 
    hotspotN(hsn), hotspotDropoff(hsd), scale(_scale)
{
    args.push_back(&backgroundMaxN);
    _arg_max.push_back(1e100);
    _arg_min.push_back(0);
    args.push_back(&backgroundFlatness);
    _arg_max.push_back(1e100);
    _arg_min.push_back(1e-6);
    args.push_back(&inversionDeltaN);
    _arg_max.push_back(1e10);
    _arg_min.push_back(0);
    args.push_back(&inversionA);
    _arg_max.push_back(1e10);
    _arg_min.push_back(1e-6);
    args.push_back(&inversionHciso);
    _arg_max.push_back(1e100);
    _arg_min.push_back(0);
    args.push_back(&hotspotN);
    _arg_max.push_back(1e10);
    _arg_min.push_back(0);
    args.push_back(&hotspotDropoff);
    _arg_max.push_back(1e10);
    _arg_min.push_back(1e-6);
    args.push_back(&scale);
    _arg_max.push_back(1e100);
    _arg_min.push_back(1e-6);
}

TestModel::~TestModel()
{
}

double TestModel::f(glm::dvec3 p)
{
    double altitude = p.y;
    double deStd = 
        background(altitude) +
        inversionLayer(altitude) +
        hotspot(altitude);
    double deStdNormalized = deStd / (abs(backgroundMaxN) + abs(inversionDeltaN) + abs(hotspotN));
    double refrIdx = deStdNormalized * abs(scale) * 0.003 + 1;
    return refrIdx;
}

glm::dvec3 TestModel::gradient(glm::dvec3 p)
{
    double altitude = p.y;
    double deStdDeriv =
        backgroundDeriv(altitude) +
        inversionLayerDeriv(altitude) +
        hotspotDeriv(altitude);
    double deStdDerivNormalized = deStdDeriv / (abs(backgroundMaxN) + abs(inversionDeltaN) + abs(hotspotN));
    double dyn = deStdDerivNormalized * abs(scale) * 0.003;
    return glm::dvec3(0, dyn, 0);
}

std::vector<double *> *TestModel::arguments()
{
    return &args;
}

std::vector<double> *TestModel::arg_max()
{
    return &_arg_max;
}

std::vector<double> *TestModel::arg_min()
{
    return &_arg_min;
}

inline double TestModel::logistic(const double &L, const double &invK, 
    const double &xNaught, double x) const
{
    return L / (1 + exp((xNaught - x) / invK));
}

inline double TestModel::logisticDeriv(const double &L, const double &invK,
    const double &xNaught, double x) const
{
    return L / (2 * invK * cosh((xNaught - x) / invK) + 2 * invK);
}

inline double TestModel::background(const double &altitude) const
{
    return logistic(backgroundMaxN, 10*backgroundFlatness, 0, altitude);
}

inline double TestModel::backgroundDeriv(const double &altitude) const
{
    return logisticDeriv(backgroundMaxN, 10*backgroundFlatness, 0, altitude);
}

inline double TestModel::inversionLayer(const double &altitude) const
{
    return logistic(inversionDeltaN, -0.1*inversionA, inversionHciso, altitude);
}

inline double TestModel::inversionLayerDeriv(const double &altitude) const
{
    return logisticDeriv(inversionDeltaN, -0.1*inversionA, inversionHciso, altitude);
}

inline double TestModel::hotspot(const double &altitude) const
{
    return logistic(hotspotN, 0.3*hotspotDropoff, 0, altitude);
}

inline double TestModel::hotspotDeriv(const double &altitude) const
{
    return logisticDeriv(hotspotN, 0.3*hotspotDropoff, 0, altitude);
}