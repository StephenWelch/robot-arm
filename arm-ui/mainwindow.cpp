#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "joint.h"
#include "circle_graphic.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtMath>
#include <utility>
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>
#include <QtDebug>
#include <nlohmann/json.hpp>

MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	scene = new QGraphicsScene(this);

	dataSendTimer = new QTimer(this);
	jointMutex = new std::mutex();

	// Setup serial
	port = new QSerialPort(this);
	port->setBaudRate(QSerialPort::Baud115200);
	port->setDataBits(QSerialPort::Data8);
	port->setParity(QSerialPort::NoParity);
	port->setStopBits(QSerialPort::OneStop);
	port->setFlowControl(QSerialPort::NoFlowControl);

	connect(dataSendTimer, &QTimer::timeout, this, &MainWindow::onDataSend);
	connect(port, &QSerialPort::readyRead, this, &MainWindow::onPortReadyRead);
	connect(port, &QSerialPort::errorOccurred, this, &MainWindow::onPortErrorOccurred);

	baseJoint = new Joint(80, 0, 360);
	auto *midJointA = new Joint(80, 0, 360);
	auto *midJointB = new Joint(80, 0, 360);
	auto *endJoint = new Joint(80, 0, 360);

	Joint::link({baseJoint, midJointA, midJointB, endJoint});

	jointRepresentationMap = new std::unordered_map<Joint *, JointGraphic *>{
			{baseJoint,
			 new JointGraphic(80, 40, baseJoint, jointMutex, jointRepresentationMap)},
			{midJointA, new JointGraphic(80, 40, midJointA, jointMutex, jointRepresentationMap)},
			{midJointB, new JointGraphic(80, 40, midJointB, jointMutex, jointRepresentationMap)},
			{endJoint, new JointGraphic(80, 40, endJoint, jointMutex, jointRepresentationMap)}
	};

	for (auto pair : *jointRepresentationMap) {
		// Add to scene
		scene->addItem(pair.second);
	}

	jointRepresentationMap->at(baseJoint)->updateFromModel();

	ui->armView->setTransform(ui->armView->transform().rotateRadians(-M_PI/2));
	ui->armView->setScene(scene);
	ui->armView->show();
	this->setMouseTracking(true);
	this->grabMouse();
	// Grab initial state from GUI
	isSim = ui->modeSelectSim->isChecked();
	// We are initially stopped regardless of whether in sim or real
	running = false;
	firstRun = true;

//	auto *circleB = new Circle(60, 60, 50);
//	auto *circleA = new Circle(0, 0, 100);
//
//	scene->addItem(new CircleGraphic(*circleA));
//	scene->addItem(new CircleGraphic(*circleB));
//
//	for (const auto &intersectionPoint : circleA->getIntersection(*circleB)) {
//		auto pointGraphic = new CircleGraphic(intersectionPoint.x, intersectionPoint.y, 5);
//		pointGraphic->setBrush(Qt::red);
//		scene->addItem(pointGraphic);
//	}

}

MainWindow::~MainWindow() {
	delete ui;
	delete scene;
	delete port;
	delete jointRepresentationMap;
}

void MainWindow::processSerialData() {

//  std::vector<char> dataOut;
//
//  for (auto pair : *jointRepresentationMap) {
//    uint16_t angle = qRadiansToDegrees(pair.first->getAngle(pair.first));
//    unsigned char first = angle & 255u;
//    unsigned char second = (angle & 65280u) << 8u;
//    qDebug() << "Formatting 1: " << QString::number(first, 2);
//    qDebug() << "Formatting 2: " << QString::number(second, 2);
//	dataOut.push_back(first);
//	dataOut.push_back(second);
//
//  }
//
//  if(firstRun) {
//	firstRun = false;
//	port->write({1, (char)(dataOut.size())});
//
//	qDebug() << "Writing outgoing data size...";
//  } else {
//	QByteArray buf = QByteArray(dataOut.data(), sizeof(dataOut.data()));
//	port->write(buf);
////	qDebug() << "Writing angle data: ";
//  }
}

void MainWindow::on_connectionButton_clicked() {
	qDebug() << "Attempting connection...";
	if (!isSim) {
		port->setPortName(ui->comPortTextField->text());
		if (port->open(QSerialPort::ReadWrite)) {
			ui->connectionStatusText->setText("CONNECTED");

			qDebug() << "Serial port connection successful";
		} else {
			ui->connectionStatusText->setText("ERROR");

			qDebug() << "Serial port connection error: " << port->errorString();
		}
	} else {
		qDebug() << "In sim, can't connect";
	}
}

void MainWindow::on_modeSelectReal_clicked() {
	isSim = false;
}

void MainWindow::on_modeSelectSim_clicked() {
	isSim = true;
}

void MainWindow::on_holdAngleField_stateChanged(int state) {
	for (auto pair : *jointRepresentationMap) {
		pair.second->setHoldNextJoints(state);
	}
}

void MainWindow::on_startButton_clicked() {
	running = true;
	dataSendTimer->start(DATA_SEND_INTERVAL_MS);
}

void MainWindow::on_stopButton_clicked() {
	running = false;

	if (port->isOpen()) {
		port->close();

		qDebug() << "Closed serial port";
	}
	dataSendTimer->stop();
}

void MainWindow::onDataSend() {
	std::scoped_lock lock(*jointMutex);
	using json = nlohmann::json;

	std::vector<double> angles;
	for (auto pair : *jointRepresentationMap) {
		double angleDegrees = normalizeAngleDegrees(qRadiansToDegrees(pair.first->getAngle(pair.first)));
		angles.push_back(angleDegrees);
	}
	json doc;
	doc["angles"] = angles;

	port->write("s");
	port->write(doc.dump().c_str());
}

