#ifndef GEOMETRY_
#define GEOMETRY_
#include <application.h>

point_t geoGetLineIntersection(line_t, line_t);
std::vector<point_t> geoGetPointsOfLine(point_t, point_t);
line_t geoGetPerpendicularLine(line_t, point_t, float);
line_t geoGetParallelLine(line_t, point_t, float);
float geoGetLineLength(line_t);
point_t geoRotatePointAroundPoint(point_t, point_t, float);
line_t geoRotateLine(line_t, point_t, float );
line_t geoExtendLine(line_t, float);
line_t geoExtendLineAngle(point_t, float, float);
point_t geoGetLineMidpoint(line_t);
float geoGetLineAngle(line_t);
float geoRadiansToDegrees(float);

point_t geoGetArcEndpoint(arc_t, float);
float geoGetArcStartAngle(arc_t);
#endif
