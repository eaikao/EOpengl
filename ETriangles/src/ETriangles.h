#pragma once

#include <QOpenGLFunctions_4_3_Core>
#include <iostream>
#include <qopengl.h>
#include <qopenglext.h>

const GLuint NumVertices = 6;

class ETriangles : public QOpenGLFunctions_4_3_Core {
public:
	enum VAO_IDS {
		Triangles,
		NumVAOs
	};

	enum Buffer_IDS {
		ArrayBuffer,
		NumBuffers
	};

	enum EBO_IDS {
		INDEXS,
		NumEBOs
	};

	enum Attrib_IDs {
		vPosition = 0
	};

	enum Color_IDs {
		vColor = 1
	};

	enum Texture_IDs {
		vTexCoord = 2
	};

	enum PBO_MODE{
		NO_PBO = 0,
		SINGLE_PBO = 1,
		DOUBLE_PBO = 2
	};

public:
	ETriangles ();
	virtual ~ETriangles ();

	void InitOne();
	void DisplayOne();

	void InitTwo();
	void DisplayTwo();

	void InitThree();
	void DisplayThree();

	void InitFour();
	void DisplayFour();

	// Texture
	void InitFive();
	void DisplayFive();

	// Mix Texture
	void InitSix();
	void DisplaySix();
	
	// color mix texture
	void InitSeven();
	void DisplaySeven();

	//coordinate transformation
	void InitEight();
	void DisplayEight();

	//coordinate systems
	void InitNine();
	void DisplayNine();

	//3D coordinate systems, a cube
	void InitTen();
	void DisplayTen();

	//Multiple cubes
	void InitEleven();
	void DisplayEleven();


	//PBO 
	void InitThirteen();
	void DisplayThirteen();
	void InitGl();			// initialize OpenGL, disable unused features
	void ChangePboMode(int nMode);
	void UpdatePixels(GLubyte* pDst, int nSize);
	

	//RAM Dynamic texture
	void InitFourteen();
	void DisplayFourteen();

	//PBO Dynamic texture
	void InitFifteen();
	void DisplayFifteen();

private:
	GLuint VAOs[NumVAOs];
	GLuint EBOs[NumEBOs];
	GLuint Buffers[NumBuffers];

	GLubyte* m_pImageData;
	int		 m_pboMode;
};