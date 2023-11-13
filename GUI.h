#pragma once

#include <Windows.h>

enum class Stage : int {
	MAIN_MENU = 0,
	LOGIN,
	REGISTER,
	GAME
};

namespace GUI {
	inline Stage stage = Stage::MAIN_MENU;

	void clearScreen() noexcept;
	void showConsoleCursor(bool flag) noexcept;
	void printOnScreen(const int& x, const int& y, const char* str) noexcept;
	void printOnScreenCentered(const int& y, const char* str) noexcept;
	void setWindow(const DWORD& width, const DWORD& height) noexcept;
	void render() noexcept;
}