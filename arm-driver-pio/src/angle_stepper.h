#ifndef ANGLE_STEPPER_H
#define ANGLE_STEPPER_H

#include <AccelStepper.h>
#include <math.h>

class AngleStepper : private AccelStepper {
private:
    const int STEPS_PER_REV;
    const double ANGLE_UNITS_PER_REV;
    bool reversed;
    double minAngle = 0;
    double maxAngle = 360;

    int angleToSteps(double angle) {
        return angle / ANGLE_UNITS_PER_REV * STEPS_PER_REV;
    }

    double toAngle(int steps) {
        return (double) steps / STEPS_PER_REV * ANGLE_UNITS_PER_REV;
    }

    double normalize(double angle) {
        while(angle > ANGLE_UNITS_PER_REV) angle -= ANGLE_UNITS_PER_REV;
        while(angle < 0) angle += ANGLE_UNITS_PER_REV;
        return angle;
    }

    double subtractAngle(double angleA, double angleB) {

        int sign = (angleA - angleB) < 0 ? 1 : -1;

        if(angleA - angleB > ANGLE_UNITS_PER_REV / 2.0) {
            return changeSign(sign, ANGLE_UNITS_PER_REV - angleA - angleB);
        } else if(angleA - angleB < -ANGLE_UNITS_PER_REV / 2.0) {
            return changeSign(sign, ANGLE_UNITS_PER_REV + angleA - angleB);
        } else if(abs(angleA - angleB) == ANGLE_UNITS_PER_REV / 2.0){
            return changeSign(sign, angleA - angleB);
        } else {
            return angleA - angleB;
        }
    }

    double changeSign(int signum, double num) {
        return num > 0 ? num * signum : -signum * num;
    }

    double invert(double angle) {
        return angle < 0 ? -angle : ANGLE_UNITS_PER_REV - angle;
    }

    bool angleInRangeInclusive(double angle, double minAngle, double maxAngle) {
        if(maxAngle >= minAngle) {
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
        if(maxAngle == minAngle) {
            return true;
        }

        if(maxAngle > minAngle) {
            return angle < maxAngle && angle > minAngle;
        }

        double range = abs(subtractAngle(maxAngle, minAngle));
        double distFromMin = subtractAngle(angle, minAngle);
        double distFromMax = subtractAngle(angle, maxAngle);

        bool inMinBounds = distFromMin > 0 && abs(distFromMin) < range;
        bool inMaxBounds = distFromMax < 0 && abs(distFromMax) < range;

        return inMinBounds && inMaxBounds;
    }

public:
    AngleStepper(int stepsPerRev, double anglePerRev, bool reversed = false, uint8_t interface = AccelStepper::FULL4WIRE, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, bool enable = true) : STEPS_PER_REV(stepsPerRev), ANGLE_UNITS_PER_REV(anglePerRev), reversed(reversed), AccelStepper(interface, pin1, pin2, pin3, pin4, enable) {};
    AngleStepper(int stepsPerRev, double anglePerRev, void (*forward)(), void (*backward)(), bool reversed = false) : STEPS_PER_REV(stepsPerRev), ANGLE_UNITS_PER_REV(anglePerRev), reversed(reversed), AccelStepper(forward, backward) {};
    
    using AccelStepper::run;
    using AccelStepper::runSpeed;
    using AccelStepper::runToPosition;
    using AccelStepper::runSpeedToPosition;
    using AccelStepper::stop;
    using AccelStepper::disableOutputs;
    using AccelStepper::enableOutputs;
    using AccelStepper::setMinPulseWidth;
    using AccelStepper::setEnablePin;
    using AccelStepper::setPinsInverted;
    using AccelStepper::isRunning;

    void moveTo(double angle) {
        if(angleInRangeInclusive(angle, minAngle, maxAngle)) {
            double angleToGoal = subtractAngle(angle, currentAbsolutePosition());

            // Handle bounds preventing us from taking shortest movement to goal
            if(angleToGoal > 0) {
                if(angleInRangeExclusive(minAngle, currentAbsolutePosition(), angle) && angleInRangeExclusive(maxAngle, currentAbsolutePosition(), angle)) {
                    angleToGoal = ANGLE_UNITS_PER_REV - angleToGoal;
                }
            } else {
                if(angleInRangeExclusive(minAngle, angle, currentAbsolutePosition()) && angleInRangeExclusive(maxAngle, angle, currentAbsolutePosition())) {
                    angleToGoal = ANGLE_UNITS_PER_REV - angleToGoal;
                }
            }

            move(angleToGoal);
        } else {
            Serial.println("WARNING: Commanded angle outside of limits");
        }
    } 

    void move(double angle) {
        double adjustedAngle = reversed ? -angle : angle;
        AccelStepper::move(angleToSteps(adjustedAngle));
    }

    void setMaxSpeed(double anglePerSec) {
        AccelStepper::setMaxSpeed(angleToSteps(anglePerSec));
    }

    double maxSpeed() {
        return toAngle(AccelStepper::maxSpeed());
    }

    void setAcceleration(double anglePerSecPerSec) {
        AccelStepper::setAcceleration(angleToSteps(anglePerSecPerSec));
    }

    void setSpeed(double anglePerSec) {
        AccelStepper::setSpeed(angleToSteps(anglePerSec));
    }

    double speed() {
        return reversed ? -toAngle(AccelStepper::speed()) : toAngle(AccelStepper::speed());
    }

    double distanceToGo() {
        return toAngle(AccelStepper::distanceToGo());
    }

    double targetPosition() {
        return reversed ? -toAngle(AccelStepper::targetPosition()) : toAngle(AccelStepper::targetPosition());
    }

    double targetAbsolutePosition() {
        return reversed ? invert(normalize(targetPosition())) : normalize(targetPosition());
    }

    double currentPosition() {
        return reversed ? -toAngle(AccelStepper::currentPosition()) : toAngle(AccelStepper::currentPosition());
    }

    double currentAbsolutePosition() {
        return normalize(currentPosition());
    }

    void setCurrentPosition(double angle) {
        double adjustedAngle = reversed ? -angle : angle;
        AccelStepper::setCurrentPosition(angleToSteps(adjustedAngle));
    }

    void runToNewPosition(double angle) {
        move(angle);
        AccelStepper::runToPosition();
    }

    void runToNewAbsolutePosition(double angle) {
        moveTo(angle);
        AccelStepper::runToPosition();
    }

    double getMinAngle() const {
        return minAngle;
    }

    void setMinAngle(double minAngle) {
        this->minAngle = minAngle;
    }

    void setMaxAngle(double maxAngle) {
        this->maxAngle = maxAngle;
    }

    double getMaxAngle() const {
        return maxAngle;
    }

    bool isReversed() const {
        return reversed;
    }

};

#endif