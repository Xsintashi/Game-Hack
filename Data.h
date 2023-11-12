#pragma once

class GlobalVars;
class Player;
class Mine;
class Bullet;
class EntityList;

#define THREAD_SLEEP(time)			std::this_thread::sleep_for(std::chrono::milliseconds(time))

struct Data {
	GlobalVars* globalVars;
	Player* enemy;
};

inline Data data;
inline EntityList* entityList;

inline bool isRunning = true;