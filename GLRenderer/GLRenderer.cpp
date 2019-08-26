#include "camera.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <meshoptimizer.h>
#include <fast_obj.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include <stdint.h>
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 =	int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using bool32 = i32;


// BLOATED STUFF:
#include <vector>
using std::vector;
#include <string>
using std::string;

struct TemporalVertex
{
	vec3 position;
};

struct posNormal
{
	vec3 position;
	vec3 normal;
};

struct posNormalTexCoord
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
};

struct PositionNormalTexcoordTangentBitangent
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	vec3 tangent;
	vec3 bitangent;
};

using Vertex = PositionNormalTexcoordTangentBitangent;

struct Texture
{
	u32 id;
	string type;
	string path;
};

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

#define MATERIALS_COUNT 24

enum class MaterialType : u32
{
	EMERALD				= 0,
	JADE				= 1,
	OBSIDIAN			= 2,
	PEARL				= 3,
	RUBY				= 4,
	TURQUOISE			= 5,
	BRASS				= 6,
	BRONZE				= 7,
	CHROME				= 8,
	COPPER				= 9,
	GOLD				= 10,
	SILVER				= 11,
	BLACK_PLASTIC		= 12,
	CYAN_PLASTIC		= 13,
	GREEN_PLASTIC		= 14,
	RED_PLASTIC			= 15,
	WHITE_PLASTIC		= 16,
	YELLOW_PLASTIC		= 17,
	BLACK_RUBBER		= 18,
	CYAN_RUBBER			= 19,
	GREEN_RUBBER		= 20,
	RED_RUBBER			= 21,
	WHITE_RUBBER		= 22,
	YELLOW_RUBBER		= 23,
};

const static Material materials[MATERIALS_COUNT] =
{
	{ vec3(0.0215f ,0.1745f, 0.0215f),			vec3(0.07568f, 0.61424f, 0.07568f),		vec3(0.633f, 0.727811f, 0.633f), 0.6f},
	{ vec3(0.135f, 0.2225f, 0.1575f),			vec3(0.54		,0.89		,0.63		),	vec3(0.316228	,0.316228	,0.316228	), 0.1			},
	{ vec3(0.05375f, 0.05f, 0.06625f),			vec3(0.18275	,0.17		,0.22525	),	vec3(0.332741	,0.328634	,0.346435	), 0.3			},
	{ vec3(0.25f, 0.20725f, 0.20725f),			vec3(1			,0.829		,0.829		),	vec3(0.296648	,0.296648	,0.296648	), 0.088		},
	{ vec3(0.1745f, 0.01175f, 0.01175f),		vec3(0.61424	,0.04136	,0.04136	),	vec3(0.727811	,0.626959	,0.626959	), 0.6			},
	{ vec3(0.1f, 0.18725f, 0.1745f),			vec3(0.396		,0.74151	,0.69102	),	vec3(0.297254	,0.30829	,0.306678	), 0.1			},
	{ vec3(0.329412f, 0.223529f, 0.027451f),	vec3(0.780392	,0.568627	,0.113725	),	vec3(0.992157	,0.941176	,0.807843	), 0.21794872	},
	{ vec3(0.2125f, 0.1275f, 0.054f),			vec3(0.714		,0.4284		,0.18144	),	vec3(0.393548	,0.271906	,0.166721	), 0.2			},
	{ vec3(0.25f, 0.25f, 0.25f),				vec3(0.4		,0.4		,0.4		),	vec3(0.774597	,0.774597	,0.774597	), 0.6			},
	{ vec3(0.19125f, 0.0735, 0.0225f),			vec3(0.7038		,0.27048	,0.0828		),	vec3(0.256777	,0.137622	,0.086014	), 0.1			},
	{ vec3(0.24725f, 0.1995f, 0.0745f),			vec3(0.75164	,0.60648	,0.22648	),	vec3(0.628281	,0.555802	,0.366065	), 0.4			},
	{ vec3(0.19225f, 0.19225f, 0.19225f),		vec3(0.50754	,0.50754	,0.50754	),	vec3(0.508273	,0.508273	,0.508273	), 0.4			},
	{ vec3(0.0f, 0.0, 0.0),						vec3(0.01		,0.01		,0.01		),	vec3(0.50		,0.50		,0.50		), .25			},
	{ vec3(0.0f, 0.1f, 0.06f),					vec3(0.0		,0.50980392	,0.50980392	),	vec3(0.50196078	,0.50196078	,0.50196078	), .25			},
	{ vec3(0.0f, 0.0f, 0.0f),					vec3(0.1		,0.35		,0.1		),	vec3(0.45		,0.55		,0.45		), .25			},
	{ vec3(0.0f, 0.0f, 0.0f),					vec3(0.5		,0.0		,0.0		),	vec3(0.7		,0.6		,0.6		), .25			},
	{ vec3(0.0f, 0.0f, 0.0f),					vec3(0.55		,0.55		,0.55		),	vec3(0.70		,0.70		,0.70		), .25			},
	{ vec3(0.0f, 0.0f, 0.0f),					vec3(0.5		,0.5		,0.0		),	vec3(0.60		,0.60		,0.50		), .25			},
	{ vec3(0.02f, 0.02f, 0.02f),				vec3(0.01		,0.01		,0.01		),	vec3(0.4		,0.4		,0.4		), .078125		},
	{ vec3(0.0f, 0.05f, 0.05f),					vec3(0.4		,0.5		,0.5		),	vec3(0.04		,0.7		,0.7		), .078125		},
	{ vec3(0.0f, 0.05f, 0.0f),					vec3(0.4		,0.5		,0.4		),	vec3(0.04		,0.7		,0.04		), .078125		},
	{ vec3(0.05f, 0.0f, 0.0f),					vec3(0.5		,0.4		,0.4		),	vec3(0.7		,0.04		,0.04		), .078125		},
	{ vec3(0.05f, 0.05f, 0.05f),				vec3(0.5		,0.5		,0.5		),	vec3(0.7		,0.7		,0.7		), .078125		},
	{ vec3(0.05f, 0.05f, 0.0f),					vec3(0.5		,0.5		,0.4		),	vec3(0.7		,0.7		,0.04		), .078125		},
};

