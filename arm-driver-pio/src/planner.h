#ifndef PLANNER_H
#define PLANNER_H

#include <math.h>
#include "joint.h"
#include "geometry.h"

class ArmPlanner {
public:
    /**
     * Calculates the joint angles to bring the end joint to the specified position relative
     * to the start joint and absolute angle, and updates the states of the Joint objects accordingly.
     * If there is no solution, Joint states will be left untouched.
    **/
    void calculateAngles(Joint* start, Joint* end, Position goalPos, double goalAngle);

    /**
     * Calculates the joint angles to bring the end joint to the specified position relative
     * to the start joint and updates the state of the Joint objects accordingly. If there is no solution,
     * Joint states will be left untouched.
    **/
    void calculateAngles(Joint* start, Joint* end, Position goalPos);
};

#endif