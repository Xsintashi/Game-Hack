#include "../pch.h"
#include "hooks.h"
#include <stdio.h>

#include "../minhook/include/MinHook.h"

namespace Offsets {
    constexpr uintptr_t text = 0x5614; // 4c 89 6c 24 ? 48 c7 44 24 ? ? ? ? ? 44 88 6c 24 ? 44 89 6c 24
    constexpr uintptr_t hp = 0x203D; // 29 88
}

uintptr_t base = (uintptr_t)GetModuleHandle(__GAME__);
void* localPlayer;
typedef void(__stdcall* TextFunction)();
typedef void(__stdcall* HealthFunction)();

TextFunction textFunction = TextFunction(base + Offsets::text);
TextFunction textFunctionOriginal;

HealthFunction healthFunction = HealthFunction(base + Offsets::hp);
HealthFunction healthFunctionOriginal;

void updateStuff() {
    localPlayer = *reinterpret_cast<void**>(base + 0x10BC0);
}

void WINAPI textHook() {
    updateStuff();
    printf(" xs9 Hook dll Test! :D, LocalPlayer: 0x%p\n", localPlayer);
    textFunctionOriginal();
}

void WINAPI healthHook() {
    int& hp = *reinterpret_cast<int*>(reinterpret_cast<unsigned*>(localPlayer) + 0x3B4 /*Health*/);
    hp = 100;
    healthFunctionOriginal();

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
    MH_CreateHook(textFunction, &textHook, reinterpret_cast<LPVOID*>(&textFunctionOriginal));
    MH_CreateHook(healthFunction, &healthHook, reinterpret_cast<LPVOID*>(&healthFunctionOriginal));
}

void Hooks::enable() noexcept {
    MH_EnableHook(MH_ALL_HOOKS);
}

void Hooks::disable() noexcept{
    MH_DisableHook(MH_ALL_HOOKS);
}
