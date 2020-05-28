#ifndef JOINT_H
#define JOINT_H

#include <math.h>
#include "geometry.h"

class Joint {
private:
    double angle;
    double minAngle, maxAngle;
    double length;
    const Joint* previous;
    const Joint* next;

public:
    Joint(Joint* previous, Joint* next, double length, double minAngle, double maxAngle) : previous(previous), next(next), length(length), minAngle(minAngle), maxAngle(maxAngle) {};

    Position getPosition(const Joint* relativeTo) const {
        if(relativeTo == this) {
            return Position(angle) * length;
        } else { 
            return Position(angle) + previous->getPosition(relativeTo);
        }
    }

    void setAngle(double angle) {
        if(angle >= minAngle && angle <= maxAngle) {
            this->angle = angle;
        }
    }

    void setMinAngle(double minAngle) {
        this->minAngle = minAngle;
    }

    void setMaxAngle(double maxAngle) {
        this->maxAngle = maxAngle;
    }

    void setLength(double length) {
        this->length = length;
    }

    auto getPosition() const { 
        return getPosition(this);
    }

    auto getAngle(const Joint* relativeTo) const {
        if(relativeTo == this) {
            return angle;
        } else {
            // TODO handle angle wrapping
            return angle + previous->getAngle(relativeTo);
        }
    }

    auto getAngle() const {
        return getAngle(this);
    }

    auto getMinAngle() const {
        return minAngle;
    }

    auto getMaxAngle() const {
        return maxAngle;
    }

    auto getLength() const {
        return length;
    }

};

#endif