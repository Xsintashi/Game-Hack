#pragma once
#include <cstddef>
#include <string>
#include <array>

#include "Behavior.h"
#include "Entity.h"
#include "Statistics.h"
#include "UserMove.h"
#include "Vector.h"

class Player : public Entity {
public:
	Player(char name[64]) {
		identify = new EntityIdentify;
		identify->name = "Player";
		identify->classname = "entity_player";

		nickname = name;
		health = 100;
		origin = { 37,13 };
		address = uintptr_t(this);
		behavior = new Behavior;

		behavior->trusted = true;
		behavior->level = 100;
		behavior->reason = Reason::NOT_BANNED;
		behavior->time = 0;
	}
private:
	std::byte __pad0x01[0xFA];
public:
	int health;
	UserMove userMove;
	Statistics statistics;
private:
	std::byte __pad0x02[0x85];
public:
	bool isDead;
private:
	std::byte __pad0x03[0xA9];
public:
	std::string nickname;
private:
	std::byte __pad0x04[0xCC];
public:
	Behavior* behavior;
};