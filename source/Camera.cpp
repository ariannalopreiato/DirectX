#include "pch.h"
#include "Camera.h"

Camera::Camera(float aspectRatio, float fovAngle, const Vector3& origin)
	:fovAngle(fovAngle)
	, origin(origin)
	, aspectRatio(aspectRatio)
{
	fov = tanf((fovAngle * TO_RADIANS) / 2.f);
}

void Camera::Initialize(float aspectRatio, float fovAngle, Vector3 origin)
{
	fovAngle = fovAngle;
	fov = tanf((fovAngle * TO_RADIANS) / 2.f);

	origin = origin;

	aspectRatio = aspectRatio;
}

Matrix Camera::GetViewMatrix()
{
	return viewMatrix;
}

Matrix Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::CalculateViewMatrix()
{
	viewMatrix = Matrix::CreateLookAtLH(origin, forward, Vector3::UnitY);
	invViewMatrix = Matrix::Inverse(viewMatrix);
}

void Camera::CalculateProjectionMatrix()
{
	projectionMatrix = Matrix::CreatePerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::Update(Timer* pTimer)
{
	const float deltaTime = pTimer->GetElapsed();

	moveFactor = 0.5f;

	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
		origin.z += moveFactor;
	if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
		origin.z -= moveFactor;
	if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
		origin.x -= moveFactor;
	if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
		origin.x += moveFactor;

	//Mouse Input
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
	if ((mouseState & SDL_BUTTON_LMASK) != 0)
	{
		if (pKeyboardState[SDL_SCANCODE_LSHIFT] || pKeyboardState[SDL_SCANCODE_RSHIFT])
			moveFactor += 4.f;

		if ((mouseState & SDL_BUTTON_RMASK) != 0)
		{
			if (mouseY < 0)
				origin.y -= moveFactor;
			if (mouseY > 0)
				origin.y += moveFactor;
		}
		else
		{
			if (mouseY < 0)
				origin.z += moveFactor;
			if (mouseY > 0)
				origin.z -= moveFactor;

			totalYaw -= mouseX * deltaTime;
		}
	}
	else
	{
		if ((mouseState & SDL_BUTTON_RMASK) != 0)
		{
			totalPitch += mouseY * deltaTime;
			totalYaw -= mouseX * deltaTime;
		}
	}
	const Matrix rotation{ Matrix::CreateRotation(totalPitch, totalYaw, 0.f) };
	forward = rotation.TransformVector(Vector3::UnitZ);
	forward.Normalize();

	//Update Matrices
	CalculateViewMatrix();
	CalculateProjectionMatrix();
}