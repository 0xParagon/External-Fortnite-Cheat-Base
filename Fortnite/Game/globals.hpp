#pragma once
#include <Windows.h>

namespace Fortnite 
{
    DWORD process_id;
    DWORD64 image_base;
    DWORD_PTR world;
}

namespace offsets
{
    DWORD
        Gameinstance = 0x1a8,
        LocalPlayers = 0x38,
        PlayerController = 0x30,
        PlayerCameraManager = 0x328,
        AcknowledgedPawn = 0x310,
        PlayerState = 0x290,
        TeamID = 0x1010,
        GameState = 0x148,
        PlayerArray = 0x288,
        PrivatePawn = 0x2e0,
        RootComponent = 0x188,
        RelativeLocation = 0x128,
        RelativeRotation = 0x140,
        MeshComponent = 0x2f0,
        FOVChanger = 0x288;
}

bool InLobby = false;

namespace settings
{
    inline bool show_menu = true;
    inline bool fovchanger = false;
    inline float fovchangervalue = 100.0f;

    namespace aimbot 
    {
        inline bool aimbot = true;
        inline float fov = 150;
        inline float smoothness = 7.0f;
        inline bool drawfov = true;
        inline bool vischeck = true;
    }

    namespace visuals 
    {
        inline bool snapline = false;
        inline bool distance = true;
        inline bool box = true;
        inline bool weapon = true;
    }

    namespace exploits
    {
        inline bool playerfly = false;
        inline bool doublepump = false;
        inline bool spinbot = false;
        inline bool carfly = false;
    }
}