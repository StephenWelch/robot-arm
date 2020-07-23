#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QMouseEvent>
#include <unordered_map>
#include <QTimer>
#include <mutex>
#include "joint.h"
#include "joint_graphic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
 Q_OBJECT

 public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	bool getRunning() const;
	void setRunning(bool value);

 private slots:
	void on_modeSelectReal_clicked();

	void on_modeSelectSim_clicked();

	void on_startButton_clicked();

	void on_stopButton_clicked();

	void on_connectionButton_clicked();

	void onPortReadyRead();

	void onPortErrorOccurred(QSerialPort::SerialPortError error);

	void onDataSend();

	void on_holdAngleField_stateChanged(int arg1);

	void mousePressEvent(QMouseEvent *event) override;

 private:
	const int SERIAL_TIMEOUT_MS = 3000;
	const int DATA_SEND_INTERVAL_MS = 50;

	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QSerialPort *port;
	QTimer *dataSendTimer;
	std::mutex *jointMutex;
	bool isSim;
	bool running;
	bool firstRun;

	Joint *baseJoint;
	std::unordered_map<Joint *, JointGraphic *> *jointRepresentationMap;
	std::vector<QGraphicsItem*> ikDebugGraphics;

	void processSerialData();
	void calculateIk(const Position &targetPos, double targetAngle);
	void calculateIk(const Position &targetPos);
	double normalizeAngleDegrees(double degrees) const;
};
#endif // MAINWINDOW_H
