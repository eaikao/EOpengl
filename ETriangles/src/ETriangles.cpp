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

//两个三角形组成的矩形
void ETriangles::InitOne() {
	initializeOpenGLFunctions();

	//申请一个顶点数组对象，顶点数组对象存储的是顶点的操作状态和属性，是一个集合对象，
	//VAO本质上是state-object（状态对象）,记录的是一次绘制所需要的信息，包括数据在哪，数据格式之类的信息，以下设置的状态都会存储在vao中
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	// 初始化顶点数组缓存
	GLfloat vertices[NumVertices][2] = {
	{ -0.90, - 0.90},
	{ 0.85, -0.90},
	{ -0.90, 0.85},
	{ 0.90, -0.85 },
	{ 0.90, 0.90 },
	{ -0.85, 0.90 }
	};

	//VBO—> 顶点缓存对象
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	//类似将客户端(cpu)的数据拷贝到服务器端(gpu)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles1.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles1.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	//设置顶点数据在index位置可以访问的数据值，vertices中有NumVertices个顶点，每个顶点有两个元素，所以第二个实参为2，这里所设置的状态会保存到VAO对象中
	//第四个参数表示一个顶点的步长，如果顶点数值是紧密连着的，那么我们可以使用0表示，这里也可以用sizeof(float) * 2,第5个参数表示的是：缓冲中起始位置的偏移量(Offset)
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

	//一个三角形
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

	// 查询获取一致变量的位置
	gScaleLocation = glGetUniformLocation(program, "gScale");
	printf("gScaleLocation = %d\n", gScaleLocation);

	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
}

void ETriangles::DisplayTwo() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//设置渐变大小
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

	// 使用ebo绘制矩形
	GLfloat vertices[4][2] = {
	{ -0.5, 0.5 },		//左上角
	{ 0.5, 0.5 },		//右上角
	{ 0.5, -0.5 },		//右下角
	{-0.5, -0.5}		//左下角
	};

	//绘制两个三角形的顺序
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
	
	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles3.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles3.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	// 查询获取一致变量的位置
	gScaleLocation = glGetUniformLocation(program, "gScale");
	printf("gScaleLocation = %d\n", gScaleLocation);

	gFragColor = glGetUniformLocation(program, "gColor");
	printf("gFragColor = %d\n", gFragColor);

	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));
	//以线框模式绘制
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableVertexAttribArray(vPosition);
}