struct AttenuationConstants
{
	float distance;
	float constant; // always 1.0f
	float linear;
	float quadratic;
};

static AttenuationConstants attenuationConstants[] =
{
	{ 7.0f,		1.0f,	0.7f,		1.8f		},
	{ 13.0f,	1.0f,	0.35f,		0.44f		},
	{ 20.0f,	1.0f,	0.22f,		0.20f		},
	{ 32.0f,	1.0f,	0.14f,		0.07f		},
	{ 50.0f,	1.0f,	0.09f,		0.032f		},
	{ 65.0f,	1.0f,	0.07f,		0.017f		},
	{ 100.0f,	1.0f,	0.045f,		0.0075f		},
	{ 160.0f,	1.0f,	0.027f,		0.0028f		},
	{ 200.0f,	1.0f,	0.022f,		0.0019f		},
	{ 325.0f,	1.0f,	0.014f,		0.0007f		},
	{ 600.0f,	1.0f,	0.007f,		0.0002f		},
	{ 3250.0f,	1.0f,	0.0014f,	0.000007f	},
};


#ifndef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

struct
{
	float lastX, lastY;
} g_MouseLastPosition;
bool32 firstMouse = true;

int width = 1024;
int height = 576;

#define ASPECT_RATIO ( ((float)width) / ((float)height) )

Camera g_Camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool32 blinnModel = false;
bool32 blinnKeyPressed = false;

