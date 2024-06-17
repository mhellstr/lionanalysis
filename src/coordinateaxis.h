#ifndef COORDINATEAXIS_H
#define COORDINATEAXIS_H
#include <string>

enum class CoordinateAxis {
    x, y, z
};

CoordinateAxis stringToCoordinateAxis(const std::string&);
std::string coordinateAxisToString(const CoordinateAxis);


#endif
