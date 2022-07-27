#pragma once
#include "../../Includes/strings.hpp"

auto drawmenu() -> void
{
	if (settings::show_menu)
	{
		ImGui::Begin(XorStr("Fortnite").c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::SetWindowSize(XorStr("Fortnite").c_str(), ImVec2(300, 450));

		ImGui::Checkbox(XorStr("Enable Box").c_str(), &settings::visuals::box);
		ImGui::Checkbox(XorStr("Enable Snaplines").c_str(), &settings::visuals::snapline);
		ImGui::Checkbox(XorStr("Enable Distance").c_str(), &settings::visuals::distance);
		ImGui::Checkbox(XorStr("Enable Weapon ESP").c_str(), &settings::visuals::weapon);

		ImGui::Checkbox(XorStr("Enable Aimbot").c_str(), &settings::aimbot::aimbot);
		ImGui::Checkbox(XorStr("Visible Check").c_str(), &settings::aimbot::vischeck);
		if (settings::aimbot::aimbot)
		{
			ImGui::Checkbox(XorStr("Draw FOV").c_str(), &settings::aimbot::drawfov);
			ImGui::Text(XorStr("Aimbot FOV").c_str());
			ImGui::SliderFloat("  ", &settings::aimbot::fov, 10, 1000);
			ImGui::Text(XorStr("Smoothness").c_str());
			ImGui::SliderFloat(" ", &settings::aimbot::smoothness, 1, 20);
		}
		ImGui::Checkbox(XorStr("Enable FOV Changer").c_str(), &settings::fovchanger);
		if (settings::fovchanger)
		{
			ImGui::Text(XorStr("Value").c_str());
			ImGui::SliderFloat("   ", &settings::fovchangervalue, 50, 150);
		}
		ImGui::End();

		ImGui::Begin(XorStr("Exploits").c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowSize(XorStr("Exploits").c_str(), ImVec2(300, 450));

		ImGui::Checkbox(XorStr("Enable Carfly").c_str(), &settings::exploits::carfly);
		ImGui::Checkbox(XorStr("Enable Double Pump").c_str(), &settings::exploits::doublepump);
		ImGui::Checkbox(XorStr("Enable Playerfly").c_str(), &settings::exploits::playerfly);
		ImGui::Checkbox(XorStr("Enable Spinbot").c_str(), &settings::exploits::spinbot);

		ImGui::End();
	}
}