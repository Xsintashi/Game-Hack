#pragma once

#include <Windows.h>
#include <stdio.h>

inline void printOnScreen(const int& x, const int& y, const char* str, ...) noexcept {
    COORD pos = { static_cast<short>(x), static_cast<short>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf(str);
}