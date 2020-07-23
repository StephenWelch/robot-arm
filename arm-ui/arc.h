//
// Created by Stephen Welch on 7/7/2020.
//

#ifndef ARC_H
#define ARC_H

#include <vector>
#include "position.h"

class Arc {
 public:
	Arc(double startAngle, double endAngle, double radius, const Position &center = Position());

	double getArcLength() const;
	std::vector<Position> getIntersection(const Arc &other) const;

	auto getRadius() const { return radius; }
	auto getStartAngle() const { return startAngle; }
	auto getEndAngle() const { return endAngle; }
	auto getCenter() const { return center; }
 private:
	double radius;
	double startAngle;
	double endAngle;
	Position center;
};

#endif
