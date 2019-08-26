#pragma once
#include <glm/glm.hpp>
using namespace glm;

enum class CameraMovement : u32
{
	FORWARD, BACKWARD, LEFT, RIGHT,
};

struct Camera
{
	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;
	vec3 worldUp;
	float pitch;
	float yaw;
	float roll;
	float movementSpeed;
	float mouseSensitivity;
	float zoom;
};

static inline void updateCameraVectors(Camera& camera)
{
	vec3 front;
	front.x = cos(radians(camera.pitch)) * cos(radians(camera.yaw));
	front.y = sin(radians(camera.pitch));
	front.z = cos(radians(camera.pitch)) * sin(radians(camera.yaw));
	camera.front = normalize(front);
	camera.right = normalize(cross(camera.front, camera.worldUp));
	camera.up = normalize(cross(camera.right, camera.front));
}

static inline void initCamera(Camera& camera,
	const vec3& position = vec3(0.0f, 0.0f, 0.0f),
	const vec3& up = vec3(0.0f, 1.0f, 0.0f),
	float pitch = 0.0f, float yaw = -90.0f)
{
	camera.position = position;
	camera.front = vec3(0.0f, 0.0f, -1.0f);
	camera.worldUp = up;
	camera.pitch = pitch;
	camera.yaw = yaw;
	camera.roll = 0.0f;
	camera.movementSpeed = 2.5f;
	camera.mouseSensitivity = 0.1f;
	camera.zoom = 45.0f;
	updateCameraVectors(camera);
}

static inline void initCameraScalars(Camera& camera,
	float posX, float posY, float posZ,
	float upX, float upY, float upZ,
	float pitch, float yaw)
{
	camera.position = vec3(posX, posY, posZ);
	camera.front = vec3(0.0f, 0.0f, -1.0f);
	camera.worldUp = vec3(upX, upY, upZ);
	camera.pitch = pitch;
	camera.yaw = yaw;
	camera.roll = 0.0f;
	camera.movementSpeed = 2.5f;
	camera.mouseSensitivity = 0.1f;
	camera.zoom = 45.0f;
	updateCameraVectors(camera);
}

static inline void updateCameraWithKeyboard(Camera& camera, CameraMovement direction, float deltaTime)
{
	float velocity = camera.movementSpeed * deltaTime;
	if (direction == CameraMovement::FORWARD)
	{
		camera.position += camera.front * velocity;
	}
	if (direction == CameraMovement::BACKWARD)
	{
		camera.position -= camera.front * velocity;
	}
	if (direction == CameraMovement::LEFT)
	{
		camera.position -= camera.right * velocity;
	}
	if (direction == CameraMovement::RIGHT)
	{
		camera.position += camera.right * velocity;
	}
}

static inline void updateCameraWithMouse(Camera& camera, float xOffset, float yOffset, bool constrainPitch = true)
{
	xOffset *= camera.mouseSensitivity;
	yOffset *= camera.mouseSensitivity;

	camera.pitch += yOffset;
	camera.yaw += xOffset;

	if (constrainPitch)
	{
		if (camera.pitch > 89.0f)
		{
			camera.pitch = 89.0f;
		}
		if (camera.pitch < -89.0f)
		{
			camera.pitch = -89.0f;
		}
	}

	updateCameraVectors(camera);
}

static inline void updateCameraWithMouseScroll(Camera& camera, float yOffset)
{
	float& zoom = camera.zoom;
	if (zoom >= 1.0f && zoom <= 45.0f)
	{
		zoom -= yOffset;
	}
	if (zoom <= 1.0f)
	{
		zoom = 1.0f;
	}
	if (zoom >= 45.0f)
	{
		zoom = 45.0f;
	}
}