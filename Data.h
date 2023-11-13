#pragma once

#include <string>

class GlobalVars;
class Player;
class Mine;
class Bullet;
class EntityList;

#define THREAD_SLEEP(time)			std::this_thread::sleep_for(std::chrono::milliseconds(time))

struct Data {
	GlobalVars* globalVars;
	bool turnDebug = false;

	std::string username;
};

inline Data data;
inline EntityList* entityList;

inline constexpr int NUMBER_OF_MINES = 10;

inline bool isRunning = true;