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

	auto *baseJoint = new Joint(80, 0, 360);
	auto *midJointA = new Joint(80, 0, 360);
	auto *midJointB = new Joint(80, 0, 360);
	auto *endJoint = new Joint(80, 0, 360);

	Joint::link({baseJoint, midJointA, midJointB, endJoint});

	jointRepresentationMap = new std::unordered_map<Joint *, JointGraphic *>();
	jointRepresentationMap->insert({
																		 {baseJoint,
																			new JointGraphic(80, 40, baseJoint, jointMutex, jointRepresentationMap)},
																		 {midJointA, new JointGraphic(80, 40, midJointA, jointMutex, jointRepresentationMap)},
																		 {midJointB, new JointGraphic(80, 40, midJointB, jointMutex, jointRepresentationMap)},
																		 {endJoint, new JointGraphic(80, 40, endJoint, jointMutex, jointRepresentationMap)}
																 });

	for (auto pair : *jointRepresentationMap) {
		// Add to scene
		scene->addItem(pair.second);
	}
	jointRepresentationMap->at(baseJoint)->updateFromModel();

	ui->armView->setScene(scene);
	ui->armView->show();

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
		angles.push_back(qRadiansToDegrees(pair.first->getAngle(pair.first)));
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
	qDebug() << "Serial communications error occurred";
}
