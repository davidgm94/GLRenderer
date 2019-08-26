#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out vec2 texCoords;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	texCoords = TexCoord;
	gl_Position = proj * view * world * vec4(Position, 1.0);
}