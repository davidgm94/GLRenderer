#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

out vec4 fragmentColor;

in vec3 normal;
in vec3 fragmentPosition;
in vec2 texCoords;

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambientLight = light.ambient * vec3(texture(material.diffuse, texCoords));

	vec3 normalUnitVector = normalize(normal);
	vec3 lightDirection = normalize(light.position - fragmentPosition);
	// do this so lightColors can't be negative --> dot product with (angles > 90º) produce negative results
	float diff = max(dot(normalUnitVector, lightDirection), 0.0f);
	vec3 diffuseLight = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

	vec3 viewDirection = normalize(viewPosition - fragmentPosition);
	vec3 reflectionDirection = reflect(-lightDirection, normalUnitVector);
	float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
	vec3 specularLight = light.specular * spec * vec3(texture(material.specular, texCoords));

	float distance = length(light.position - fragmentPosition);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambientLight *= attenuation;
	diffuseLight *= attenuation;
	ambientLight *= attenuation;

	vec3 result = ambientLight + diffuseLight + specularLight;
	fragmentColor = vec4(result, 1.0f);
}