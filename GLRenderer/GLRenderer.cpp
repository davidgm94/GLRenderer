#include <stdio.h>

#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void readShaderSource(const char* shaderSourcePath, char* buffer)
{
	FILE* file = fopen(shaderSourcePath, "r+");
	assert(file);

	fseek(file, 0, SEEK_END);
	long length = ftell(file); // length (bytes) of the shader bytecode
	assert(length >= 0);
	fseek(file, 0, SEEK_SET);


	size_t rc = fread(&buffer[0], 1, length, file);
	buffer[rc] = '\0';
	fclose(file);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

bool checkForCompilationSuccess(unsigned shader)
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

bool checkForLinkingSuccess(unsigned shaderProgram)
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

int main()
{
	const int width = 1024;
	const int height = 576;
	const char* title = "LearnOpenGL";
	auto glfwInitialization = glfwInit();

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	assert(glfwInitialization);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	assert(window);
	glfwMakeContextCurrent(window);

	auto gladInitialization = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	assert(gladInitialization);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, (const GLchar * const*)&vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	bool VScompilation = checkForCompilationSuccess(vertexShader);

	unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar * const*)&fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	bool FScompilation = checkForCompilationSuccess(fragmentShader);

	unsigned shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	bool SPlinking = checkForLinkingSuccess(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] =
	{
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
	};

	unsigned indices[] =
	{
		0, 1, 3,
		1, 2, 3
	};

	unsigned vertexBuffer, vertexArray;
	unsigned elementArrayBuffer;
	glGenVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &elementArrayBuffer);

	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glUseProgram(shaderProgram);
		glBindVertexArray(vertexArray);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}