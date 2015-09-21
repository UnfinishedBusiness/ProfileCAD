#ifndef GEOMETRY_
#define GEOMETRY_
#include <application.h>

//Angle 0 degrees is on x horizon

bool geoInTolerance(point_t, point_t, float);
bool geoInTolerance(float, float, float);


point_t geoGetIntersection(line_t, line_t);
point_t geoGetIntersection(arc_t, line_t);
point_t geoGetIntersection(line_t, arc_t);
point_t geoGetIntersection(arc_t, arc_t);

point_t geoGetLineIntersection(line_t, line_t);
point_t geoGetLineArcIntersection(arc_t, line_t);
point_t geoGetArcIntersection(arc_t, arc_t);

std::vector<point_t> geoGetPointsOfLine(line_t);
std::vector<point_t> geoGetPointsOfLine(line_t, float);
line_t geoGetPerpendicularLine(line_t, point_t, float);
line_t geoGetParallelLine(line_t, point_t, float);
float geoGetLineLength(line_t);
point_t geoRotatePointAroundPoint(point_t, point_t, float);
line_t geoRotateLine(line_t, point_t, float ); //angle is in degrees!
line_t geoExtendLine(line_t, float);
line_t geoExtendLineAngle(point_t, float, float);
point_t geoGetLineMidpoint(line_t);
float geoGetLineAngle(line_t);
float geoRadiansToDegrees(float);
float geoDegreesToRadians(float);

float geoGetArcLength(arc_t a);
point_t geoGetArcEndpoint(arc_t, float);
float geoGetArcStartAngleAbs(arc_t); //Returns degrees!
float geoGetArcEndAngleAbs(arc_t); //Returns degrees!
float geoGetArcStartAngle(arc_t);
float geoGetArcEndAngle(arc_t);
point_t geoGetArcPoint(arc_t, float);
std::vector<point_t> geoGetPointsOfArc(arc_t);
bool geoGetArcDirection(float, float); //Return direction given start and end angle
float geoGetIncludedAngle(arc_t); //Returns in degrees!
float geoRound(float);
#endif
