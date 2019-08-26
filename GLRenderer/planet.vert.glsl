#version 330 core
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 world;

void main()
{
	TexCoord = texCoord;
	gl_Position = proj * view * world * vec4(position, 1.0f);
}