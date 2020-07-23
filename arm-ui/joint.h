#ifndef JOINT_H
#define JOINT_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QPainter>
#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include "position.h"
#include <iostream>

class Joint {
 private:
	double angle = 0;
	double minAngle, maxAngle;
	double length;
	Joint *previous;
	Joint *next;

 public:
	Joint(Joint *previous, Joint *next, double length, double minAngle, double maxAngle)
			: previous(previous), next(next), length(length), minAngle(minAngle), maxAngle(maxAngle
	) {};
	Joint(double length, double minAngle, double maxAngle)
			: previous(nullptr), next(nullptr), length(length), minAngle(minAngle), maxAngle(maxAngle) {};

	auto getMinAngle() const {
		return minAngle;
	}

	auto getMaxAngle() const {
		return maxAngle;
	}

	auto getLength() const {
		return length;
	}

	auto getPrevious() const {
		return previous;
	}

	auto getNext() const {
		return next;
	}

	void setAngle(double angle);
	void setAngle(double angle, Joint *relativeTo);

	void setMinAngle(double minAngle);
	void setMaxAngle(double maxAngle);
	void setLength(double length);
	void setPrevious(Joint *joint);
	void setNext(Joint *joint);

	double getAngle(const Joint *relativeTo) const;
	double getAngleRelativeToOrigin() const;

	Position getEndPosition(const Joint *relativeTo) const;
	Position getEndPositionRelativeToOrigin() const;

	Position getBasePosition(const Joint *relativeTo) const;
	Position getBasePositionRelativeToOrigin() const;

	Joint *getBase() const;
	Joint *getEnd() const;
	std::vector<Joint *> getConsecutiveJoints() const;

	static void link(Joint *prev, Joint *next);
	static void link(std::vector<Joint *> jointsToLink);
	static void solve(Joint *relativeTo, const Joint &targetPos);

};

#endif // JOINT_H
