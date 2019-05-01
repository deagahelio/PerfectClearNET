#include "Windows.h"
#define DLL extern "C" __declspec(dllexport)

BOOL WINAPI DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
	switch (reason) {
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}