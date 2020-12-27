#include "Window.hpp"

int WINAPI WinMain(
	HINSTANCE hInstance,		// Instance of this application in memory
	HINSTANCE hPrevInstance,	// Always 0.
	LPSTR pCmdLine,				// String of command line.
	int nCmdShow				// Minimized, maximized or shown normally.
)
{
	::MessageBoxA(0, "Warning: If you suffer from any form of epilepsy or have experienced seizures in the past, please refrain from using this application. You are liable for any issues caused.", "Health & Safety Warning", MB_ICONWARNING);

	WindowManager* pWindow = new WindowManager(L"OpenDream", 1600, 900, 60);
	pWindow->Initialise(hInstance, L"OpenDreamDXInstance001");
	pWindow->InitialiseImGui_Dark();
	pWindow->Show();
	pWindow->Destroy();

	return 0;
}