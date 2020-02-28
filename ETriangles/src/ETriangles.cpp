#include "ETriangles.h"
#include <qopengl.h>
#include <qopenglext.h>
#include <QTime>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ELoadShader.h"

GLuint gScaleLocation;
GLuint gFragColor;
GLuint gTransform;

GLuint gModel;
GLuint gView;
GLuint gProjection;

GLuint gTextureId;
GLuint pboIds[2];
const int    IMAGE_WIDTH = 512;
const int    IMAGE_HEIGHT = 512;
const int    CHANNEL_COUNT = 3;
const int    DATA_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * CHANNEL_COUNT;

ETriangles::ETriangles() {

}

ETriangles::~ETriangles() {
	glDeleteTextures(1, &gTextureId);
	glDeleteBuffers(2, pboIds);
}

//������������ɵľ���
void ETriangles::InitOne() {
	initializeOpenGLFunctions();

	//����һ������������󣬶����������洢���Ƕ���Ĳ���״̬�����ԣ���һ�����϶���
	//VAO��������state-object��״̬����,��¼����һ�λ�������Ҫ����Ϣ�������������ģ����ݸ�ʽ֮�����Ϣ���������õ�״̬����洢��vao��
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	// ��ʼ���������黺��
	GLfloat vertices[NumVertices][2] = {
	{ -0.90, - 0.90},
	{ 0.85, -0.90},
	{ -0.90, 0.85},
	{ 0.90, -0.85 },
	{ 0.90, 0.90 },
	{ -0.85, 0.90 }
	};

	//VBO��> ���㻺�����
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	//���ƽ��ͻ���(cpu)�����ݿ�������������(gpu)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles1.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles1.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	//���ö���������indexλ�ÿ��Է��ʵ�����ֵ��vertices����NumVertices�����㣬ÿ������������Ԫ�أ����Եڶ���ʵ��Ϊ2�����������õ�״̬�ᱣ�浽VAO������
	//���ĸ�������ʾһ������Ĳ��������������ֵ�ǽ������ŵģ���ô���ǿ���ʹ��0��ʾ������Ҳ������sizeof(float) * 2,��5��������ʾ���ǣ���������ʼλ�õ�ƫ����(Offset)
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
}

void ETriangles::DisplayOne() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glFlush();
}

void ETriangles::InitTwo() {
	initializeOpenGLFunctions();
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//һ��������
	GLfloat vertices[3][2] = {
	{ -0.85, -0.85 },
	{ 0.85, -0.85 },
	{ 0.00, 0.90 }
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles2.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles2.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	// ��ѯ��ȡһ�±�����λ��
	gScaleLocation = glGetUniformLocation(program, "gScale");
	printf("gScaleLocation = %d\n", gScaleLocation);

	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
}

void ETriangles::DisplayTwo() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//���ý����С
	static float Scale = 0.0f;
	Scale += 0.01f;
	glUniform1f(gScaleLocation, std::sinf(Scale));

	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glFlush();
}

void ETriangles::InitThree() {
	initializeOpenGLFunctions();

	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	// ʹ��ebo���ƾ���
	GLfloat vertices[4][2] = {
	{ -0.5, 0.5 },		//���Ͻ�
	{ 0.5, 0.5 },		//���Ͻ�
	{ 0.5, -0.5 },		//���½�
	{-0.5, -0.5}		//���½�
	};

	//�������������ε�˳��
	GLuint indices[] = {
	0, 1, 2,
	2, 3, 0
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles3.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles3.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	// ��ѯ��ȡһ�±�����λ��
	gScaleLocation = glGetUniformLocation(program, "gScale");
	printf("gScaleLocation = %d\n", gScaleLocation);

	gFragColor = glGetUniformLocation(program, "gColor");
	printf("gFragColor = %d\n", gFragColor);

	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));
	//���߿�ģʽ����
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableVertexAttribArray(vPosition);
}

