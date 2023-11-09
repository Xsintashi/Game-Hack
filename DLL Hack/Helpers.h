#pragma once

#include "pch.h"

namespace Helpers {
	static void printText(int x, int y, const char* str) noexcept {
        COORD pos = { static_cast<short>(x), static_cast<short>(y) };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        printf(str);
    }
}