#include "cmd.h"
#include "../pch.h"
#include "../Memory.h"

#include <Windows.h>
#include <Windows.h>

void CMD::init() noexcept {
    system("cls");
    Sleep(10);
    Memory mem = Memory{ __GAME__ };
    uintptr_t baseAddress = mem.GetModuleBaseAddress(__GAME__);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD(0, 0));
    printf("Game: %s\nBase: 0x%x\n", __GAME__, (unsigned)baseAddress);

    CommandHandler cmd;

    while (1) {
        std::string command;
        std::getline(std::cin, command);
        cmd.execute(command, &mem);
    }
}