#version 430
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoord;

out vec2 TexCoord;
uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(vPosition, 1.0);
	TexCoord = vec2(vTexCoord.x, vTexCoord.y);
}