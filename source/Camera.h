#pragma once
#include "Math.h"

using namespace dae;

class Camera
{
public:
	Camera(float aspectRatio, float fovAngle, const Vector3& origin);
	void Initialize(float aspectRatio, float fovAngle = 45.f, Vector3 origin = { 0.f, 0.f, 0.f });

	void Update(const Timer* pTimer);
	Matrix GetViewMatrix();
	Matrix GetProjectionMatrix();

	Matrix invViewMatrix{};

private:
	Vector3 origin{};
	float fovAngle{ 90.f };
	float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };
	float moveFactor{ 1.f };
	float aspectRatio{};

	Vector3 forward{ Vector3::UnitZ };
	Vector3 up{ Vector3::UnitY };
	Vector3 right{ Vector3::UnitX };

	float totalPitch{};
	float totalYaw{};

	
	Matrix viewMatrix{};
	Matrix projectionMatrix{};
	Matrix worldMatrix{};

	float nearPlane{ 0.1f };
	float farPlane{ 100.f };

	float m_AspectRatio{};

	void CalculateViewMatrix();
	void CalculateProjectionMatrix();
};