static inline mat4 getViewMatrix()
{
	return lookAt(g_Camera.position, g_Camera.position + g_Camera.front, g_Camera.up);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		updateCameraWithKeyboard(g_Camera, CameraMovement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		updateCameraWithKeyboard(g_Camera, CameraMovement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		updateCameraWithKeyboard(g_Camera, CameraMovement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		updateCameraWithKeyboard(g_Camera, CameraMovement::RIGHT, deltaTime);
	}

	if ( (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) && !blinnKeyPressed)
	{
		blinnModel = !blinnModel;
		blinnKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xPosition, double yPosition)
{
	if (firstMouse)
	{
		g_MouseLastPosition.lastX = float(xPosition);
		g_MouseLastPosition.lastY = float(yPosition);
		firstMouse = false;
	}

	float xOffset = float(xPosition) - g_MouseLastPosition.lastX;
	float yOffset = g_MouseLastPosition.lastY - float(yPosition);
	g_MouseLastPosition.lastX = float(xPosition);
	g_MouseLastPosition.lastY = float(yPosition);

	updateCameraWithMouse(g_Camera, xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	updateCameraWithMouseScroll(g_Camera, float(yOffset));
}

char* readFile(const char* filePath)
{
	FILE* file = fopen(filePath, "r+");
	assert(file);

	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	assert(length >= 0);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char*)malloc(length + 1);
	size_t rc = fread(&buffer[0], 1, length, file);

	buffer[rc] = '\0';
	fclose(file);

	return buffer;
}

#include <fstream>
#include <string>
#include <sstream>
string readFileBloated(const char* shaderPath)
{
	string shaderSource;
	std::ifstream shaderFile;
	shaderFile.open(shaderPath);
	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();
	shaderSource = shaderStream.str();
	return shaderSource;
}

bool32 checkForCompilationSuccess(u32 shader)
{
	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success)
	{
		return true;
	}
	else
	{
		glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
		printf(infoLog);
		return false;
	}
}

bool32 checkForLinkingSuccess(u32 shaderProgram)
{
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (success)
	{
		return true;
	}
	else
	{
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
		return false;
	}
}

u32 createShader(const char* shaderPath, u32 shaderType)
{
	string shaderString = readFileBloated(shaderPath);
	const char* shaderRawString = shaderString.c_str();

	u32 shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &shaderRawString, nullptr);
	glCompileShader(shader);

	bool32 shaderCompilation = checkForCompilationSuccess(shader);
	assert(shaderCompilation);

	return shader;
}

u32 createShaderProgram(u32 vertexShader, u32 fragmentShader)
{
	u32 shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	bool32 SPlinking = checkForLinkingSuccess(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

#define MAX_SHADER_TYPES 3

struct ShaderNames
{
	char value[MAX_SHADER_TYPES][256];
};

static inline void initShaderNames(ShaderNames* n)
{
	memset(n, 0, sizeof(ShaderNames));
}

enum ShaderType
{
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	GEOMETRY_SHADER,
};

struct Shader
{
	u32 shaders[MAX_SHADER_TYPES];
	bool32 activeShaders[MAX_SHADER_TYPES];
	u32 program;

	Shader(const ShaderNames& names)
	{
		for (u32 i = 0; i < MAX_SHADER_TYPES; i++)
		{
			if (*(names.value[i]) == '\0')
			{
				activeShaders[i] = false;
				continue;
			}

			u32 shaderType;
			switch (i)
			{
				case (VERTEX_SHADER):
				{
					shaderType = GL_VERTEX_SHADER;
				} break;
				case (FRAGMENT_SHADER):
				{
					shaderType = GL_FRAGMENT_SHADER;
				} break;
				case (GEOMETRY_SHADER):
				{
					shaderType = GL_GEOMETRY_SHADER;
				} break;
			}
			shaders[i] = createShader(names.value[i], shaderType);
			activeShaders[i] = true;
		}
		program = glCreateProgram();

		for (u32 i = 0; i < MAX_SHADER_TYPES; i++)
		{
			if (activeShaders[i])
			{
				glAttachShader(program, shaders[i]);
			}
		}
		glLinkProgram(program);
		checkForLinkingSuccess(program);
		for (u32 i = 0; i < MAX_SHADER_TYPES; i++)
		{
			if (activeShaders[i])
			{
				glDeleteProgram(shaders[i]);
			}
		}
	}

	void use()
	{
		glUseProgram(program);
	}

	void setInt(const char* objectName, int value) const
	{
		glUniform1i(glGetUniformLocation(program, objectName), value);
	}

	void setFloat(const char* objectName, float value) const
	{
		glUniform1f(glGetUniformLocation(program, objectName), value);
	}

	void setVec3(const char* objectName, const vec3& value)
	{
		glUniform3fv(glGetUniformLocation(program, objectName), 1, &value[0]);
	}
	void setVec3(const char* objectName, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(program, objectName), x, y, z);
	}
	void setMat4(const char* objectName, const mat4& value)
	{
		glUniformMatrix4fv(glGetUniformLocation(program, objectName), 1, GL_FALSE, &value[0][0]);
	}
};


struct Mesh
{
	vector<Vertex> vertices;
	vector<u32> indices;
	vector<Texture> textures;

	u32 vertexArray;
	u32 vertexBuffer;
	u32 elementBuffer;

	void setupMesh()
	{
		glGenVertexArrays(1, &vertexArray);
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &elementBuffer);

		glBindVertexArray(vertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

		glBindVertexArray(0);
	}

	Mesh(const vector<Vertex>& vertices_, const vector<u32>& indices_, const vector<Texture>& textures_)
		: vertices(vertices_), indices(indices_), textures(textures_)
	{
		setupMesh();
	}

	void draw(const Shader& shader)
	{
		u32 diffuseNumber = 1;
		u32 specularNumber = 1;
		u32 normalNumber = 1;
		u32 heightNumber = 1;

		for (u32 i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			string number;
			string name = textures[i].type;

			if (name == "texture_diffuse")
				number = std::to_string(diffuseNumber++);
			else if (name == "texture_specular")
				number = std::to_string(specularNumber++);
			else if (name == "texture_normal")
				number = std::to_string(normalNumber++);
			else if (name == "texture_height")
				number = std::to_string(heightNumber++);

			shader.setInt((name + number).c_str(), (int)i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		
		glBindVertexArray(vertexArray);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}
};

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Model
{
	vector<Texture> loadedTextures;
	vector<Mesh> meshes;
	string directory;
	bool32 gammaCorrection;
	//void processNode()
	Model(const char* path, bool32 gammaCorrection = false)
	{
		load(path);
	}

	void processNode(aiNode* node, const aiScene* scene)
	{
		for (u32 i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (u32 i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		vector<Vertex> vertices;
		vector<u32> indices;
		vector<Texture> textures;

		for (u32 i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;

			if (mesh->mTextureCoords[0])
			{
				vec2 texCoord;
				texCoord.x = mesh->mTextureCoords[0][i].x;
				texCoord.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoord = texCoord;
			}
			else
			{
				vertex.texCoord = vec2(0.0f, 0.0f);
			}

			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;

			vertices.push_back(vertex);
		}

		for (u32 i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (u32 j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 1. Diffuse maps
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. Normal maps
		vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. Height maps
		vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		return Mesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (u32 i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (u32 j = 0; j < loadedTextures.size(); j++)
			{
				if (strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(loadedTextures[j]);
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				Texture texture;
				texture.id = textureFromFile(str.C_Str());
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				loadedTextures.push_back(texture);
			}
		}

		return textures;
	}

	u32 textureFromFile(const char* textureName)
	{
		string texturePath = directory + '/' + textureName;
		u32 texture;
		const u32 textureType = GL_TEXTURE_2D;
		// TEXTURE 1
		glGenTextures(1, &texture);
		
		int width, height, channelCount;
		stbi_set_flip_vertically_on_load(true);

		u8* data = stbi_load(texturePath.c_str(), &width, &height, &channelCount, 0);
		assert(data);

		u32 format = 0;
		switch (channelCount)
		{
			case (1):
			{
				format = GL_RED;
			} break;
			case (3):
			{
				format = GL_RGB;
			} break;
			case (4):
			{
				format = GL_RGBA;
			} break;
			default:
			{
				format = 0;
				assert(channelCount != 3 && !(channelCount > 4) && "Error: Format not supported");
			} break;
		}

		glBindTexture(textureType, texture);

		glTexImage2D(textureType, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		return texture;
	}

	void load(string const& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		assert(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && (scene->mRootNode));

		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void draw(const Shader& shader)
	{
		for (Mesh& mesh : meshes)
		{
			mesh.draw(shader);
		}
	}
};

int queryMaxNAttributes()
{
	// query for n of attributes
	int numberOfAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numberOfAttributes);
	printf("MAXIMUM NUMBER OF ATTRIBUTES SUPPORTED: %d\n", numberOfAttributes);
	return numberOfAttributes;
}

u32 createBuffer(u32 bufferType, const void* data, size_t dataSize, u32 usage)
{
	u32 buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(bufferType, buffer);
	glBufferData(bufferType, dataSize, data, usage);

	return buffer;
}

u32 createVertexArray()
{
	u32 vertexArray;
	glGenVertexArrays(1, &vertexArray);

	return vertexArray;
}

struct VertexFeature
{
	u32 elementCount;
	u32 elementSize;
	u32 elementType;
	bool32 normalized = false;
};

void setAttributes(const VertexFeature* vertexFeatures, size_t vertexFeatureCount, size_t vertexSize)
{
#ifdef _DEBUG
	size_t sizeCount = 0;
	for (u32 i = 0; i < vertexFeatureCount; i++)
	{
		VertexFeature const& feature = vertexFeatures[i];
		sizeCount += feature.elementCount * feature.elementSize;
	}
	size_t sizeOfVertex = sizeof(Vertex);
	if (sizeOfVertex != sizeCount)
	{
		assert(!"Vertex sizes differ. Check padding and vertex layout, maybe a bug made of copypasta origins");
	}
	assert(sizeCount == sizeof(Vertex));
#endif

	u64 stride = 0;
	for (u32 i = 0; i < vertexFeatureCount; i++)
	{
		VertexFeature const& feature = vertexFeatures[i];
		glVertexAttribPointer(i, feature.elementCount, feature.elementType, feature.normalized, (GLsizei)vertexSize, (void*)(stride));
		glEnableVertexAttribArray(i);
		stride += feature.elementCount * feature.elementSize;
	}
}

u32 createTexture(const char* texturePath)
{
	u32 texture;
	const u32 textureType = GL_TEXTURE_2D;
	// TEXTURE 1
	glGenTextures(1, &texture);
	glBindTexture(textureType, texture);
	
	glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, channelCount;
	stbi_set_flip_vertically_on_load(true);

	u8* data = stbi_load(texturePath, &width, &height, &channelCount, 0);
	assert(data);

	u32 format;
	switch (channelCount)
	{
		case (1):
		{
			format = GL_RED;
		} break;
		case (3):
		{
			format = GL_RGB;
		} break;
		case (4):
		{
			format = GL_RGBA;
		} break;
		default:
		{
			assert(channelCount != 3 && !(channelCount > 4) && "Error: Format not supported");
		} break;
	}

	glTexImage2D(textureType, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	return texture;
}

int main(int argc, char** argv)
{
	initCamera(g_Camera, vec3(0.0f, 0.0f, 55.0f));
	g_MouseLastPosition.lastX = width / 2.0f;
	g_MouseLastPosition.lastY = height/ 2.0f;
	vec3 lightPosition(1.2f, 1.0f, 2.0f);
	const char* title = "LearnOpenGL";

	int glfwInitialization = glfwInit();
	assert(glfwInitialization);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	assert(window);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int gladInitialization = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	assert(gladInitialization);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	ShaderNames shaderNames;
	initShaderNames(&shaderNames);
	strcpy(shaderNames.value[0], "asteroid.vert.glsl");
	strcpy(shaderNames.value[1], "asteroid.frag.glsl");
	Shader asteroidShader(shaderNames);

	ShaderNames shaderNames2;
	initShaderNames(&shaderNames2);
	strcpy(shaderNames2.value[0], "planet.vert.glsl");
	strcpy(shaderNames2.value[1], "planet.frag.glsl");
	Shader planetShader(shaderNames2);

	Model planet("models/planet/planet.obj");
	Model rock("models/rock/rock.obj");

	const u32 asteroidCount = 100000;
	mat4* modelMatrices = new mat4[asteroidCount];
	srand(glfwGetTime());

	float radius = 150.0f;
	float offset = 25.0f;

	for (u32 i = 0; i < asteroidCount; i++)
	{
		mat4 world = mat4(1.0f);
		float angle = float(i) / float(asteroidCount) * 360.0f;
		float displacement = (rand() % int(2 * offset * 100)) / 100.0f - offset;
		float x = glm::sin(angle) * radius + displacement;
		displacement = (rand() % int(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f;
		displacement = (rand() % int(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		world = translate(world, vec3(x, y, z));

		float scaleBy = (rand() % 20) / 100.0f + 0.05f;
		world = scale(world, vec3(scaleBy));

		float rotationAngle = (rand() % 360);
		world = rotate(world, rotationAngle, vec3(0.4f, 0.6f, 0.8f));

		modelMatrices[i] = world;
	}

	u32 instanceBuffer;
	glGenBuffers(1, &instanceBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, asteroidCount * sizeof(mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (u32 i = 0; i < rock.meshes.size(); i++)
	{
		u32 vertexArray = rock.meshes[i].vertexArray;
		glBindVertexArray(vertexArray);

		const u64 vec4Size = sizeof(vec4);
		const u64 mat4Size = 4 * vec4Size;
		u64 strideMultiplier = 0;
		for (u32 currentVertexAttributeIndex = 3; currentVertexAttributeIndex < 7; currentVertexAttributeIndex++, strideMultiplier++)
		{
			glEnableVertexAttribArray(currentVertexAttributeIndex);
			glVertexAttribPointer(currentVertexAttributeIndex, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(strideMultiplier * vec4Size));
		}

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// RENDER & UPDATE
		
		mat4 proj = perspective(radians(45.0f), ASPECT_RATIO, 0.1f, 1000.0f);
		mat4 view = getViewMatrix();
		mat4 worldPlanetMatrix(1.0f);
		worldPlanetMatrix = translate(worldPlanetMatrix, vec3(0.0f, -3.0f, 0.0f));
		worldPlanetMatrix = scale(worldPlanetMatrix, vec3(4.0f));
	
		planetShader.use();
		planetShader.setMat4("proj", proj);
		planetShader.setMat4("view", view);
		planetShader.setMat4("world", worldPlanetMatrix);

		planet.draw(planetShader);

		asteroidShader.use();
		asteroidShader.setMat4("proj", proj);
		asteroidShader.setMat4("view", view);
		asteroidShader.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.loadedTextures[0].id);

		const u32 rockMeshesSize = rock.meshes.size();
		for (u32 i = 0; i < rockMeshesSize; i++)
		{
			glBindVertexArray(rock.meshes[i].vertexArray);
			glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, asteroidCount);
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete[] modelMatrices;

	glfwTerminate();
	return 0;
}