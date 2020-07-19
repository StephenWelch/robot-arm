//
// Created by Stephen Welch on 7/18/2020.
//

#include "circle.h"

std::vector<Position> Circle::getIntersection(const Circle &other) const {

	double d = (other.getCenter() - getCenter()).getLength();

	if(d > getRadius() + other.getRadius() || d <= abs(getRadius() - other.getRadius()) || d == 0) {
		return {};
	}

	double a = (pow(getRadius(), 2) - pow(other.getRadius(), 2) + pow(d, 2)) / (2.0 * d);

	double h = sqrt(pow(getRadius(), 2) - pow(a, 2));

	double chordMidpointX = x() + a / d * (other.x() - x());
	double chordMidpointY = y() + a / d * (other.y() - y());

	double chordXLength = h / d * (other.y() - y());
	double chordYLength = h / d * (other.x() - x());

	if(a == getRadius()) {
		return {
			Position(chordMidpointX + chordXLength, chordMidpointY - chordYLength)
		};
	} else {
		return {
				Position(chordMidpointX + chordXLength, chordMidpointY - chordYLength),
				Position(chordMidpointX - chordXLength, chordMidpointY + chordYLength)
		};
	}
}
