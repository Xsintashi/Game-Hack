#pragma once

#include "SDK/Entity.h"
#include "SDK/Player.h"
#include "SDK/GlobalVars.h"
#include "Data.h"

#include <chrono>
#include <debugapi.h>
#include <thread>
#include "Lock.h"

class GlobalVars;

// Ghetto Anticheat LOL
class AntiCheat {
public:
	void thread() noexcept {
		while (isRunning || turned) {
			THREAD_SLEEP(128);
			Lock lock;
			auto& lvl = data.localPlayer->behavior->level;
			if (static_cast<int>(data.globalVars->tickCount) % 10 == 1) {
				lvl++;
				lvl = std::clamp(lvl, 0, 100);
			}
#if !defined(_DEBUG)
			if (IsDebuggerPresent()) {
				// Debugger is present closing game in a few seconds
				THREAD_SLEEP((static_cast<DWORD>(data.globalVars->tickCount * 100) % 15) * 1000);
				data.localPlayer->behavior->trusted= false;
				isRunning = false;
			}
#endif
			const auto lUserMove = data.localPlayer->userMove;
			if (data.localPlayer->health < 101 && verifyUserMove(lUserMove.buttons, lUserMove.backup))
				continue;

			incorrections++;
			lvl = 100 - incorrections;
			if (lvl < 1) {
				data.localPlayer->behavior->trusted = false;
				isRunning = false;
			}
		}
	}

	bool overSleep() noexcept {
		while (isRunning || turned) {
			THREAD_SLEEP(1000);
			static auto buffTick = GetTickCount64();
			if (GetTickCount64() - buffTick > 1500) // Slept a lil too much
				return true;
			return false;
		}
	}

	bool verifyUserMove(int buttons, int backup) noexcept {
		constexpr unsigned mask = 0xFFFFFFFF;
		int supposedBackup = buttons ^ mask;
		return backup == supposedBackup;
	}
	int incorrections = 0;
	bool turned = false;
public:
	AntiCheat() {
		turned = true;
	}

	~AntiCheat() {
		// AC has been turned off
		incorrections = 0;
		turned = false;
	}
};

