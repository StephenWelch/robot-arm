#include "position.h"

Position Position::operator+(const Position &other) const {
	return Position(x + other.x, y + other.y);
}

Position Position::operator-(const Position &other) const {
	return Position(x - other.x, y - other.y);
}
Position Position::operator*(double scalar) const {
	return Position(x*scalar, y*scalar);
}

Position &Position::operator+=(const Position &other) {
	x += other.x;
	y += other.y;

	return *this;
}

Position &Position::operator-=(const Position &other) {
	x -= other.x;
	y -= other.y;

	return *this;
}

Position &Position::operator*=(double scalar) {
	x *= scalar;
	y *= scalar;

	return *this;
}

Position &Position::operator/=(double scalar) {
	this->operator*=(1/scalar);
	return *this;
}

double Position::getAngle() const {
	return atan2(y, x);
}

double Position::getLength() const {
	return sqrt((x*x) + (y*y));
}

Position Position::rotate(double angle) const {
	return Position(
			(x*cos(angle)) - (y*sin(angle)),
			(x*sin(angle)) + (y*cos(angle))
	);
}

double Position::getDistanceTo(Position other) const {
	return (this->operator-(other)).getLength();
}

std::ostream &operator<<(std::ostream &stream, const Position &position) {
	return stream << "X:\t" << position.x << "\tY:\t" << position.y;
}
