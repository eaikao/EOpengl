#pragma  once

#include <QOpenGLFunctions_4_3_Core>
#include <qopengl.h>
#include <qopenglext.h>

#define BUFFER_OFFSET(x)  ((const void*) (x))

typedef struct {
	GLenum       glType;
	const char*  cFilename;
	GLuint       glShader;
} EShaderInfo;

class ELoadShader : public QOpenGLFunctions_4_3_Core {
public:
	ELoadShader();
	~ELoadShader();

	GLuint LoadShaders(EShaderInfo* shadersInfo);
	static const GLchar* ReadShader(const char* filename);
private:

};