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
	uintptr_t address;
	EntityIdentify* identify;
private:
	std::byte __pad0x01[0xFA];
public:
	Vector2D origin;
};