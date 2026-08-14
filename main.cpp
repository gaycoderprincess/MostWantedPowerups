#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <mutex>
#include <random>
#include <thread>
#include <toml++/toml.hpp>

#include "nya_dx9_hookbase.h"
#include "nya_commonhooklib.h"
#include "nya_commonmath.h"
#include "nfsmw.h"

#include "include/chloemenulib.h"
#include "include/cwoeemodel.h"

std::vector<void(*)()> aMainLoopFunctions;
std::vector<void(*)()> aMainLoopFunctionsOnce;
std::vector<void(*)()> aDrawingLoopFunctions;
std::vector<void(*)()> aDrawingLoopFunctionsOnce;
std::vector<void(*)()> aDrawing3DLoopFunctions;
std::vector<void(*)()> aDrawing3DLoopFunctionsOnce;
std::vector<void(*)()> aPlayerTeleportFunctions;
std::vector<void(*)()> aPlayerDestroyFunctions;

#include "util.h"
#include "d3dhook.h"
#include "chaosvars.h"
#include "chaospopup.h"
#include "components/render3d.h"
#include "components/render3d_objects.h"
#include "hooks/carrender.h"
#include "hooks/catchup.h"
#include "components/hints.h"

#include "hooks/input.h"

#include "include/surface_terrains.h"
#include "include/audio_defines.h"
#include "include/sm64_defs.h"
#include "include/libsm64.h"
#include "box3d/box3d.h"
#include "components/collisioncache.h"
#include "components/customphysics.h"
#include "components/customphysics_objects.h"
#include "components/sm64.h"
#include "components/powerup.h"
#include "components/updatecheck.h"

void MainLoop() {
	PerformanceBenchmarker _perf("MainLoop");

	for (auto& func : aMainLoopFunctions) {
		func();
	}

	for (auto& func : aMainLoopFunctionsOnce) {
		func();
	}
	aMainLoopFunctionsOnce.clear();
}

void Render3DLoop(eView* view) {
	PerformanceBenchmarker _perf("Render3DLoop");

	Render3D::pViewToDraw = view;
	//WriteLog(std::format("view {:X} id {}", (uintptr_t)view, (int)view->ID));

	static IDirect3DStateBlock9* state = nullptr;
	if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &state) != D3D_OK) {
		return;
	}

	if (state->Capture() < 0) {
		state->Release();
		return;
	}

	Render3D::BeginRendering();

	for (auto& func : aDrawing3DLoopFunctions) {
		func();
	}

	for (auto& func : aDrawing3DLoopFunctionsOnce) {
		func();
	}
	aDrawing3DLoopFunctionsOnce.clear();

	Render3D::FinalizeRendering();

	state->Apply();
	state->Release();
}

void Render3DLoopMain() {
	Render3DLoop(&eViews[EVIEW_PLAYER1]);
}

void Render3DLoopShadows() {
	Render3DLoop(&eViews[EVIEW_SHADOWMAP1]);
}

void ChaosLoop() {
	PerformanceBenchmarker _perf("ChaosLoop");

	static bool bOnce = true;
	if (bOnce) {
		NyaAudio::Init(GameWindow);
		bOnce = false;
	}

	for (auto& func : aDrawingLoopFunctions) {
		func();
	}

	for (auto& func : aDrawingLoopFunctionsOnce) {
		func();
	}
	aDrawingLoopFunctionsOnce.clear();

	static CNyaTimer gTimer;
	gTimer.Process();
	DrawPerformanceWarnings(gTimer.fDeltaTime);

	DrawLogPopups();
}

void DebugMenu() {
	ChloeMenuLib::BeginMenu();

#ifndef CWOEE_NO_UPDATER
	if (UpdateChecker::bUpdateAvailable) {
		if (DrawMenuOption("Update available!") || DrawMenuOption("Click here to update")) {
			UpdateChecker::OpenUpdatePage();
		}
	}
#endif

	QuickValueEditor("bDebugPrintsEnabled", bDebugPrintsEnabled);
	QuickValueEditor("Overhead Display", Powerups::bOverheadDisplay);
	QuickValueEditor("Powerups Style", Powerups::bMK64Style, "Re-Volt", "Mario Kart 64");

	if (DrawMenuOption("Give Powerup")) {
		ChloeMenuLib::BeginMenu();

		for (int i = 0; i < Powerups::NUM_POWERUPS; i++) {
			if (DrawMenuOption(std::format("give {}", Powerups::aPowerupNames[i]))) {
				Powerups::RollPowerup(GetLocalPlayerVehicle());
				Powerups::GetPowerupState(GetLocalPlayerVehicle())->GivePowerup(i);
			}
		}

		ChloeMenuLib::EndMenu();
	}

	ChloeMenuLib::EndMenu();
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x3C4040) {
				MessageBoxA(nullptr, "Unsupported game version! Make sure you're using v1.3 (.exe size of 6029312 bytes)", "nya?!~", MB_ICONERROR);
				return TRUE;
			}

			srand(time(0));

			GetCurrentDirectoryW(MAX_PATH, gDLLDir);

			for (auto& func : ChloeHook::aHooks) {
				func();
			}

			if (std::filesystem::exists("NFSMWPowerups_gcp.toml")) {
				auto config = toml::parse_file("NFSMWPowerups_gcp.toml");
				Powerups::bMK64Style = config["style_mk64"].value_or(Powerups::bMK64Style);
				Powerups::bOverheadDisplay = config["show_overhead"].value_or(Powerups::bOverheadDisplay);
			}

			ChloeMenuLib::RegisterMenu("Cwoee Powerups", &DebugMenu);

			NyaHooks::PlaceD3DHooks(true);
			NyaHooks::D3DEndSceneHook::aFunctions.push_back(D3DHookMain);
			NyaHooks::D3DResetHook::aFunctions.push_back(OnD3DReset);
			NyaHooks::PreHUDDrawHook::aFunctions.push_back(D3DHookPreHUD);
			NyaHooks::WndProcHook::Init();
			NyaHooks::WndProcHook::aFunctions.push_back(WndProcHook);
			NyaHooks::WorldServiceHook::Init();
			NyaHooks::WorldServiceHook::aFunctions.push_back(MainLoop);
			NyaHooks::InputBlockerHook::Init();
			NyaHooks::LateInitHook::Init();
			NyaHooks::LateInitHook::aFunctions.push_back([](){
				NyaHooks::RenderEnvHook::Init();
				NyaHooks::RenderEnvHook::aPostFunctions.push_back(Render3DLoop);

				// memory corruption here if 360 stuff isnt installed
				if (GetModuleHandleA("X360Stuff.asi")) {
					Render3D::bShadowsAvailable = true;
					NyaHooks::RenderShadowsHook::Init();
					NyaHooks::RenderShadowsHook::aPostFunctions.push_back(Render3DLoopShadows);
				}

				if (!GetModuleHandleA("vulkan-1.dll") && !GetModuleHandleA("winevulkan.dll")) {
					MessageBoxA(nullptr, "WARNING: DXVK is not installed properly! Make sure you've placed d3d9.dll from the mod's archive into the game folder or you WILL encounter stability issues!", "nya?!~", MB_ICONERROR);
				}
			});
			NyaHooks::RenderWorldHook::Init();
			NyaHooks::RenderWorldHook::aPostFunctions.push_back(Render3DLoopMain);

			CustomPhysics::bEnabled = true;

			//SkipFE = true;
		} break;
		default:
			break;
	}
	return TRUE;
}
