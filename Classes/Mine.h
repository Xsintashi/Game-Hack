#pragma once

#include <ctime>


#include "Entity.h"
#include "Vector.h"

class Mine : public Entity {
public:
	Mine(int r) {
		identify = new EntityIdentify;
		identify->name = "Mine";
		identify->classname = "entity_mine";
		damage = r % 25;
		origin = { r % 74 , r % 26 };
		draw = false;
		charDraw = '*';
	}
	void recreate(int r) {
		damage = r % 25;
		origin = { r % 74 , r % 26 };
	}
private:
	[[maybe_unused]] std::byte __pad0x02[0x2D];
public:
	int damage;
private:
	[[maybe_unused]] std::byte __pad0x03[0x10];
public:	
};