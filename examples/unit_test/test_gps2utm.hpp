#ifndef __TEST_GPS_2_UTM__
#define __TEST_GPS_2_UTM__

#include "data_structure.hpp"

int testLatLon()
{
    dg::LatLon a, b(1, 2);
    VVS_CHECK_TRUE(a.lat == 0);
    VVS_CHECK_TRUE(a.lon == 0);
    VVS_CHECK_TRUE(a.x == 0);
    VVS_CHECK_TRUE(a.y == 0);
    VVS_CHECK_TRUE(b.lat == 1);
    VVS_CHECK_TRUE(b.lon == 2);
    VVS_CHECK_TRUE(b.x == 1);
    VVS_CHECK_TRUE(b.y == 2);

    a.lat = 1;
    a.lon = 2;
    VVS_CHECK_TRUE(a.lat == 1);
    VVS_CHECK_TRUE(a.lon == 2);
    VVS_CHECK_TRUE(a.x == 1);
    VVS_CHECK_TRUE(a.y == 2);
    VVS_CHECK_TRUE(a == b);

    b = dg::Point2(3, 4);
    VVS_CHECK_TRUE(b.lat == 3);
    VVS_CHECK_TRUE(b.lon == 4);
    VVS_CHECK_TRUE(b.x == 3);
    VVS_CHECK_TRUE(b.y == 4);
    VVS_CHECK_TRUE(a != b);

    a = 2 * b - b;
    VVS_CHECK_TRUE(a.lat == 3);
    VVS_CHECK_TRUE(a.lon == 4);
    VVS_CHECK_TRUE(a.x == 3);
    VVS_CHECK_TRUE(a.y == 4);
    VVS_CHECK_TRUE(a == b);

    return 0;
}

#ifndef UTM_H
    extern int  LatLonToUTMXY(double lat, double lon, int zone, double& x, double& y);
    extern void UTMXYToLatLon(double x, double y, int zone, bool southhemi, double& lat, double& lon);
#endif

int testRawGPS2UTM(const dg::LatLon& x, const dg::Point2& sol, bool verbose = true)
{
    dg::Point2 y;
    int zone = LatLonToUTMXY(x.lat, x.lon, -1, y.x, y.y);
    if (verbose) std::cout << "[VERBOSE] LatLon = " << x << " --> UTM = " << y << ", Zone = " << zone << std::endl;
    VVS_CHECK_RANGE(y.x, sol.x, 0.1);
    VVS_CHECK_RANGE(y.y, sol.y, 0.1);
    return 0;
}

int testRawUTM2GPS(const dg::Point2& x, int zone, bool is_south, const dg::LatLon& sol, bool verbose = true)
{
    dg::LatLon y;
    UTMXYToLatLon(x.x, x.y, zone, is_south, y.lat, y.lon);
    y *= 180 / CV_PI; // [rad] to [deg]
    if (verbose) std::cout << "[VERBOSE] UTM = " << x << ", Zone = " << zone << " --> LatLon = " << y << std::endl;
    VVS_CHECK_RANGE(y.lat, sol.lat, 0.1);
    VVS_CHECK_RANGE(y.lon, sol.lon, 0.1);
    return 0;
}

#endif // End of '__TEST_GPS_2_UTM__'