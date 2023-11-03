#pragma once

class GlobalVars;
class Player;
class Mine;

#define THREAD_SLEEP(time)			std::this_thread::sleep_for(std::chrono::milliseconds(time))

struct Data {
	GlobalVars* globalVars;
	Player* localPlayer;
	Mine* mine;
};

inline Data data;
inline bool isRunning = true;