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
	if (previous!=nullptr) {
		return position + previous->getEndPosition();
	}
	return position;
}

Position Joint::getBasePosition() const {
	if (previous!=nullptr) {
		return previous->getEndPosition();
	}
	return {};
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
		return Position(getAngle()) * length;
	} else {
		return Position(getAngle()) + previous->getEndPosition(relativeTo);
	}
}

void Joint::link(Joint *prev, Joint *next) {
	prev->setNext(next);
	next->setPrevious(prev);
}

void Joint::link(std::vector<Joint*> jointsToLink) {
	for(int i = 0; i < jointsToLink.size() - 1; i++) {
		link(jointsToLink[i], jointsToLink[i+1]);
	}
}

Joint *Joint::getBase() const {
	Joint* current = getPrevious();
	if(current != nullptr && current->getPrevious() != nullptr) {
		current = current->getPrevious();
	}
	return current;
}

Joint *Joint::getEnd() const {
	Joint* current = getNext();
	if(current != nullptr && current->getNext() != nullptr) {
		current = current->getNext();
	}
	return current;
}

void Joint::solve(Joint* relativeTo, const Joint &targetPos) {
	Joint *end = relativeTo->getEnd();

	end->setAngle(targetPos.getAngle());
}
