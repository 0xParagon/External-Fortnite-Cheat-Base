#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>

#include "driver/loader.hpp"
#include "driver/driver.hpp"

#include "overlay/render.hpp"
#include "overlay/menu.hpp"

#include "game/cheat.hpp"

#include "../Includes/Auth/auth.hpp"
#include <string>

using namespace KeyAuth;

// i dont suggest using keyauth, you can just create session skips
std::string name = XorStr("").c_str(); 		// Application ID
std::string ownerid = XorStr("").c_str(); 	// Owner Profile ID
std::string secret = XorStr("").c_str(); 	// Application UID 
std::string version = XorStr("1.0").c_str();
std::string url = XorStr("https://keyauth.win/api/1.1/").c_str();
std::string sslPin = XorStr("ssl pin key (optional)").c_str();
api KeyAuthApp(name, ownerid, secret, version, url, sslPin);

auto main() -> NTSTATUS
{
	std::cout << XorStr("\n Connecting...").c_str();

	KeyAuthApp.init();
	if (!KeyAuthApp.data.success)
	{
		Sleep(1500);
		exit(0);
	}

	std::string key;

	std::cout << XorStr("\n Enter License: ").c_str();
	std::cin >> key;
	KeyAuthApp.license(key);

	if (!KeyAuthApp.data.success)
	{
		Sleep(1500);
		exit(0);
	}
	else
	{
		if (GlobalFindAtomA(XorStr("DriverAlreadyLoaded").c_str()) == 0)
		{
			system(XorStr("taskkill /F /IM EpicGamesLauncher.exe").c_str());
			system(XorStr("taskkill /F /IM EasyAntiCheatLauncher.exe").c_str());
			system(XorStr("taskkill /F /IM BEService.exe").c_str());
			system(XorStr("taskkill /F /IM BattleEyeLauncher.exe").c_str());
			system(XorStr("taskkill /F /IM FortniteClient-Win64-Shipping.exe").c_str());
			system(XorStr("taskkill /F /IM FortniteLauncher.exe").c_str());

			load_driver();
			system(XorStr("cls").c_str());
			GlobalAddAtomA(XorStr("DriverAlreadyLoaded").c_str());
		}

		InjectMouseInput = reinterpret_cast<InjectMouseInput_t>(GetProcAddress(LoadLibraryA("user32.dll"), "InjectMouseInput"));
		if (!InjectMouseInput)
		{
			printf(XorStr("Failed to load user32.dll\n").c_str());
		}

		HWND Entryhwnd = NULL;

		while (Entryhwnd == NULL)
		{
			printf(XorStr("Waiting for Fortnite...\r").c_str());
			Sleep(1);
			Fortnite::process_id = getprocessid(XorStr("FortniteClient-Win64-Shipping.exe").c_str());
			Entryhwnd = get_process_wnd(Fortnite::process_id);
			Sleep(1);
		}

		system(XorStr("cls").c_str());

		driver = std::make_unique<drv>(XorStr("EasyAntiCheat_x64.dll").c_str(), XorStr("FortniteClient-Win64-Shipping.exe").c_str());

		if (driver != NULL)
		{
			setup_window();
			init_wndparams(MyWnd);

			Fortnite::image_base = driver->get_base_address(); if (!Fortnite::image_base) printf(XorStr("Failed to get Base Address!\n").c_str());
			Fortnite::world = GetWorld("\x48\x89\x05\x00\x00\x00\x00\x48\x8B\x4B\x78", "xxx????xxxx");


			printf(XorStr("Loaded Successfully!\n").c_str());

			std::thread(CacheGame).detach();

			while (true) main_loop();
		}
	}

	exit(0);
}

auto render() -> void 
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (GetAsyncKeyState(VK_F1) & 1) { settings::show_menu = !settings::show_menu; }

	cheatloop();
	drawmenu();

	ImGui::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (p_Device->BeginScene() >= 0) 
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}

	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) 
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&d3dpp);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

auto main_loop() -> WPARAM
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT) 
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom) {

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		render();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_d3d();
	DestroyWindow(MyWnd);

	return Message.wParam;
}
