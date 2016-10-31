#include "AtmosphereModel.h"
#include <glm/glm.hpp>

AtmosphereModel::AtmosphereModel(double mpu, double idt, double ia, double ih,
    double hst, double hsd)
    : meterPerUnit(mpu), inversionDeltaT(idt), inversionA(ia),
    inversionHciso(ih), hotspotT(hst), hotspotDropoff(hsd)
{
    args.push_back(&inversionDeltaT);
    args.push_back(&inversionA);
    args.push_back(&inversionHciso);
    args.push_back(&hotspotT);
    args.push_back(&hotspotDropoff);
    args.push_back(&scale);
}

AtmosphereModel::~AtmosphereModel()
{
}

double AtmosphereModel::f(glm::dvec3 p)
{
    double altitude = p.y * meterPerUnit;
    double temp = usStdAtmTemp(altitude) + /*inversionLayer(altitude) +*/ hotspot(altitude);
    //double refrIdx = usStdAtmPressure(altitude) * M / R / temp * (Navg - 1) + 1;
    double refrIdx = MoverRbyNavgMinusOne * usStdAtmPressure(altitude) / temp + 1;
    //return temp;
    return refrIdx * scale;
}

glm::dvec3 AtmosphereModel::gradient(glm::dvec3 p)
{
    double altitude = p.y * meterPerUnit;
    double temp = usStdAtmTemp(altitude) + /*inversionLayer(altitude) +*/ hotspot(altitude);
    double pressure = usStdAtmPressure(altitude);
    double dzt = tempDeriv + /*inversionLayerDeriv(altitude) +*/ hotspotDeriv(altitude);
    double dzp = usStdAtmPressureDeriv(altitude);
    double dzn = MoverRbyNavgMinusOne * (temp * dzp - pressure * dzt) / (temp * temp);
    return glm::dvec3(0, 0, dzn) * meterPerUnit * scale;
}

std::vector<double *> *AtmosphereModel::arguments()
{
    return &args;
}

// Only the first subscript (~11km)
inline double AtmosphereModel::usStdAtmTemp(const double &altitude) const
{
    return 288.15 + tempDeriv * altitude;
}

inline double AtmosphereModel::usStdAtmPressure(const double &altitude) const
{
    return 101325 * pow((1 - altitude * 2.25694e-5), 5.25588);
}

inline double AtmosphereModel::usStdAtmPressureDeriv(const double &altitude) const
{
    return -12.0194 * pow((1 - altitude * 2.25694e-5), 4.25588);
}

inline double AtmosphereModel::inversionLayer(const double &altitude) const
{
    return -inversionDeltaT + inversionDeltaT / (1 + exp((altitude - inversionHciso) / inversionA));
}

inline double AtmosphereModel::inversionLayerDeriv(const double &altitude) const
{
    return (-inversionDeltaT / 2) / (inversionA * cosh((inversionHciso - altitude) / inversionA) + inversionA);
}

inline double AtmosphereModel::hotspot(const double &altitude) const
{
    return hotspotT * exp(-altitude / hotspotDropoff);
}

inline double AtmosphereModel::hotspotDeriv(const double &altitude) const
{
    return -hotspotT * exp(-altitude / hotspotDropoff) / hotspotDropoff;
}