#pragma once
 
#include <cmath>
 
const int EarthRad = 6371000;
 
const double PI = 3.14159265359;
 
struct Coordinates {
 
    double lat;
 
    double lng;
 
    bool operator==(const Coordinates& other) const {
 
        return lat == other.lat && lng == other.lng;
 
    }
 
    bool operator!=(const Coordinates& other) const {
 
        return !(*this == other);
 
    }
 
};
 
inline double ComputeDistance(Coordinates from, Coordinates to) {
 
    using namespace std;
 
    if (from == to) {
 
        return 0;
 
    }
 
    static const double dr = PI / 180.;
 
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
 
        + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
 
        * EarthRad;
 
}
 
