#pragma once
#include <string>

#include "Vector.h"

class EntityIdentify {
public:
	std::string classname;
	std::string name;
};

class Entity {
public: 
	Entity() {
		address = (uintptr_t)this;
	}
	uintptr_t address;
	EntityIdentify* identify;
private:
	[[maybe_unused]] std::byte __pad0x01[0xFA];
public:
	bool draw;
	char charDraw;
private:
	[[maybe_unused]] std::byte __pad0x02[0xFC];
public:
	Vector2D origin;
};