bool MainWindow::getRunning() const {
	return running;
}

void MainWindow::setRunning(bool value) {
	running = value;
}

void MainWindow::onPortReadyRead() {
//  if(port->canReadLine()) {
	qDebug() << "Serial message: " << QString::fromLatin1(port->readLine());
//  }
}

void MainWindow::onPortErrorOccurred(QSerialPort::SerialPortError error) {
	qDebug() << "Serial communications error occurred: " << port->errorString();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
	updateIkFromMouse(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
	updateIkFromMouse(event);
}

void MainWindow::updateIkFromMouse(QMouseEvent *event) {
	// QMouseEvent::button() returns Qt::NoButton when the mouse is moving
	if (event->buttons()==Qt::LeftButton) {
		QPoint remapped = ui->armView->mapFromParent(event->pos());
		if (ui->armView->rect().contains(remapped)) {
			QPointF targetPoint = ui->armView->mapToScene(remapped);
			Position targetPos = Position(targetPoint.x(), targetPoint.y());
//			double targetAngle = M_PI / 2;
//			calculateIk(targetPos, &targetAngle, baseJoint->getEnd());
			calculateIk(targetPos, nullptr, nullptr);

		}
	}
}

/*
 * TODO:
 * - Point end joint towards target point when there is no target angle
 * - Handle the case where two circles don't intersect
 * - Handle the case where joints have min/max angles
 */
void MainWindow::calculateIk(const Position &targetPos, const double *targetAngle, Joint *terminatingJoint) {
	std::scoped_lock lock(*jointMutex);

	clearDebugGraphics();

	Position baseBasePosition = baseJoint->getBasePositionRelativeToOrigin();
	Position endBasePosition = targetPos;
	if (targetAngle!=nullptr && terminatingJoint!=nullptr) {
		endBasePosition = targetPos - (Position(*targetAngle)*terminatingJoint->getLength());
	}

	// Sum up arm lengths
	// Take into account that end joint has set angle
	double totalArmLength = 0;
	for (Joint *current = baseJoint; current!=terminatingJoint; current = current->getNext()) {
		totalArmLength += current->getLength();
	}

	// Check if target point is in range
	if ((endBasePosition - baseBasePosition).getLength() > totalArmLength) {
		return;
	}

	std::vector<Joint *> joints = baseJoint->getConsecutiveJoints();
	Joint *vertexJoint = joints[joints.size()/2];
	double lengthA = 0;
	double lengthB = 0;

	for (Joint *current = joints[0]; current!=vertexJoint; current = current->getNext()) {
		lengthA += current->getLength();
	}
	for (Joint *current = vertexJoint; current!=terminatingJoint; current = current->getNext()) {
		lengthB += current->getLength();
	}

	// Calculate circle intersection
	Circle circleA = Circle(baseJoint->getBasePosition(baseJoint), lengthA);
	Circle circleB = Circle(endBasePosition, lengthB);
	std::vector<Position> intersections = circleA.getIntersection(circleB);

	// Calculate new angles
//			for(const auto &intersection : intersections) {
	if (!intersections.empty()) {
		const auto &intersection = intersections[0];
		baseJoint->setAngle((intersection - baseBasePosition).getAngle(), baseJoint);
		vertexJoint->setAngle((endBasePosition - intersection).getAngle(), baseJoint);
		if (targetAngle!=nullptr && terminatingJoint!=nullptr) {
			terminatingJoint->setAngle(*targetAngle, baseJoint);
		} else {
			baseJoint->getEnd()->setAngle(0);
		}
	}
//			}

	// Add graphics elements
	auto qTargetPos = QPointF(targetPos.x, targetPos.y);
	auto targetVector = new QGraphicsLineItem(QLineF(qTargetPos, qTargetPos + QPointF(0, 30)));
	auto rangeGraphic = new CircleGraphic(Circle(baseJoint->getBasePosition(baseJoint), totalArmLength));
	targetVector->setPen(QPen(Qt::green));
	rangeGraphic->setPen(QPen(Qt::blue));

	ikDebugGraphics.push_back(targetVector);
	ikDebugGraphics.push_back(rangeGraphic);
	ikDebugGraphics.push_back(new CircleGraphic(circleA));
	ikDebugGraphics.push_back(new CircleGraphic(circleB));

	for (const auto &position : intersections) {
		auto intersectionToBase =
				new QGraphicsLineItem(QLineF(positionToQPointF(position), positionToQPointF(baseBasePosition)));
		auto
				intersectionToEnd =
				new QGraphicsLineItem(QLineF(positionToQPointF(position), positionToQPointF(endBasePosition)));
		intersectionToBase->setPen(QPen(Qt::red));
		intersectionToEnd->setPen(QPen(Qt::red));
		ikDebugGraphics.push_back(intersectionToBase);
		ikDebugGraphics.push_back(intersectionToEnd);
	}

	jointRepresentationMap->at(baseJoint)->updateFromModel();
	for (const auto &item : ikDebugGraphics) {
		scene->addItem(item);
	}

}

void MainWindow::calculateIk(const Position &targetPos) {
	calculateIk(targetPos, nullptr, nullptr);
}

double MainWindow::normalizeAngleDegrees(double degrees) const {
	while (degrees < 0) degrees += 360;
	while (degrees > 360) degrees -= 360;
	return degrees;
}
void MainWindow::clearDebugGraphics() {
	// Clear old debug graphics
	for (const auto &item : ikDebugGraphics) {
		scene->removeItem(item);
	}
	ikDebugGraphics.clear();
}

QPointF MainWindow::positionToQPointF(const Position &position) const {
	return QPointF(position.x, position.y);
}
