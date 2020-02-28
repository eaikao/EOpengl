#include "ELoadShader.h"
#include <iostream>
#include <stdio.h>
#include <QDebug>

ELoadShader::ELoadShader() {

}

ELoadShader::~ELoadShader() { 
}

GLuint ELoadShader::LoadShaders(EShaderInfo* shadersInfo) {
	if (shadersInfo == NULL) { return 0; }
	initializeOpenGLFunctions();
	GLuint program = glCreateProgram();

	EShaderInfo* entry = shadersInfo;
	while (entry->glType != GL_NONE) {
		GLuint shader = glCreateShader(entry->glType);

		entry->glShader = shader;

		const GLchar* source = ReadShader(entry->cFilename);
		if (source == NULL) {
			for (entry = shadersInfo; entry->glType != GL_NONE; ++entry) {
				glDeleteShader(entry->glShader);
				entry->glShader = 0;
			}

			return 0;
		}

		glShaderSource(shader, 1, &source, NULL);
		delete[] source;

		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLsizei len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = new GLchar[len + 1];
			glGetShaderInfoLog(shader, len, &len, log);
			std::cerr << "Shader compilation failed: " << log << std::endl;
			delete[] log;

			return 0;
		}

		glAttachShader(program, shader);

		++entry;
	}

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(program, len, &len, log);
		std::cerr << "Shader linking failed: " << log << std::endl;
		delete[] log;

		for (entry = shadersInfo; entry->glType != GL_NONE; ++entry) {
			glDeleteShader(entry->glShader);
			entry->glShader = 0;
		}

		return 0;
	}

	return program;
}

const GLchar* ELoadShader::ReadShader(const char* filename) {
#ifdef WIN32
	FILE* infile;
	fopen_s(&infile, filename, "rb");
#else
	FILE* infile = fopen(filename, "rb");
#endif // WIN32

	if (!infile) {
#ifdef _DEBUG
		std::cerr << "Unable to open file '" << filename << "'" << std::endl;
#endif /* DEBUG */
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = new GLchar[len + 1];

	fread(source, 1, len, infile);
	fclose(infile);

	source[len] = 0;
	qDebug("%s", source);
	//for (int i = 0; i < len; i++) {
	//	
	//}
	return const_cast<const GLchar*>(source);
}

