#include "ETriangleWidget.h"
#include <qopengl.h>
#include <qopenglext.h>
#include <QThread>
#include "ELoadShader.h"

ETriangleWidget::ETriangleWidget(QWidget *parent)
	: QGLWidget(parent)
	, m_pTriangles(new ETriangles()) {
	ui.setupUi(this);
}

ETriangleWidget::~ETriangleWidget() {
	delete m_pTriangles;
}

void ETriangleWidget::initializeGL() {
	//m_pTriangles->InitOne();
	m_pTriangles->InitTwo();
	//m_pTriangles->InitThree();
	//m_pTriangles->InitFour();
	//m_pTriangles->InitFive();
	//m_pTriangles->InitSix();
	//m_pTriangles->InitSeven();
	//m_pTriangles->InitEight();
	//m_pTriangles->InitNine();
	//m_pTriangles->InitTen();
	//m_pTriangles->InitEleven();

	//m_pTriangles->InitGl();
	//m_pTriangles->InitThirteen();

	//m_pTriangles->InitFourteen();
	//m_pTriangles->InitFifteen();
}

void ETriangleWidget::resizeGL(int nWidth, int nHeight) {
	m_pTriangles->initializeOpenGLFunctions();
	m_pTriangles->glViewport(0, 0, nWidth, nHeight);
}

void ETriangleWidget::paintGL() {
	//m_pTriangles->DisplayOne();
	m_pTriangles->DisplayTwo();
	//m_pTriangles->DisplayThree();
	//m_pTriangles->DisplayFour();
	//m_pTriangles->DisplayFive();
	//m_pTriangles->DisplaySix();
	//m_pTriangles->DisplaySeven();
	//m_pTriangles->DisplayEight();
	//m_pTriangles->DisplayNine();
	//m_pTriangles->DisplayTen();
	//m_pTriangles->DisplayEleven();
	//m_pTriangles->DisplayThirteen();
	//m_pTriangles->DisplayFourteen();
	//m_pTriangles->DisplayFifteen();
	QThread::msleep(500);
	QMetaObject::invokeMethod(this, "updateGL", Qt::QueuedConnection);
}
