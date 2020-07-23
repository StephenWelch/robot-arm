//
// Created by Stephen Welch on 7/7/2020.
//

#include <algorithm>
#include "arc.h"

Arc::Arc(double startAngle, double endAngle, double radius, const Position &center) : center(center) {
	this->startAngle = std::clamp(startAngle, 0.0, 2*M_PI);
	this->endAngle = std::clamp(endAngle, 0.0, 2*M_PI);
	this->radius = abs(radius);
}

double Arc::getArcLength() const {
	double rawDifference = endAngle - startAngle;

	return rawDifference < 0 ? rawDifference + 2*M_PI : rawDifference;
}

std::vector<Position> Arc::getIntersection(const Arc &other) const {

	return std::vector<Position>();
}
