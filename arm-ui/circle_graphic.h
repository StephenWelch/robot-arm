//
// Created by Stephen Welch on 7/18/2020.
//

#ifndef CIRCLE_GRAPHIC_H
#define CIRCLE_GRAPHIC_H

#include <QtWidgets/qgraphicsitem.h>
#include "circle.h"

class CircleGraphic : public QGraphicsEllipseItem {

 public:
	CircleGraphic(double centerX, double centerY, double radius) : QGraphicsEllipseItem(centerX - radius,
																																											centerY - radius,
																																											radius*2,
																																											radius*2) {};

	CircleGraphic(const QPointF &center, double radius) : CircleGraphic(center.x(), center.y(), radius) {};
	CircleGraphic(const Circle &circle) : CircleGraphic(circle.x(), circle.y(), circle.getRadius()) {};

	auto center() const { return this->rect().center(); }
	auto radius() const { return this->rect().width()/2; }
	auto x() const { return this->rect().center().x(); }
	auto y() const { return this->rect().center().y(); }

};

#endif //CIRCLE_GRAPHIC_H
