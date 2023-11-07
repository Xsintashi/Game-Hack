#pragma once

class GlobalVars;
class Player;
class Mine;
class Bullet;

#define THREAD_SLEEP(time)			std::this_thread::sleep_for(std::chrono::milliseconds(time))

struct Data {
	GlobalVars* globalVars;
	Player* localPlayer;
	Mine* mine;
	Bullet* bullet;
	Player* enemy;
};

inline Data data;
inline bool isRunning = true;