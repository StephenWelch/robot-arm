#include "joint.h"

void Joint::setAngle(double angle) {
	if (angle >= minAngle && angle <= maxAngle) {
		this->angle = angle;
	}
}

void Joint::setAngle(double angle, Joint *relativeTo) {
	if (relativeTo==this) {
		this->angle = angle;
	} else if (previous!=nullptr) {
		this->angle = angle - previous->getAngle();
	}
}

void Joint::setMinAngle(double minAngle) {
	this->minAngle = minAngle;
}

void Joint::setMaxAngle(double maxAngle) {
	this->maxAngle = maxAngle;
}

void Joint::setLength(double length) {
	this->length = length;
}

void Joint::setPrevious(Joint *joint) {
	this->previous = joint;
}

void Joint::setNext(Joint *joint) {
	this->next = joint;
}

double Joint::getAngle(const Joint *relativeTo) const {
	if (relativeTo==this) {
		return angle;
	} else if (previous!=nullptr) {
		// TODO handle angle wrapping
		return angle + previous->getAngle(relativeTo);
	} else {
		return angle;
	}
}

double Joint::getAngle() const {
	if (previous!=nullptr) {
		return angle + previous->getAngle();
	} else {
		return angle;
	}
}

Position Joint::getEndPosition() const {
	Position position = getEndPosition(this);
	const Joint *current = previous;
	while (current!=nullptr) {
		position += previous->getEndPosition();
		current = current->getPrevious();
	}
	return position;
}

Position Joint::getBasePosition() const {
	Position position;
	const Joint *current = previous;
	while (current!=nullptr) {
		position += current->getEndPosition();
		current = current->getPrevious();
	}
	return position;
}

Position Joint::getBasePosition(const Joint *relativeTo) const {
	if (relativeTo==this) {
		return Position();
	} else {
		return previous->getEndPosition(relativeTo);
	}
}

Position Joint::getEndPosition(const Joint *relativeTo) const {
	if (relativeTo==this) {
		Position pos = Position(angle);
		Position scaled = pos*length;
		return scaled;
	} else {
		return Position(angle) + previous->getEndPosition(relativeTo);
	}
}

void Joint::link(Joint *prev, Joint *next) {
	prev->setNext(next);
	next->setPrevious(prev);
}
