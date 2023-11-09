#pragma once

#include "../pch.h"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>
#include <bitset>
#include "../Helpers.h"
#include <format>
#include <iomanip>

class CommandHandler
{
public:
	bool execute(std::string& command, Memory* mem) {
		std::stringstream ss(command);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> slices(begin, end);

		if (slices[0] == "version") {
			printf("Build: (" __CONFIGURATION__ ") " __DATE__ " at " __TIME__);
			goto finish;
		}
		else if (slices[0] == "debug") {
			debugScreen(mem);
			goto finish;
		}

		if (slices.size() < 2) {
			printf("Not enough arguments for: %s\n", slices[0].c_str());
			return false;
		}

		if (slices[0] == "echo")
			for (size_t i = 1; i < slices.size(); i++)
				printf(slices[1].c_str());

		else printf("Unknown command: %s", slices[0].c_str());

		finish:

		keybd_event(VK_SPACE,
			0x45,
			KEYEVENTF_EXTENDEDKEY | 0,
			0);

		command.erase();
		printf("\n");

		return true;
	}
private:
	void debugScreen(Memory* mem) noexcept {
		system("cls");
		Sleep(100);
		uintptr_t baseAddress = mem->GetModuleBaseAddress(__GAME__);

		const auto& localPlayer = mem->Read<uintptr_t>(baseAddress + 0x8788);

		if (!localPlayer) {
			printf("LocalPlayer not found! \n");
			return;
		}

		while (!(GetAsyncKeyState(VK_END) & 0x8000)) {
			Sleep(100);
			for (int i = 0; i < 8; i++) {
				Helpers::printText(0, i, "                        ");
			}
			const auto& health = mem->Read<int>(localPlayer + 0x30C);
			const auto& UserModeMask = mem->Read<int>(localPlayer + 0x310);
			const auto& UserModeButtons = mem->Read<int>(localPlayer + 0x338);
			//const auto& UserModeDirection = mem->Read<short>(localPlayer + 0x344);

			const std::string byteArrayBackup = std::bitset<8>(UserModeMask).to_string();
			const std::string byteArrayButtons = std::bitset<8>(UserModeButtons).to_string();

			Helpers::printText(0, 0, "Debug: (Press end to Stop!)");
			Helpers::printText(0, 2, std::string("LocalPlayer: 0x").append(std::format("{:x}", localPlayer)).c_str());
			Helpers::printText(0, 3, std::string(" Health: ").append(std::to_string(health)).c_str());
			Helpers::printText(0, 4, std::string(" UserMode:").c_str());
			Helpers::printText(0, 5, std::string("  Backup:  ").append(byteArrayBackup).append(" | ").append(std::to_string(UserModeMask)).c_str());
			Helpers::printText(0, 6, std::string("  Buttons: ").append(byteArrayButtons).append(" |  ").append(std::to_string(UserModeButtons)).c_str());
		}
	}
};
