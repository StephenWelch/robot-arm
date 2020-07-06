#ifndef ANGLE_H
#define ANGLE_H

#include <cmath>
#include "position.h"

class Angle {
 private:
	const double ANGLE_UNITS_PER_REV = 360;
	double value;

	double radiansToUnits(double radians) {
		return radians/2/M_PI*ANGLE_UNITS_PER_REV;
	}

	double degreesToUnits(double degrees) {
		return degrees/360*ANGLE_UNITS_PER_REV;
	}

	double custom_atan2(double y, double x) {
		return radiansToUnits(atan2(y, x));
	}

 public:
	Angle(double value) : value(value) {};
	Angle(double x, double y) : value(custom_atan2(y, x)) {};
	Angle(const Position &position) : value(custom_atan2(position.y, position.x)) {};

	double normalize(double angle) {
		while (angle > ANGLE_UNITS_PER_REV) angle -= ANGLE_UNITS_PER_REV;
		while (angle < 0) angle += ANGLE_UNITS_PER_REV;
		return angle;
	}

	double subtractAngle(double angleA, double angleB) {
		int sign = (angleA - angleB) < 0 ? 1 : -1;

		if (angleA - angleB > 180.0) {
			return changeSign(sign, ANGLE_UNITS_PER_REV - angleA - angleB);
		} else if (angleA - angleB < -180.0) {
			return changeSign(sign, ANGLE_UNITS_PER_REV + angleA - angleB);
		} else if (abs(angleA - angleB)==180.0) {
			return changeSign(sign, angleA - angleB);
		} else {
			return angleA - angleB;
		}
	}

	double changeSign(int signum, double num) {
		return num > 0 ? num*signum : -signum*num;
	}

	double invert(double angle) {
		return angle < 0 ? -angle : ANGLE_UNITS_PER_REV - angle;
	}

	bool angleInRangeInclusive(double angle, double minAngle, double maxAngle) {
		if (maxAngle >= minAngle) {
			return angle <= maxAngle && angle >= minAngle;
		}

		double range = abs(subtractAngle(maxAngle, minAngle));
		double distFromMin = subtractAngle(angle, minAngle);
		double distFromMax = subtractAngle(angle, maxAngle);

		bool inMinBounds = distFromMin >= 0 && abs(distFromMin) <= range;
		bool inMaxBounds = distFromMax <= 0 && abs(distFromMax) <= range;

		return inMinBounds && inMaxBounds;
	}

	bool angleInRangeExclusive(double angle, double minAngle, double maxAngle) {
		if (maxAngle==minAngle) {
			return true;
		}

		if (maxAngle > minAngle) {
			return angle < maxAngle && angle > minAngle;
		}

		double range = abs(subtractAngle(maxAngle, minAngle));
		double distFromMin = subtractAngle(angle, minAngle);
		double distFromMax = subtractAngle(angle, maxAngle);

		bool inMinBounds = distFromMin > 0 && abs(distFromMin) < range;
		bool inMaxBounds = distFromMax < 0 && abs(distFromMax) < range;

		return inMinBounds && inMaxBounds;
	}
};

#endif // ANGLE_H
