#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_EDrawCommands.h"
#include <QOpenGLFunctions_4_3_Core.h>
#include <QWidget>
#include <QtOpenGL>

class EDrawCommands : public QGLWidget, QOpenGLFunctions_4_3_Core {
	Q_OBJECT

public:
	EDrawCommands(QWidget *parent = Q_NULLPTR);
	virtual ~EDrawCommands();

	void Init();
	void Display();

protected:
	virtual void initializeGL();
	virtual void resizeGL(int nWidth, int nHeight);
	virtual void paintGL();

private:
	Ui::EDrawCommandsClass ui;
};
