#include "../pch.h"
#include "hooks.h"
#include "Utils.h"
#include <stdio.h>

#include "../minhook/include/MinHook.h"

namespace Offsets {
    constexpr uintptr_t screenUpdate = 0x61F0; // 8b 0d ? ? ? ? 85 c9 74 ? 83 e9 ? 74 ? 83 e9 ? 74 ? 83 f9 ? 75 ? e8

    constexpr uintptr_t dwLocalPlayer = 0x10BC0;
}

uintptr_t base = (uintptr_t)GetModuleHandle(__GAME__);
void* localPlayer;
typedef void(__stdcall* UpdateScreenFunction)();

UpdateScreenFunction updateScreenFunction = UpdateScreenFunction(base + Offsets::screenUpdate);
UpdateScreenFunction updateScreenFunctionOriginal;


void updateStuff() {
    localPlayer = *reinterpret_cast<void**>(base + Offsets::dwLocalPlayer);
}

void WINAPI updateScreen() {
    updateStuff();
    printOnScreen(0, 1, "xs9 Hook dll Test! :D");
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
        printOnScreen(0, 1, "LocalPlayer: 0x%p", localPlayer);
    }
    updateScreenFunctionOriginal();
}

void Hooks::init() noexcept {
    MH_Initialize();
    bool initHook = false;
    do {
        create();
        enable();
        initHook = true;
    } while (!initHook);
}

void Hooks::create() noexcept {
    MH_CreateHook(updateScreenFunction, &updateScreen, reinterpret_cast<LPVOID*>(&updateScreenFunctionOriginal));
}

void Hooks::enable() noexcept {
    MH_EnableHook(MH_ALL_HOOKS);
}

void Hooks::disable() noexcept{
    MH_DisableHook(MH_ALL_HOOKS);
}
