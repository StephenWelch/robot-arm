//
// Created by Stephen Welch on 7/18/2020.
//

#ifndef CIRCLE_H
#define CIRCLE_H

#include <vector>
#include "position.h"
class Circle {
 public:
	Circle(const Position &center, double radius) : center(center), radius(radius) {};
	Circle(double centerX, double centerY, double radius) : Circle(Position(centerX, centerY), radius) {};

	auto getCenter() const { return center; }
	auto getRadius() const { return radius; }
	auto x() const { return center.x; }
	auto y() const { return center.y; }

	std::vector<Position> getIntersection(const Circle &other) const;

 private:
	Position center;
	double radius;

};

#endif //CIRCLE_H
