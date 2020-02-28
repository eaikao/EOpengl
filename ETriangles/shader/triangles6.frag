#version 430

in  vec3 OurColor;
in  vec2 TexCoord;
out vec4 fColor;

uniform sampler2D TextureCntainer;
uniform sampler2D TextureAwesomeface;

void main()
{
	fColor = mix(texture(TextureCntainer, TexCoord), texture(TextureAwesomeface, TexCoord), 0.2);
}