#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragmentPosition;
out vec3 normal;
out vec2 texCoords;

void main()
{
	fragmentPosition = vec3(world * vec4(aPos, 1.0f));
	normal = mat3(transpose(inverse(world))) * aNormal;
	gl_Position = proj * view * vec4(fragmentPosition, 1.0f);
	texCoords = aTexCoords;
}