void ETriangles::DisplayThree() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//���ý����С
	static float Scale = 0.0f;
	Scale += 0.01f;
	glUniform1f(gScaleLocation, std::sinf(Scale));
	//���ý�����ɫ
	static float fColor = 0.0f;
	fColor += 0.01f;
	glUniform4f(gFragColor, std::sinf(fColor), 0.0f, std::sinf(fColor), 1.0f);

	glBindVertexArray(VAOs[Triangles]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glFlush();
}

void ETriangles::InitFour() {
	initializeOpenGLFunctions();

	//����һ������������󣬶����������洢���Ƕ���Ĳ���״̬�����ԣ���һ�����϶���
	//VAO��������state-object��״̬����,��¼����һ�λ�������Ҫ����Ϣ�������������ģ����ݸ�ʽ֮�����Ϣ���������õ�״̬����洢��vao��
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	/*����λ�ú���ɫ��һ������*/
	GLfloat vertices[] = {
		// λ��              // ��ɫ
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // ����
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // ����
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // ����
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles4.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles4.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6/*0*/, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6/*0*/, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(1);
}

void ETriangles::DisplayFour() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glFlush();
}

void ETriangles::InitFive() {
	initializeOpenGLFunctions();

	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//�����ε�λ�ú���������,���������ԭ�������½ǣ�ͼ�����ݵ�����ԭ�������Ͻǣ������������ʱ���������������ϵͳ��ת�����ᵼ��ͼ��ʱ���ġ�
	//��������ԭ�����м�
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,// bottom left
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
		0.0f, 1.0f, 0.0f, 0.5f, 0.0f
	};
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//λ��
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
	//����
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//����һ���������
	GLuint TextureWall;
	glGenTextures(1, &TextureWall);
	glBindTexture(GL_TEXTURE_2D, TextureWall);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float borderColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles5.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles5.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg
	cv::Mat textureImage = cv::imread("resource/wall.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		//����rgb���룬����bgr�������ʾ�ź�ԭͼһ��
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glUniform1i(glGetUniformLocation(program, "textureWall"), 0);
}

void ETriangles::DisplayFive() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glFlush();
}

void ETriangles::InitSix() {
	initializeOpenGLFunctions();

	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//�������ꡢ��ɫֵ����������
	//��������ԭ�����м�
	GLfloat vertices[] = {
		// position				// colors               // texture coords
		-0.3f, 0.7f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,	// top left
		0.7f,  0.7f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,	// top right
		0.7f, -0.3f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,	// bootom right
		-0.3f,-0.3f, 0.0f,		1.0f, 1.0f, 0.0f,		0.0f, 0.0f	// bootom left
	};
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//����˳��
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//λ��
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//��ɫ
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(vColor);

	//����
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoord);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles6.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles6.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//����һ�������������
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//����һ��Ц���������
	GLuint TextureAwesomeface;
	glGenTextures(1, &TextureAwesomeface);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureAwesomeface);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg
	cv::Mat textureFaceImage = cv::imread("resource/awesomeface.png");
	cv::Mat textureFaceImage1;
	cv::flip(textureFaceImage, textureFaceImage1, 0);
	unsigned char* pFaceTextureData = textureFaceImage1.data;
	int nFaceWidth = textureFaceImage1.rows;
	int nFaceHeight = textureFaceImage1.cols;
	if (pFaceTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nFaceWidth, nFaceHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pFaceTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glUniform1i(glGetUniformLocation(program, "TextureCntainer"), 0);
	glUniform1i(glGetUniformLocation(program, "TextureAwesomeface"), 1);
}

void ETriangles::DisplaySix() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBindVertexArray(VAOs[Triangles]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glFlush();
}

void ETriangles::InitSeven() {
	initializeOpenGLFunctions();

	//�󶨶����������
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//������Ϣ
	GLfloat vertices[] = {
		// position				// colors               // texture coords
		-0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,	// top left
		0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,	// top right
		0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,	// bootom right
		-0.5f,-0.5f, 0.0f,		1.0f, 1.0f, 0.0f,		0.0f, 0.0f	// bootom left
	};
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//����˳��
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//λ��
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//��ɫ
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(vColor);

	//����
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoord);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles7.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles7.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//����һ�������������
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	//cv::Mat textureContainerImage;
	//cv::flip(textureImage, textureContainerImage, 0);
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glUniform1i(glGetUniformLocation(program, "TextureCntainer"), 0);
}

