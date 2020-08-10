//
// Created by Stephen Welch on 6/30/2020.
//

#ifndef JOINT_GRAPHIC_H_
#define JOINT_GRAPHIC_H_

#include "joint.h"
#include <iostream>
#include <QTime>
#include <QDrag>
#include <QMimeData>
#include <QBrush>
#include <QtMath>
#include <mutex>

class JointGraphic : public QGraphicsItem {
 private:
	std::unordered_map<Joint *, JointGraphic *> *jointToGraphic;
	Joint *joint;
	std::mutex *jointMutex;
	const double length;
	const double width;
	QColor color = Qt::black;
	bool holdNextJoints = false;

 public:
	JointGraphic(double length,
							 double width,
							 Joint *joint,
							 std::mutex *jointMutex,
							 std::unordered_map<Joint *, JointGraphic *> *jointToGraphic,
							 QGraphicsItem *parent = nullptr);

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

	void updateFromModel();
	void setAngle(double radians);
	double getAngle() const;
	void setHoldNextJoints(bool holdNextJoints);
	bool getHoldNextJoints() const;
	void setColor(QColor color);
	QColor getColor() const;

	static double qPointFToRadians(const QPointF &point);
	static double normalizeAngleRadians(double radians);

};

#endif //ARM_UI__JOINT_GRAPHIC_H_
