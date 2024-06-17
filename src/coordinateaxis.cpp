#include "coordinateaxis.h"


CoordinateAxis stringToCoordinateAxis(const std::string &s) {
    if (s=="x" || s=="X") return CoordinateAxis::x;
    else if (s=="y" || s =="Y") return CoordinateAxis::y;
    else if (s=="z" || s == "Z") return CoordinateAxis::z;
    else throw std::string("Unknown coordinate axis: " + s);
}

std::string coordinateAxisToString(const CoordinateAxis a) {
    switch (a) {
        case CoordinateAxis::x: return "x"; break;
        case CoordinateAxis::y: return "y"; break;
        case CoordinateAxis::z: return "z"; break;
    }
    return "unknown";
}
