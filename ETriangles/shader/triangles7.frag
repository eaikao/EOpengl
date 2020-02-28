#version 430

in  vec3 OurColor;
in  vec2 TexCoord;
out vec4 fColor;

uniform sampler2D TextureCntainer;

void main()
{
	fColor = texture(TextureCntainer, TexCoord) * vec4(OurColor, 1.0);
}