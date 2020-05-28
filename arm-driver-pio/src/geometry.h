#ifndef GEOMETRY_H
#define GEOMETRY_H

class Position {
public:
    double x, y;

    Position(double x, double y) : x(x), y(y) {};
    Position(double angle) : x(cos(angle)), y(sin(angle)) {};

    Position operator + (Position other) const {
        return Position(x + other.x, y + other.y);
    }

    Position operator - (Position other) const {
        return Position(x - other.x, y - other.y);
    }

    Position operator * (float scalar) const {
        return Position(x * scalar, y * scalar);
    }

    double getAngle() const {
        return atan2(y, x);
    }

    double getLength() const{
        return sqrt((x * x) + (y * y));
    }

    Position rotate(double angle) const {
        return Position(
            (x * cos(angle)) - (y * sin(angle)),
            (x * sin(angle)) + (y * cos(angle))
        );
    }

    double getDistanceTo(Position other) const {
        return (this->operator-(other)).getLength();
    }

};

// class Circle {
// public:
//     Position center;
//     double radius;

//     Circle(Position center, double radius) : center(center), radius(radius) {};

//     Position* getIntersections(Circle other) const {
//         double D = getDistanceTo(other);
//     }

//     double getDistanceTo(Circle other) const {
//         return center.getDistanceTo(other.center);
//     }

// };

#endif