void ETriangles::DisplayThree() {
	initializeOpenGLFunctions();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//设置渐变大小
	static float Scale = 0.0f;
	Scale += 0.01f;
	glUniform1f(gScaleLocation, std::sinf(Scale));
	//设置渐变颜色
	static float fColor = 0.0f;
	fColor += 0.01f;
	glUniform4f(gFragColor, std::sinf(fColor), 0.0f, std::sinf(fColor), 1.0f);

	glBindVertexArray(VAOs[Triangles]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glFlush();
}

void ETriangles::InitFour() {
	initializeOpenGLFunctions();

	//申请一个顶点数组对象，顶点数组对象存储的是顶点的操作状态和属性，是一个集合对象，
	//VAO本质上是state-object（状态对象）,记录的是一次绘制所需要的信息，包括数据在哪，数据格式之类的信息，以下设置的状态都会存储在vao中
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	/*顶点位置和颜色在一起的情况*/
	GLfloat vertices[] = {
		// 位置              // 颜色
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // 左下
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // 顶部
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
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

	//三角形的位置和纹理坐标,纹理的坐标原点在左下角，图像数据的坐标原点在左上角，所以贴纹理的时候如果不进行坐标系统的转换，会导致图像时反的。
	//顶点坐标原点在中间
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,// bottom left
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
		0.0f, 1.0f, 0.0f, 0.5f, 0.0f
	};
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//位置
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
	//纹理
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//创建一个纹理对象
	GLuint TextureWall;
	glGenTextures(1, &TextureWall);
	glBindTexture(GL_TEXTURE_2D, TextureWall);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float borderColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles5.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles5.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);

	//载入图片生成一个纹理
	//awesomeface.png, wall.jpg
	cv::Mat textureImage = cv::imread("resource/wall.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		//按照rgb读入，按照bgr输出，显示才和原图一致
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

	//顶点坐标、颜色值、纹理坐标
	//顶点坐标原点在中间
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

	//绘制顺序
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//位置
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//颜色
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(vColor);

	//纹理
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoord);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles6.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles6.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//创建一个容器纹理对象
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
	//awesomeface.png, wall.jpg
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//创建一个笑脸纹理对象
	GLuint TextureAwesomeface;
	glGenTextures(1, &TextureAwesomeface);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureAwesomeface);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
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

	//绑定顶点数组对象
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//顶点信息
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

	//绘制顺序
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//位置
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//颜色
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(vColor);

	//纹理
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoord);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles7.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles7.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//创建一个容器纹理对象
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
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

	//顶点坐标、颜色值、纹理坐标
	//顶点坐标原点在中间
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

	//绘制顺序
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//位置
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//纹理
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles8.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles8.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//创建一个容器纹理对象
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
	//awesomeface.png, wall.jpg, opencv按照bgr读取
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//创建一个笑脸纹理对象
	GLuint TextureAwesomeface;
	glGenTextures(1, &TextureAwesomeface);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureAwesomeface);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
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

	//缩放0.5倍的矩阵
	float fZoomTrans[][4] = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	// 单通道
	cv::Mat matZoomtransform = cv::Mat::zeros(4, 4, CV_32F);
	for (int i = 0; i < matZoomtransform.rows; ++i) {
		for (int j = 0; j < matZoomtransform.cols; ++j) {
			matZoomtransform.at<float>(i, j) = fZoomTrans[i][j];
		}
	}

	float fCurTime = (float)QTime::currentTime().second();
	float fCosTime = std::cos(fCurTime);
	float fSinTime = std::sin(fCurTime);
	//沿z轴(0,0,1)旋转矩阵
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

	//位移矩阵
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

	// 在shader着色器中，会将变换矩阵的行变为列，所以在输入的时候列表示的是着色器中的行，这里是4行4列乘以4行一列
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

	//顶点坐标、颜色值、纹理坐标
	//顶点坐标原点在中间
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

	//绘制顺序
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//位置
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//纹理
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles9.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles9.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//创建一个容器纹理对象
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
	//awesomeface.png, wall.jpg, opencv按照bgr读取
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//创建一个笑脸纹理对象
	GLuint TextureAwesomeface;
	glGenTextures(1, &TextureAwesomeface);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureAwesomeface);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
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
	//沿x轴顺时针旋转55度
	fModel = glm::rotate(fModel, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 fView = glm::mat4(1.0f);
	//沿着z轴向后移动3个单位，转换为观察空间，使自己处于世界坐标中去观察该图像。
	fView = glm::translate(fView, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 fProjection = glm::mat4(1.0f);
	//齐次坐标，透视投影
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

	//开启深度测试
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//3D顶点坐标，1个面由两个三角形组成需要6个点，6个面一共36个点
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


	//位置
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//纹理
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles10.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles10.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//创建一个容器纹理对象
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
	//awesomeface.png, wall.jpg, opencv按照bgr读取
	cv::Mat textureImage = cv::imread("resource/container.jpg");
	unsigned char* pTextureData = textureImage.data;
	int nWidth = textureImage.rows;
	int nHeight = textureImage.cols;
	if (pTextureData != NULL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pTextureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//创建一个笑脸纹理对象
	GLuint TextureAwesomeface;
	glGenTextures(1, &TextureAwesomeface);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureAwesomeface);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//载入图片生成一个纹理
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //清除深度缓冲

	glm::mat4 fModel = glm::mat4(1.0f);
	//沿60度轴随着时间旋转角度
	float fCurTime = (float)QTime::currentTime().second();
	fModel = glm::rotate(fModel, (float)fCurTime, glm::vec3(0.5f, 1.0f, 0.0f));

	glm::mat4 fView = glm::mat4(1.0f);
	//沿着z轴向后移动3个单位，转换为观察空间，使自己处于世界坐标中去观察该图像。
	fView = glm::translate(fView, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 fProjection = glm::mat4(1.0f);
	//齐次坐标，透视投影
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //清除深度缓冲

	glm::mat4 fView = glm::mat4(1.0f);
	//沿着z轴向后移动3个单位，转换为观察空间，使自己处于世界坐标中去观察该图像。
	fView = glm::translate(fView, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 fProjection = glm::mat4(1.0f);
	//齐次坐标，透视投影
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
		// 根据不同的变换坐标转换当前立方体，先移动在旋转,即转换到世界坐标中
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

		//拷贝pbo中的数据到纹理对象
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, 0);

		// 更新pbo像素内容
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
	glPushMatrix();			//在修改模型矩阵之前保存之前的模型矩阵

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

	//绑定顶点数组对象
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//顶点信息
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

	//绘制顺序
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//位置
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//颜色
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(vColor);

	//纹理
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoord);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles7.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles7.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//创建一个容器纹理对象
	GLuint TextureCntainer;
	glGenTextures(1, &TextureCntainer);
	glBindTexture(GL_TEXTURE_2D, TextureCntainer);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//初始化数据到纹理中
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

	//绑定顶点数组对象
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	//顶点信息
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

	//绘制顺序
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	glGenBuffers(1, EBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[INDEXS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//位置
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	//颜色
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(vColor);

	//纹理
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoord);

	// 加载顶点(VERTEX)着色器和片元(FRAGMENT)着色器
	EShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "shader/triangles7.vert" },
			{ GL_FRAGMENT_SHADER, "shader/triangles7.frag" },
			{ GL_NONE, NULL }
	};
	ELoadShader loadShader;
	GLuint program = loadShader.LoadShaders(shaders);
	glUseProgram(program);


	//创建一个容器纹理对象
	glGenTextures(1, &gTextureId);
	glBindTexture(GL_TEXTURE_2D, gTextureId);

	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//设置纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//初始化数据到纹理中
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
		//拷贝pbo中的数据到纹理对象
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
		//拷贝pbo中的数据到纹理对象
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

