#include "../pch.h"
#include "hooks.h"
#include <stdio.h>

#include "SDK/Entity.h"

#include <string>
#include <ostream>
#include <iostream>

#include "../minhook/include/MinHook.h"
#include "MemoryH.h"

namespace Offsets {
    constexpr uintptr_t screenUpdate = 0x6140; // 8b 0d ? ? ? ? 85 c9 74 ? 83 e9 ? 74 ? 83 e9 ? 74 ? 83 f9 ? 75 ? e8
    constexpr uintptr_t userMove = 0x1b15; // 40 57 48 83 ec

    constexpr uintptr_t dwLocalPlayer = 0x10BC0;
}

LocalPlayer* localPlayer;
typedef void(__fastcall* UpdateScreenFunction)(void* thisPtr);
typedef void(__fastcall* UserMoveFunction)(LocalPlayer* thisPtr);

UpdateScreenFunction updateScreenFunction;
UpdateScreenFunction updateScreenFunctionOriginal;

UserMoveFunction userMoveFunction;
UserMoveFunction userMoveFunctionOriginal;


void updateStuff() {
    localPlayer = *reinterpret_cast<LocalPlayer**>(memoryHooks->base + Offsets::dwLocalPlayer);
}

void __fastcall updateScreen(void* thisPtr) {
    updateStuff();
    Helpers::printText(0, 1, "Ukryta wiadomosc po wstrzyknieciu programu trzeciego: JXU284");
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
        Helpers::printText(0, 1, "");
        printf("LocalPlayer: 0x%p", localPlayer);
        Helpers::printText(0, 2, "");
        printf("Health: %i", localPlayer->health());

    }
    updateScreenFunctionOriginal(thisPtr);
}

void __fastcall userMove(LocalPlayer* rdi) {
    rdi->health() = 1337;
    userMoveFunctionOriginal(rdi);
}

void Hooks::init() noexcept {
    memoryHooks = std::make_unique<const MemoryH>();
    MH_Initialize();
    bool initHook = false;
    do {
        create();
        enable();
        initHook = true;
    } while (!initHook);
}

void Hooks::create() noexcept {
    updateScreenFunction = UpdateScreenFunction(memoryHooks->base + memoryHooks->update);
    userMoveFunction = UserMoveFunction(memoryHooks->base + memoryHooks->userMove);
    MH_CreateHook(updateScreenFunction, &updateScreen, reinterpret_cast<LPVOID*>(&updateScreenFunctionOriginal));
    MH_CreateHook(userMoveFunction, &userMove, reinterpret_cast<LPVOID*>(&userMoveFunctionOriginal));
}

void Hooks::enable() noexcept {
    MH_EnableHook(MH_ALL_HOOKS);
}

void Hooks::disableAndExit() noexcept {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
