#ifndef __SIMPLE_MAP_MANAGER__
#define __SIMPLE_MAP_MANAGER__

#include "dg_core.hpp"

// rapidjson header files
#include "../../EXTERNAL/rapidjson/include/rapidjson/document.h" 
#include "../../EXTERNAL/rapidjson/include/rapidjson/stringbuffer.h"
#include "../../EXTERNAL/rapidjson/include/rapidjson/prettywriter.h"
#include <fstream>
using namespace rapidjson;

// curl header files
#include "../../EXTERNAL/curl/include/curl/curl.h"

#define M_PI 3.14159265358979323846

//
//enum NodeType
//{
//	NODE_BASIC = 0,
//	NODE_CROSS = 1,
//	NODE_DOOR = 2,
//	NODE_ELEVATOR = 3
//};
//enum EdgeType
//{
//	EDGE_SIDEWALK = 0,
//	EDGE_STREET = 1,
//	EDGE_CROSSWALK = 2
//};

namespace dg
{

class MapManager
{
public:
	/**
	 * The default constructor
	 */
	MapManager() {}

	int long2tilex(double lon, int z);
	int lat2tiley(double lat, int z);
	double tilex2long(int x, int z);
	double tiley2lat(int y, int z);
	void downloadMap(cv::Point2i tile);
	cv::Point2i lonlat2xy(double lon, double lat, int z);
	/**
	 * Read a map from the given file
	 * @param lon longitude
	 * @param lat latitude
	 * @param z zoom
	 * @return Result of success (true) or failure (false)
	 */
	bool load(double lon = 128, double lat = 38, int z = 19);
	///**
	// * Check whether this map is empty or not
	// * @return True if empty (true) or not (false)
	// */
	//bool isEmpty() const;

	bool generatePath();
	Path getPath(const char* filename = "test_simple_Path.json");
	
	Map& getMap(Path path);
	Map& getMap();

	std::vector<cv::Point2d> getPOIloc(const char* poiname = "UST");

protected:
	Map m_map;
	Path m_path;

	double m_lon = 0;
	double m_lat = 0;
	int m_z = 0;
};

} // End of 'dg'

#endif // End of '__SIMPLE_MAP_MANAGER__'