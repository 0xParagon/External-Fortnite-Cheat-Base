#pragma once
#include "sdk.hpp"
#include <iostream>

using namespace Globals;
using namespace camera;

auto CacheGame() -> void
{
	while (true) 
	{
		std::vector<FNlEntity> tmpList;

		Uworld = driver->read<DWORD_PTR>(Fortnite::world);
		DWORD_PTR Gameinstance = driver->read<DWORD_PTR>(Uworld + offsets::Gameinstance);
		DWORD_PTR LocalPlayers = driver->read<DWORD_PTR>(Gameinstance + offsets::LocalPlayers);
		Localplayer = driver->read<DWORD_PTR>(LocalPlayers);

		PlayerController = driver->read<DWORD_PTR>(Localplayer + offsets::PlayerController);
		PlayerCameraManager = driver->read<DWORD_PTR>(PlayerController + offsets::PlayerCameraManager);

		if (settings::fovchanger)
			driver->write(PlayerCameraManager + offsets::FOVChanger, settings::fovchangervalue);

		LocalPawn = driver->read<DWORD_PTR>(PlayerController + offsets::AcknowledgedPawn);

		InLobby = false;
		if (!Globals::LocalPawn) InLobby = true;

		PlayerState = driver->read<DWORD_PTR>(LocalPawn + offsets::PlayerState);
		RootComponent = driver->read<DWORD_PTR>(LocalPawn + offsets::RootComponent);

		GameState = driver->read<DWORD_PTR>(Uworld + offsets::GameState);
		auto PlayerArray = driver->read<TArray<DWORD_PTR>>(GameState + offsets::PlayerArray);

		for (auto& Players : PlayerArray.GetIter())
		{
			DWORD_PTR CurrentActor = driver->read<DWORD_PTR>(Players + offsets::PrivatePawn);

			FNlEntity fnlEntity{ };
			fnlEntity.Actor = CurrentActor;
			fnlEntity.MeshComponent = driver->read<uint64_t>(CurrentActor + offsets::MeshComponent);
			tmpList.push_back(fnlEntity);
		}

		entityList.clear();
		entityList = tmpList;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

auto cheatloop() -> void
{
	auto entityListCopy = entityList;
	float closestDistance = FLT_MAX;
	DWORD_PTR closestPawn = NULL;

	if (settings::aimbot::drawfov) DrawCircle(ScreenCenterX, ScreenCenterY, settings::aimbot::fov, &Col.white, 100);

	for (int i = 0; i < entityListCopy.size(); ++i)
	{
		FNlEntity entity = entityListCopy.at(i);

		auto ActorRootComponent = driver->read<uintptr_t>(entity.Actor + offsets::RootComponent);
		auto RelativeLocation = driver->read<Vector3>(ActorRootComponent + offsets::RelativeLocation);
		//auto RelativeLocationOut = ProjectWorldToScreen(RelativeLocation);


		Vector3 vBaseBone = GetBone(entity.MeshComponent, 0);
		Vector3 vBaseBoneOut = ProjectWorldToScreen(vBaseBone);
		Vector3 vBaseBoneOut2 = ProjectWorldToScreen(Vector3(vBaseBone.x, vBaseBone.y, vBaseBone.z - 15));
		Vector3 vHeadBone = GetBone(entity.MeshComponent, 68);
		Vector3 vHeadBoneOut = ProjectWorldToScreen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z));
		Vector3 vHeadBoneOut2 = ProjectWorldToScreen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));

		auto distance = CameraLocation.Distance(RelativeLocation) - 10;

		float BoxHeight = abs(vHeadBoneOut2.y - vBaseBoneOut2.y);
		float BoxWidth = BoxHeight * 0.40;

		auto IsVisible = isVisible(entity.MeshComponent);

		ImColor ESPColor; if (IsVisible) { ESPColor = ImColor(0, 255, 0); }
		else { ESPColor = ImColor(255, 0, 0); }

		auto ActorPlayerState = driver->read<DWORD_PTR>(entity.Actor + offsets::PlayerState);
		auto ActorTeam = driver->read<int>(ActorPlayerState + offsets::TeamID);

		auto LocalTeam = driver->read<int>(PlayerState + offsets::TeamID);

		if (ActorTeam != LocalTeam || InLobby)
		{
			if (settings::visuals::snapline) if (distance < 200)
				DrawLine(ImVec2(Width / 2, Height), ImVec2(vBaseBoneOut.x, vBaseBoneOut.y), ESPColor, 0.5f);

			if (settings::visuals::box) if (distance < 150)
				Draw2DBox(vBaseBoneOut.x - (BoxWidth / 2), vHeadBoneOut2.y, BoxWidth, BoxHeight, ESPColor);

			if (settings::visuals::distance)
			{
				char dist[64];
				sprintf_s(dist, "Entity [%.fM]", distance);
				std::string Out = dist;
				ImVec2 TextSize = ImGui::CalcTextSize(Out.c_str());

				ImGui::GetOverlayDrawList()->AddText(ImVec2(vHeadBoneOut2.x - TextSize.x / 2, vHeadBoneOut2.y - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), Out.c_str());
			}

			if (settings::visuals::weapon)
			{
				auto CurrentWeapon = driver->read<uintptr_t>(entity.Actor + 0x790);
				auto ItemData = driver->read<DWORD_PTR>(CurrentWeapon + 0x3d8);

				auto DisplayName = driver->read<uint64_t>(ItemData + 0x90);
				auto WeaponLength = driver->read<uint32_t>(DisplayName + 0x38);
				wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

				driver->readsize((ULONG64)driver->read<PVOID>(DisplayName + 0x30), WeaponName, WeaponLength * sizeof(wchar_t));
				std::string Text = wchar_to_char(WeaponName);


				ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());
				ImGui::GetOverlayDrawList()->AddText(ImVec2(vBaseBoneOut2.x - TextSize.x / 2, vBaseBoneOut2.y - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), Text.c_str());
			}

			if (settings::exploits::spinbot)
			{
				auto Mesh = driver->read<uint64_t>(LocalPawn + 0x2f0);
				static auto Cached = driver->read<Vector3>(Mesh + 0x140);

				if (GetAsyncKeyState(VK_RBUTTON)) {
					driver->write<Vector3>(Mesh + 0x140, Vector3(1, rand() % 361, 1));
				}
				else driver->write<Vector3>(Mesh + 0x140, Cached);
			}

			if (settings::exploits::doublepump)
			{
				uintptr_t CurrentWeapon = driver->read<uintptr_t>(LocalPawn + 0x790);
				if (CurrentWeapon) {
					driver->write<bool>(CurrentWeapon + 0xf41, true);
				}
			}

			if (settings::exploits::playerfly)
			{
				driver->write<float>(LocalPawn + 0x1ef0, 1000.f);
				driver->write<bool>(LocalPawn + 0x1ea0 + 0x18, true);
			}

			if (settings::exploits::carfly)
			{
				uintptr_t CurrentVehicle = driver->read<DWORD_PTR>(Globals::LocalPawn + 0x2158);

				if (CurrentVehicle && GetAsyncKeyState(VK_SPACE))
				{
					driver->write<bool>(CurrentVehicle + 0x668, false);
				}
				else
				{
					driver->write<bool>(CurrentVehicle + 0x668, true);
				}
			}


			auto dx = vHeadBoneOut.x - (Width / 2);
			auto dy = vHeadBoneOut.y - (Height / 2);
			auto dist = sqrtf(dx * dx + dy * dy);

			if (dist < settings::aimbot::fov && dist < closestDistance)
			{
				if (settings::aimbot::vischeck)
				{
					if (isVisible(entity.MeshComponent))
					{
						closestDistance = dist;
						closestPawn = entity.Actor;
					}
				}
				else
				{
					closestDistance = dist;
					closestPawn = entity.Actor;
				}
			}
		}
	}
	
	if (settings::aimbot::aimbot) if (closestPawn && GetAsyncKeyState(VK_RBUTTON) < 0) CallAimbot(closestPawn);
}