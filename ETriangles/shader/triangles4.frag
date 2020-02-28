#version 430

in vec3 ourColor;
out vec4 fColor;

void main()
{
	fColor = vec4(ourColor, 1.0);
}