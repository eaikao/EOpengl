#version 430
layout (location = 0) in vec4 vPosition;

out vec4 vertexColor;
uniform float gScale;
uniform vec4 gColor;
void main()
{
	gl_Position = vec4(gScale * vPosition.x, gScale * vPosition.y, vPosition.z, 1.0);
	vertexColor = gColor;
}