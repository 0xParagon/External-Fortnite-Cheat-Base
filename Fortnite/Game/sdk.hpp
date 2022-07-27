#pragma once

#include <Windows.h>
#include <map>
#include <d3d9types.h>
#include "globals.hpp"
#include "../Overlay/render.hpp"
#include "../Driver/driver.hpp"
#include "../../Includes/strings.hpp"
#include "structs.hpp"

namespace Globals
{
	DWORD_PTR
		Uworld,
		LocalPawn,
		PlayerState,
		Localplayer,
		RootComponent,
		GameState,
		PlayerController,
		PlayerCameraManager;

	namespace camera
	{
		Vector3 CameraLocation, CameraRotation;
		float FovAngle;
	}
}

using namespace Globals;
using namespace camera;

enum InjectedInputMouseOptions
{
	Absolute = 32768,
	HWheel = 4096,
	LeftDown = 2,
	LeftUp = 4,
	MiddleDown = 32,
	MiddleUp = 64,
	Move = 1,
	MoveNoCoalesce = 8192,
	None = 0,
	RightDown = 8,
	RightUp = 16,
	VirtualDesk = 16384,
	Wheel = 2048,
	XDown = 128,
	XUp = 256
};

typedef struct _InjectedInputMouseInfo
{
	int DeltaX;
	int DeltaY;
	unsigned int MouseData;
	InjectedInputMouseOptions MouseOptions;
	unsigned int TimeOffsetInMilliseconds;
	void* ExtraInfo;
} InjectedInputMouseInfo;

typedef bool (WINAPI* InjectMouseInput_t)(InjectedInputMouseInfo* inputs, int count);
InjectMouseInput_t InjectMouseInput;

typedef struct _FNlEntity {
	uint64_t Actor;
	uint64_t MeshComponent;
}FNlEntity;
std::vector<FNlEntity> entityList;


auto GetBone(DWORD_PTR mesh, int id) -> Vector3
{
	DWORD_PTR array = driver->read<uintptr_t>(mesh + 0x590);
	if (array == NULL)
		array = driver->read<uintptr_t>(mesh + 0x5a0);

	FTransform bone = driver->read<FTransform>(array + (id * 0x60));

	FTransform ComponentToWorld = driver->read<FTransform>(mesh + 0x240);
	D3DMATRIX Matrix;

	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

auto ProjectWorldToScreen(Vector3 WorldLocation) -> Vector3
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	auto CameraViewInfo = driver->read<CameraInfo>(GameState + 0x4b0);

	CameraLocation = Vector3(CameraViewInfo.x, CameraViewInfo.y, CameraViewInfo.z);
	CameraRotation = Vector3(CameraViewInfo.Pitch, CameraViewInfo.Yaw, 0);

	D3DMATRIX tempMatrix = Matrix(CameraRotation, Vector3(0, 0, 0));

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]), 
			vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]), 
			vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - CameraLocation;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f) vTransformed.z = 1.f;

	FovAngle = driver->read<float>(PlayerController + 0x374) * 90;

	float ScreenCenterX = Width / 2.0f;
	float ScreenCenterY = Height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

auto isVisible(DWORD_PTR mesh) -> bool
{
	float fLastSubmitTime = driver->read<float>(mesh + 0x330);
	float fLastRenderTimeOnScreen = driver->read<float>(mesh + 0x338);

	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;

	return bVisible;
}

auto aimbot(float x, float y) -> void
{
	float ScreenCenterX = (Width / 2);
	float ScreenCenterY = (Height / 2);
	float AimSpeed = settings::aimbot::smoothness;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	InjectedInputMouseInfo info = { 0 };
	info.DeltaX = TargetX;
	info.DeltaY = TargetY;
	InjectMouseInput(&info, 1);

	return;
}

auto CallAimbot(DWORD_PTR entity) -> void
{
	Vector3 HeadOut = ProjectWorldToScreen(GetBone(driver->read<uint64_t>(entity + offsets::MeshComponent), 98));
	if (HeadOut.y != 0 || HeadOut.y != 0) aimbot(HeadOut.x, HeadOut.y);
}