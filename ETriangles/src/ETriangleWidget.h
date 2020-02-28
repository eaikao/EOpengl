#pragma once

#include <QWidget>
#include <QtOpenGL>
#include "ui_ETriangleWidget.h"
#include <QOpenGLFunctions_4_3_Core.h>
#include "ETriangles.h"

class ETriangleWidget : public QGLWidget {
	Q_OBJECT

public:
	ETriangleWidget(QWidget *parent = Q_NULLPTR);
	virtual ~ETriangleWidget();

protected:
	virtual void initializeGL();
	virtual void resizeGL(int nWidth, int nHeight);
	virtual void paintGL();

private:
	Ui::ETriangleWidget ui;
	ETriangles *m_pTriangles;
};
