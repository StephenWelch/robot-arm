//
// Created by Stephen Welch on 6/30/2020.
//

#include "joint_graphic.h"
#include <iostream>

JointGraphic::JointGraphic(double length,
													 double width,
													 Joint *joint,
													 std::mutex *jointMutex,
													 std::unordered_map<Joint *, JointGraphic *> *jointToGraphic,
													 QGraphicsItem *parent)
		: length(length),
			width(width),
			joint(joint),
			jointMutex(jointMutex),
			jointToGraphic(jointToGraphic),
			QGraphicsItem(parent) {
	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemIsMovable);
}

void JointGraphic::updateFromModel() {
	setAngle(joint->getAngle());
	Position adjustedPos = joint->getBasePosition().rotate(-M_PI/2);
	setPos(adjustedPos.x, adjustedPos.y);

	if (joint->getNext()!=nullptr) {
		jointToGraphic->at(joint->getNext())->updateFromModel();
	}
}

QRectF JointGraphic::boundingRect() const {
//  return QRectF(-width / 2, -width / 2, length + width, width);
	return QRectF(-width/2, -length - width/2, width, length + width);
}

void JointGraphic::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	std::scoped_lock lock(*jointMutex);

	QPointF initialPosition = event->lastScenePos() - this->scenePos();
	QPointF finalPosition = event->scenePos() - this->scenePos();
	double angleDelta = qPointFToRadians(finalPosition) - qPointFToRadians(initialPosition);
	double newJointAngle = normalizeAngleRadians(angleDelta + joint->getAngle(joint));
	joint->setAngle(newJointAngle);

	if (holdNextJoints) {
		for (Joint *j = joint->getNext(); j!=nullptr; j = j->getNext()) {
			j->setAngle(normalizeAngleRadians(j->getAngle(j) - angleDelta));
			jointToGraphic->at(j)->updateFromModel();
		}
	} else {
		updateFromModel();
	}
}

void JointGraphic::mousePressEvent(QGraphicsSceneMouseEvent *event) {
//  QGraphicsItem::mousePressEvent(event);

//  if(this->isSelected()) {
//    color = Qt::red;
//  } else {
//    color = Qt::black;
//  }
}

void JointGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	QBrush brush(color);
	QPen pen(brush, 2);
	QPainterPath path;

	painter->setPen(pen);

//  painter->drawPoint(0, 0);
//  painter->drawPoint(length, 0);
//
//  path.moveTo(0, width / 2);
//  path.lineTo(length, width / 2);
//  path.arcTo(length - width / 2, -width / 2, width, width, 270, 180);
//  path.lineTo(0, -width / 2);
//  path.arcTo(-width / 2, -width / 2, width, width, 90, 180);

	painter->drawPoint(0, 0);
	painter->drawPoint(0, -length);

	path.moveTo(-width/2, 0);
	path.lineTo(-width/2, -length);
	path.arcTo(-width/2, -length - width/2, width, width, 180, -180);
	path.lineTo(width/2, 0);
	path.arcTo(-width/2, -width/2, width, width, 0, -180);

	painter->strokePath(path, pen);
//    painter->fillPath(path, brush);
//    painter->drawRect(boundingRect());
}

void JointGraphic::setAngle(double radians) {
	setRotation(qRadiansToDegrees(radians));
}
double JointGraphic::getAngle() const {
	return qDegreesToRadians(this->rotation());
}

double JointGraphic::qPointFToRadians(const QPointF &point) {
	double angle = atan2(point.y(), point.x());
	return angle < 0 ? angle + 2*M_PI : angle;
}

double JointGraphic::normalizeAngleRadians(double radians) {
	while (radians > 2*M_PI) radians -= 2*M_PI;
	while (radians < 0) radians += 2*M_PI;

	return radians;
}

void JointGraphic::setHoldNextJoints(bool holdNextJoints) {
	this->holdNextJoints = holdNextJoints;
}

bool JointGraphic::getHoldNextJoints() const {
	return holdNextJoints;
}
