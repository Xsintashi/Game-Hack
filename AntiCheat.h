#pragma once

#include "Classes/Entity.h"
#include "Classes/Player.h"
#include "Classes/GlobalVars.h"
#include "Data.h"

#include <chrono>
#include <debugapi.h>
#include <thread>
#include "Lock.h"

class GlobalVars;

// Ghetto Anticheat LOL
class AntiCheat {
public:

	bool overSleep() noexcept {
		while (isRunning || turned) {
			static auto buffTick = GetTickCount64();
			THREAD_SLEEP(1000);
			if (GetTickCount64() - buffTick > 1500) { // Slept a lil too much
				player->behavior->trusted = false;
				isRunning = false;
				return true;
			}
			buffTick = GetTickCount64();
		}
		return false;
	}

	void thread() noexcept {

		while (isRunning || turned) {
			THREAD_SLEEP(128);
			Lock lock;
			auto& lvl = player->behavior->level;
			if (static_cast<int>(data.globalVars->tickCount) % 10 == 1) {
				lvl++;
				lvl = std::clamp(lvl, 0, 100);
			}
#if !defined(_DEBUG)
			if (IsDebuggerPresent()) {
				// Debugger is present closing game in a few seconds
				player->behavior->level = 1;
				THREAD_SLEEP((static_cast<DWORD>(data.globalVars->tickCount * 100) % 15) * 1000);
				player->behavior->trusted= false;
				isRunning = false;
			}
#endif
			const auto lUserMove = player->userMove;
			if (player->health < 101 && verifyUserMove(lUserMove->buttons, lUserMove->backup))
				continue;

			incorrections++;
			lvl = 100 - incorrections;
			if (lvl < 1) {
				player->behavior->trusted = false;
				isRunning = false;
			}
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
	AntiCheat(Player* localPlayer) {
		player = localPlayer;
		turned = true;
	}

	~AntiCheat() {
		// AC has been turned off
		incorrections = 0;
		turned = false;
	}
private:

	Player* player;
};