void ETriangles::DisplaySeven() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAOs[Triangles]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glFlush();
}

void ETriangles::InitEight() {
	initializeOpenGLFunctions();

	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//�������ꡢ��ɫֵ����������
	//��������ԭ�����м�
	GLfloat vertices[] = {
		// position				// texture    
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,	// top left
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,	// top right
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,	// bootom right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f	// bootom left
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//����˳��
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//λ��
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//����
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles8.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles8.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//����һ�������������
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg, opencv����bgr��ȡ
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//����һ��Ц���������
	GLuint TextureAwesomeface;
	glGenTextures(1, &TextureAwesomeface);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureAwesomeface);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg
	cv::Mat textureFaceImage = cv::imread("resource/awesomeface.png");
	cv::Mat textureFaceImage1;
	cv::flip(textureFaceImage, textureFaceImage1, 0);
	unsigned char* pFaceTextureData = textureFaceImage1.data;
	int nFaceWidth = textureFaceImage1.rows;
	int nFaceHeight = textureFaceImage1.cols;
	if (pFaceTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nFaceWidth, nFaceHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pFaceTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	gTransform = glGetUniformLocation(program, "transform");
	glUniform1i(glGetUniformLocation(program, "TextureCntainer"), 0);
	glUniform1i(glGetUniformLocation(program, "TextureAwesomeface"), 1);
}

void ETriangles::DisplayEight() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//����0.5���ľ���
	float fZoomTrans[][4] = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	// ��ͨ��
	cv::Mat matZoomtransform = cv::Mat::zeros(4, 4, CV_32F);
	for (int i = 0; i < matZoomtransform.rows; ++i) {
		for (int j = 0; j < matZoomtransform.cols; ++j) {
			matZoomtransform.at<float>(i, j) = fZoomTrans[i][j];
		}
	}

	float fCurTime = (float)QTime::currentTime().second();
	float fCosTime = std::cos(fCurTime);
	float fSinTime = std::sin(fCurTime);
	//��z��(0,0,1)��ת����
	float fRotateTrans[][4] = {
		fCosTime, fSinTime, 0.0f, 0.0f,
		-fSinTime, fCosTime, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	cv::Mat matRotatetransform = cv::Mat::zeros(4, 4, CV_32F);
	for (int i = 0; i < matRotatetransform.rows; ++i) {
		for (int j = 0; j < matRotatetransform.cols; ++j) {
			matRotatetransform.at<float>(i, j) = fRotateTrans[i][j];
		}
	}

	//λ�ƾ���
	float fMoveTrans[][4] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 1.0f
	};
	cv::Mat matMovetransform = cv::Mat::zeros(4, 4, CV_32F);
	for (int i = 0; i < matMovetransform.rows; ++i) {
		for (int j = 0; j < matMovetransform.cols; ++j) {
			matMovetransform.at<float>(i, j) = fMoveTrans[i][j];
		}
	}

	cv::Mat transform = matMovetransform * matZoomtransform * matRotatetransform;
	float fTransform[][4] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	for (int i = 0; i < transform.rows; ++i) {
		for (int j = 0; j < transform.cols; ++j) {
			fTransform[i][j] = transform.at<float>(i, j);
		}
	}

	// ��shader��ɫ���У��Ὣ�任������б�Ϊ�У������������ʱ���б�ʾ������ɫ���е��У�������4��4�г���4��һ��
	glUniformMatrix4fv(gTransform, 1, GL_FALSE, fTransform[0]);

	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	glBindVertexArray(VAOs[Triangles]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glFlush();
}

void ETriangles::InitNine() {
	initializeOpenGLFunctions();

	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//�������ꡢ��ɫֵ����������
	//��������ԭ�����м�
	GLfloat vertices[] = {
		// position				// colors    
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	// top left
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f,	// top right
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,	// bootom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f	// bootom left
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//����˳��
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//λ��
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//����
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles9.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles9.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//����һ�������������
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg, opencv����bgr��ȡ
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//����һ��Ц���������
	GLuint TextureAwesomeface;
	glGenTextures(1, &TextureAwesomeface);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureAwesomeface);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg
	cv::Mat textureFaceImage = cv::imread("resource/awesomeface.png");
	cv::Mat textureFaceImage1;
	cv::flip(textureFaceImage, textureFaceImage1, 0);
	unsigned char* pFaceTextureData = textureFaceImage1.data;
	int nFaceWidth = textureFaceImage1.rows;
	int nFaceHeight = textureFaceImage1.cols;
	if (pFaceTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nFaceWidth, nFaceHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pFaceTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	gModel = glGetUniformLocation(program, "model");
	gView = glGetUniformLocation(program, "view");
	gProjection = glGetUniformLocation(program, "projection");
	glUniform1i(glGetUniformLocation(program, "TextureCntainer"), 0);
	glUniform1i(glGetUniformLocation(program, "TextureAwesomeface"), 1);
}

void ETriangles::DisplayNine() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 fModel = glm::mat4(1.0f);
	//��x��˳ʱ����ת55��
	fModel = glm::rotate(fModel, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 fView = glm::mat4(1.0f);
	//����z������ƶ�3����λ��ת��Ϊ�۲�ռ䣬ʹ�Լ���������������ȥ�۲��ͼ��
	fView = glm::translate(fView, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 fProjection = glm::mat4(1.0f);
	//������꣬͸��ͶӰ
	fProjection = glm::perspective(glm::radians(45.0f), (float)(800.0f / 600.0f), 0.1f, 100.0f);

	glUniformMatrix4fv(gModel, 1, GL_FALSE, glm::value_ptr(fModel));
	glUniformMatrix4fv(gView, 1, GL_FALSE, glm::value_ptr(fView));
	glUniformMatrix4fv(gProjection, 1, GL_FALSE, glm::value_ptr(fProjection));

	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	glBindVertexArray(VAOs[Triangles]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glFlush();
}

void ETriangles::InitTen() {
	initializeOpenGLFunctions();

	//������Ȳ���
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//3D�������꣬1���������������������Ҫ6���㣬6����һ��36����
	GLfloat vertices[] = {
		// position				// colors    
		-0.5f, -0.5f,  -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,  -0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,  -0.5f,	1.0f, 1.0f,
		 0.5f,  0.5f,  -0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,
		 0.5f, -0.5f, 0.5f,		1.0f, 0.0f,
		 0.5f,  0.5f, 0.5f,		1.0f, 1.0f,
		 0.5f,  0.5f, 0.5f,		1.0f, 1.0f,
		-0.5f,  0.5f, 0.5f,		0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,		1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,		0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		0.5f,  -0.5f, -0.5f,	1.0f, 1.0f,
		0.5f,  -0.5f,  0.5f,	1.0f, 0.0f,
		0.5f,  -0.5f,  0.5f,	1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,
		 0.5f, 0.5f, -0.5f,		1.0f, 1.0f,
		 0.5f, 0.5f,  0.5f,		1.0f, 0.0f,
		 0.5f, 0.5f,  0.5f,		1.0f, 0.0f,
		-0.5f, 0.5f,  0.5f,		0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	//λ��
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//����
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles10.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles10.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//����һ�������������
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg, opencv����bgr��ȡ
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//����һ��Ц���������
	GLuint TextureAwesomeface;
	glGenTextures(1, &TextureAwesomeface);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureAwesomeface);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬ����һ������
	//awesomeface.png, wall.jpg
	cv::Mat textureFaceImage = cv::imread("resource/awesomeface.png");
	cv::Mat textureFaceImage1;
	cv::flip(textureFaceImage, textureFaceImage1, 0);
	unsigned char* pFaceTextureData = textureFaceImage1.data;
	int nFaceWidth = textureFaceImage1.rows;
	int nFaceHeight = textureFaceImage1.cols;
	if (pFaceTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nFaceWidth, nFaceHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pFaceTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	gModel = glGetUniformLocation(program, "model");
	gView = glGetUniformLocation(program, "view");
	gProjection = glGetUniformLocation(program, "projection");
	glUniform1i(glGetUniformLocation(program, "TextureCntainer"), 0);
	glUniform1i(glGetUniformLocation(program, "TextureAwesomeface"), 1);
}

void ETriangles::DisplayTen() {
	initializeOpenGLFunctions();
	//glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //�����Ȼ���

	glm::mat4 fModel = glm::mat4(1.0f);
	//��60��������ʱ����ת�Ƕ�
	float fCurTime = (float)QTime::currentTime().second();
	fModel = glm::rotate(fModel, (float)fCurTime, glm::vec3(0.5f, 1.0f, 0.0f));

	glm::mat4 fView = glm::mat4(1.0f);
	//����z������ƶ�3����λ��ת��Ϊ�۲�ռ䣬ʹ�Լ���������������ȥ�۲��ͼ��
	fView = glm::translate(fView, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 fProjection = glm::mat4(1.0f);
	//������꣬͸��ͶӰ
	fProjection = glm::perspective(glm::radians(45.0f), (float)(800.0f / 600.0f), 0.1f, 100.0f);

	glUniformMatrix4fv(gModel, 1, GL_FALSE, glm::value_ptr(fModel));
	glUniformMatrix4fv(gView, 1, GL_FALSE, glm::value_ptr(fView));
	glUniformMatrix4fv(gProjection, 1, GL_FALSE, glm::value_ptr(fProjection));

	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glFlush();
}

void ETriangles::InitEleven() {
	InitTen();
}

void ETriangles::DisplayEleven() {
	initializeOpenGLFunctions();
	//glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //�����Ȼ���

	glm::mat4 fView = glm::mat4(1.0f);
	//����z������ƶ�3����λ��ת��Ϊ�۲�ռ䣬ʹ�Լ���������������ȥ�۲��ͼ��
	fView = glm::translate(fView, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 fProjection = glm::mat4(1.0f);
	//������꣬͸��ͶӰ
	fProjection = glm::perspective(glm::radians(45.0f), (float)(800.0f / 600.0f), 0.1f, 100.0f);
	glUniformMatrix4fv(gView, 1, GL_FALSE, glm::value_ptr(fView));
	glUniformMatrix4fv(gProjection, 1, GL_FALSE, glm::value_ptr(fProjection));

	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	//glBindVertexArray(VAOs[Triangles]);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	for (unsigned int i = 0; i < 10; i++)
	{
		// ���ݲ�ͬ�ı任����ת����ǰ�����壬���ƶ�����ת,��ת��������������
		glm::mat4 fModel = glm::mat4(1.0f);
		fModel = glm::translate(fModel, cubePositions[i]);
		float angle = 20.0f * i;
		fModel = glm::rotate(fModel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		glUniformMatrix4fv(gModel, 1, GL_FALSE, glm::value_ptr(fModel));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glFlush();
}

void ETriangles::InitThirteen() {
	//initializeOpenGLFunctions();
	//m_pImageData = new GLubyte[DATA_SIZE];
	//memset(m_pImageData, 255, DATA_SIZE);
	//m_pboMode = 0;

	//glGenTextures(1, &gTextureId);
	//glBindTexture(GL_TEXTURE_2D, gTextureId);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)m_pImageData);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glGenBuffers(2, pboIds);
	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
	//glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[1]);
	//glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}


void ETriangles::DisplayThirteen() {
	initializeOpenGLFunctions();
	static bool bInit = false;
	if (!bInit) {
		m_pImageData = new GLubyte[DATA_SIZE];
		memset(m_pImageData, 255, DATA_SIZE);
		m_pboMode = 0;

		glGenTextures(1, &gTextureId);
		glBindTexture(GL_TEXTURE_2D, gTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)m_pImageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenBuffers(2, pboIds);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[1]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		bInit = true;
	}
	static int nIndex = 0;
	int nNextIndex = 0;

	if (m_pboMode > 0) {
		if (m_pboMode == 1) {
			nIndex = 0;
			nNextIndex = 0;
		}
		else if (m_pboMode == 2) {
			nIndex = (nIndex + 1) % 2;
			nNextIndex = (nIndex + 1) % 2;
		}

		glBindTexture(GL_TEXTURE_2D, gTextureId);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nIndex]);

		//����pbo�е����ݵ��������
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, 0);

		// ����pbo��������
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nNextIndex]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (ptr != NULL) {
			UpdatePixels(ptr, DATA_SIZE);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	else {
		UpdatePixels(m_pImageData, DATA_SIZE);
		glBindTexture(GL_TEXTURE_2D, gTextureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)m_pImageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glPushMatrix();			//���޸�ģ�;���֮ǰ����֮ǰ��ģ�;���

	float cameraAngleX = 0;
	float cameraAngleY = 0;
	float cameraDistance = 2.0f;;
	glTranslatef(0, 0, -cameraDistance);
	glRotatef(cameraAngleX, 1, 0, 0);
	glRotatef(cameraAngleY, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, gTextureId);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0, 1.0f, 0.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	glFlush();
}

void ETriangles::InitGl() {
	initializeOpenGLFunctions();
	glShadeModel(GL_FLAT);                 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);


	glClearColor(0, 0, 0, 0);                   // background color
	glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);
}

void ETriangles::ChangePboMode(int nMode) {
	m_pboMode = nMode;
}

void ETriangles::UpdatePixels(GLubyte* pDst, int nSize) {
	static int color = 0;

	if (pDst == NULL)
		return;

	int* ptr = (int*)pDst;

	// copy 4 bytes at once
	for (int i = 0; i < IMAGE_HEIGHT; ++i)
	{
		for (int j = 0; j < IMAGE_WIDTH; ++j)
		{
			*ptr = color;
			++ptr;
		}
		color += 257;
	}
	++color;            // scroll down
}



void ETriangles::InitFourteen() {
	initializeOpenGLFunctions();

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);

	//�󶨶����������
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//������Ϣ
	GLfloat vertices[] = {
		// position				// colors               // texture coords
		-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// top left
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,	// top right
		1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// bootom right
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f	// bootom left
	};
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//����˳��
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//λ��
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//��ɫ
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(vColor);

	//����
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoord);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles7.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles7.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//����һ�������������
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//��ʼ�����ݵ�������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
	glUniform1i(glGetUniformLocation(program, "TextureCntainer"), 0);
}

void ETriangles::DisplayFourteen() {

	static bool nIndex = true;
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, 1);
	if (nIndex) {
		cv::Mat textureImage = cv::imread("resource/container.jpg");
		//cv::Mat textureContainerImage;
		//cv::flip(textureImage, textureContainerImage, 0);
		unsigned char* pTextureData = textureImage.data;
		int nWidth = textureImage.rows;
		int nHeight = textureImage.cols;
		if (pTextureData != NULL) {
			double dStratTime = static_cast<double>(cv::getTickCount());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 256, 256, nWidth, nHeight, GL_BGR, GL_UNSIGNED_BYTE, (GLvoid*)pTextureData);
			//glGenerateMipmap(GL_TEXTURE_2D);
			double dTime = ((double)cv::getTickCount() - dStratTime) / cv::getTickFrequency();
			qDebug("operation hours: %f s", dTime);
		}
	}
	else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		cv::Mat textureFaceImage = cv::imread("resource/wall.jpg");
		cv::Mat textureFaceImage1;
		cv::flip(textureFaceImage, textureFaceImage1, 0);
		unsigned char* pFaceTextureData = textureFaceImage1.data;
		int nFaceWidth = textureFaceImage1.rows;
		int nFaceHeight = textureFaceImage1.cols;
		if (pFaceTextureData != NULL) {
			double dStratTime = static_cast<double>(cv::getTickCount());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 256, 256, nFaceWidth, nFaceWidth, GL_BGR, GL_UNSIGNED_BYTE, (GLvoid*)pFaceTextureData);
			//glGenerateMipmap(GL_TEXTURE_2D);
			double dTime = ((double)cv::getTickCount() - dStratTime) / cv::getTickFrequency();
			qDebug("operation hours: %f s", dTime);
		}
	}
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glFlush();
	glBindTexture(GL_TEXTURE_2D, 0);
	nIndex = !nIndex;
}

void ETriangles::InitFifteen() {
	initializeOpenGLFunctions();

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);

	//�󶨶����������
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//������Ϣ
	GLfloat vertices[] = {
		// position				// colors               // texture coords
		-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// top left
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,	// top right
		1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// bootom right
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f	// bootom left
	};
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//����˳��
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//λ��
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//��ɫ
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(vColor);

	//����
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoord);

	// ���ض���(VERTEX)��ɫ����ƬԪ(FRAGMENT)��ɫ��
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles7.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles7.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//����һ�������������
	glGenTextures(1, &gTextureId);
	glBindTexture(GL_TEXTURE_2D, gTextureId);

	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//��ʼ�����ݵ�������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);

	glGenBuffers(2, pboIds);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void ETriangles::DisplayFifteen() {
	static bool nIndex = true;
	static GLvoid* pImage = NULL;
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glBindTexture(GL_TEXTURE_2D, 0);
	if (nIndex) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);

		cv::Mat textureImage = cv::imread("resource/container.jpg");
		unsigned char* pTextureData = textureImage.data;
		int nWidth = textureImage.rows;
		int nHeight = textureImage.cols;

		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (pTextureData != NULL) {
			memcpy(ptr, pTextureData, nWidth * nHeight * 3);
		}
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		double dStratTime = static_cast<double>(cv::getTickCount());
		glBindTexture(GL_TEXTURE_2D, gTextureId);
		//����pbo�е����ݵ��������
		glTexSubImage2D(GL_TEXTURE_2D, 0, 256, 256, nWidth, nHeight, GL_BGR, GL_UNSIGNED_BYTE, 0);
		double dTime = ((double)cv::getTickCount() - dStratTime) / cv::getTickFrequency();
		qDebug("operation hours: %f s", dTime);
	}
	else {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[1]);

		cv::Mat textureFaceImage = cv::imread("resource/wall.jpg");
		cv::Mat textureFaceImage1;
		cv::flip(textureFaceImage, textureFaceImage1, 0);
		unsigned char* pFaceTextureData = textureFaceImage1.data;
		int nFaceWidth = textureFaceImage1.rows;
		int nFaceHeight = textureFaceImage1.cols;

		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (pFaceTextureData != NULL) {
			memcpy(ptr, pFaceTextureData, nFaceWidth * nFaceHeight * 3);
		}
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		double dStratTime = static_cast<double>(cv::getTickCount());
		glBindTexture(GL_TEXTURE_2D, gTextureId);
		//����pbo�е����ݵ��������
		glTexSubImage2D(GL_TEXTURE_2D, 0, 256, 256, nFaceWidth, nFaceHeight, GL_BGR, GL_UNSIGNED_BYTE, 0);
		double dTime = ((double)cv::getTickCount() - dStratTime) / cv::getTickFrequency();
		qDebug("operation hours: %f s", dTime);

		//cv::Mat textureFaceImage = cv::imread("resource/wall.jpg");
		//cv::Mat textureFaceImage1;
		//cv::flip(textureFaceImage, textureFaceImage1, 0);
		//unsigned char* pFaceTextureData = textureFaceImage1.data;
		//int nFaceWidth = textureFaceImage1.rows;
		//int nFaceHeight = textureFaceImage1.cols;
		//if (pFaceTextureData != NULL) {
		//	glTexSubImage2D(GL_TEXTURE_2D, 0, 256, 256, nFaceWidth, nFaceWidth, GL_BGR, GL_UNSIGNED_BYTE, (GLvoid*)pFaceTextureData);
		//	//glTexSubImage2D(GL_TEXTURE_2D, 0, GL_RGB, nFaceWidth, nFaceHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pFaceTextureData);
		//	//glGenerateMipmap(GL_TEXTURE_2D);
		//}
	}
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glFlush();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	nIndex = !nIndex;
}

