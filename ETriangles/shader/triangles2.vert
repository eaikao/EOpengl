#version 430
layout (location = 0) in vec4 vPosition;

uniform float gScale;
void main()
{
	gl_Position = vec4(gScale * vPosition.x, gScale * vPosition.y, vPosition.z, 1.0);
}