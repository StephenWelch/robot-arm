#ifndef POSITION_H
#define POSITION_H

#include <QtMath>
#include <ostream>

class Position {
 public:
	double x, y;

	Position(double x, double y) : x(x), y(y) {};
	Position(const Position &other) : x(other.x), y(other.y) {};
	Position() : x(0), y(0) {};
	Position(double angle) : x(cos(angle)), y(sin(angle)) {};

	Position operator+(const Position &other) const;
	Position operator-(const Position &other) const;
	Position operator*(double scalar) const;

	Position &operator+=(const Position &other);
	Position &operator-=(const Position &other);
	Position &operator*=(double scalar);
	Position &operator/=(double scalar);

	double getAngle() const;
	double getLength() const;

	Position rotate(double angle) const;
	double getDistanceTo(Position other) const;

};

std::ostream &operator<<(std::ostream &stream, const Position &position);

#endif // POSITION_H
