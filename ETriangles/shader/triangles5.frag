#version 430

in  vec2 TexCoord;
out vec4 fColor;

uniform sampler2D textureWall;

void main()
{
	fColor = texture(textureWall, TexCoord);
}