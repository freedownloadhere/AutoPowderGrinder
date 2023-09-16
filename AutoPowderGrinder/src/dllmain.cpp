#include "AutoPowderGrinder.h"

void MainThread(HINSTANCE instance)
{
	std::unique_ptr<apg::AutoPowderGrinder> myPowderGrinder = std::make_unique<apg::AutoPowderGrinder>();

	myPowderGrinder->run();

	FreeLibrary(instance);
}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	static FILE* file{ nullptr };
	static std::thread mainThread{};

	if (reason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		//SetConsoleOutputCP(CP_UTF8);0000
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

