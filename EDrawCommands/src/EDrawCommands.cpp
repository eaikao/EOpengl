#include "EDrawCommands.h"
#include <QThread>
#include "ELoadShader.h"
#include "vmath.h"

//�ĸ������λ��
static const GLfloat g_vertexPositions[] = {
	-1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 1.0f
};

// ÿ���������ɫ
static const GLfloat g_vertexColors[] = {
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f
};

// ��������ֵ, ��Ҫ�����洢�����������Ϣ
static const GLushort g_vertexIndices[] = {
	0, 1, 2
};

GLuint g_ebo[2];
GLuint g_vao[2];
GLuint g_vbo[2];

float aspect = 0.1;

GLint g_modelMatrixLoc;
GLint g_projectionMatrixLoc;

EDrawCommands::EDrawCommands(QWidget *parent)
	: QGLWidget(parent) {
	ui.setupUi(this);
}

EDrawCommands::~EDrawCommands() {

}

void EDrawCommands::Init() {
	initializeOpenGLFunctions();

	EShaderInfo shaders[] = {
			{GL_VERTEX_SHADER, "triangles.vert"},
			{GL_FRAGMENT_SHADER, "triangles.frag"},
			{GL_NONE, NULL}
	};

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	g_modelMatrixLoc = glGetUniformLocation(program, "g_modelMatrixLoc");
	g_projectionMatrixLoc = glGetUniformLocation(program, "g_projectionMatrixLoc");

	//�����������飬���������ݵ�ebo����
	glGenBuffers(1, g_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_vertexIndices), g_vertexIndices, GL_STATIC_DRAW);

	//���ö��㻺�棬�������ɫ�������˻�����
	glGenVertexArrays(1, g_vao);
	glBindVertexArray(g_vao[0]);

	glGenBuffers(1, g_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo[0]);
	//�����Ŷ������ɫ�Ŀռ�
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexPositions) + sizeof(g_vertexColors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_vertexPositions), g_vertexPositions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(g_vertexPositions), sizeof(g_vertexColors), g_vertexColors);

	//һ���ĸ�����
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);	//pos
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(g_vertexPositions));	//color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void EDrawCommands::Display() {
	initializeOpenGLFunctions();
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//����ͶӰ����
	vmath::mat4 projectionMatrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f));
	glUniformMatrix4fv(g_projectionMatrixLoc, 1, GL_FALSE, projectionMatrix);

	//�󶨻���
	glBindVertexArray(g_vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo[0]);

	vmath::mat4 matModelMatrix;
	//����ƽ�ƾ���
	matModelMatrix = vmath::translate(-3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(g_modelMatrixLoc, 1, GL_FALSE, matModelMatrix);
	// DrawArrays����
	glDrawArrays(GL_TRIANGLES, 0, 3);

	matModelMatrix = vmath::translate(-1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(g_modelMatrixLoc, 1, GL_FALSE, matModelMatrix);
	//DrawElements����
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

	matModelMatrix = vmath::translate(1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(g_modelMatrixLoc, 1, GL_FALSE, matModelMatrix);
	//DrawElementsBaseVertex����
	glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

	matModelMatrix = vmath::translate(3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(g_modelMatrixLoc, 1, GL_FALSE, matModelMatrix);
	//DrawArraysInstanced����
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

	glBindVertexArray(0);
}

void EDrawCommands::initializeGL() {
	Init();
}

void EDrawCommands::resizeGL(int nWidth, int nHeight) {
	initializeOpenGLFunctions();
	glViewport(0, 0, nWidth, nHeight);
}

void EDrawCommands::paintGL() {
	Display();
	QThread::msleep(10);
	QMetaObject::invokeMethod(this, "updateGL", Qt::QueuedConnection);
}
