#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "cmd/cmd.h"
#include "hooks/hooks.h"
#include <thread>

DWORD WINAPI entryThread(LPVOID param) noexcept {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string ss = buffer;
    std::size_t found = ss.find_last_of("/\\");
    ss = ss.substr(found + 1);

    if (strcmp(ss.c_str(), "cmd.exe") == 0) { // Injected to cmd process
        CMD::init();
    } else if (strcmp(ss.c_str(), __GAME__)) { // Injected to different process than game or cmd
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        ShowWindow(GetConsoleWindow(), SW_SHOW);

        Memory mem = Memory{ __GAME__ };
        uintptr_t baseAddress = mem.GetModuleBaseAddress(__GAME__);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD(0, 1));
        printf("Game: %s\nBase: 0x%x\n", __GAME__, (unsigned)baseAddress);
    } else { // Injected to game !!!
        Hooks hook;
        hook.init();
        while (1) {
            Sleep(1);
            if (GetAsyncKeyState(VK_HOME)) {
                hook.disableAndExit();
                CloseHandle(param);
                FreeLibraryAndExitThread(0, 0);
                return 1;
            }
        }
    }
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD(0, 0));
    printf("[%s] DLL Hack Example build: (" __CONFIGURATION__ ") " __DATE__ " at " __TIME__ "\n", ss.c_str());
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  reason, LPVOID lpReserved ) {
    switch (reason){
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            //MessageBoxA(NULL, "Hack injected!", "DLL Hack Example", MB_OK | MB_ICONINFORMATION);
            CreateThread(0, 0, entryThread, hModule, 0, 0);
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

