#include "AutoPowderGrinder.h"

void MainThread(HINSTANCE instance)
{
	std::unique_ptr<apg::AutoPowderGrinder> myPowderGrinder = std::make_unique<apg::AutoPowderGrinder>();

	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		myPowderGrinder->run();
	}

	FreeLibrary(instance);
}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	static FILE* file{ nullptr };
	static std::thread mainThread{};

	if (reason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		freopen_s(&file, "CONOUT$", "w", stdout);

		mainThread = std::thread(MainThread, instance);
		mainThread.detach();
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		fclose(file);
		FreeConsole();
	}

	return 1;
}

