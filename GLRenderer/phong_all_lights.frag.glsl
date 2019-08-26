#version 330 core

struct DirectionalLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Spotlight
{
	vec3 direction;
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};
struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform vec3 viewPosition;
uniform DirectionalLight directionalLight;
#define POINT_LIGHT_COUNT 4
uniform PointLight pointLights[POINT_LIGHT_COUNT];
uniform Spotlight spotlight;
uniform Material material;

vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 computePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);
vec3 computeSpotlight(Spotlight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);

out vec4 fragmentColor;
in vec3 fragmentPosition;
in vec3 normal;
in vec2 texCoords;

void main()
{
	vec3 normalUnitVector = normalize(normal);
	vec3 viewDirection = normalize(viewPosition - fragmentPosition);

	vec3 result = computeDirectionalLight(directionalLight, normalUnitVector, viewDirection);

	for (int i = 0; i < POINT_LIGHT_COUNT; i++)
	{
		result += computePointLight(pointLights[i], normalUnitVector, fragmentPosition, viewDirection);
	}
	result += computeSpotlight(spotlight, normalUnitVector, fragmentPosition, viewDirection);
	fragmentColor = vec4(result, 1.0f);
}

vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
	vec3 lightDirection = normalize(-light.direction);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float diff = max(dot(normal, lightDirection), 0.0f);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);

	vec3 ambientLight = light.ambient * vec3(texture(material.diffuse, texCoords));
	vec3 diffuseLight = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
	vec3 specularLight = light.specular * spec * vec3(texture(material.specular, texCoords));

	return (ambientLight + diffuseLight + specularLight);
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection)
{
	float distance = length(light.position - fragmentPosition);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 lightDirection = normalize(light.position - fragmentPosition);
	float diff = max(dot(normal, lightDirection), 0.0f);

	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);

	vec3 ambientLight = light.ambient * vec3(texture(material.diffuse, texCoords));
	vec3 diffuseLight = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
	vec3 specularLight = light.specular * spec * vec3(texture(material.specular, texCoords));

	ambientLight *= attenuation;
	diffuseLight *= attenuation;
	specularLight *= attenuation;

	return (ambientLight + diffuseLight + specularLight);
}

// calculates the color when using a spot light.
vec3 computeSpotlight(Spotlight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection)
{
	vec3 lightDirection = normalize(light.position - fragmentPosition);
	// diffuse shading
	float diff = max(dot(normal, lightDirection), 0.0);
	// specular shading
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragmentPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// spotlight intensity
	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}