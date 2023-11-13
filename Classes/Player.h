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
	Player(std::string name, Vector2D pos) {
		identify = new EntityIdentify;
		identify->name = "Player";
		identify->classname = "entity_player";

		nickname = name;
		health = 100;
		origin = pos;
		address = uintptr_t(this);
		behavior = new Behavior;
		userMove = new UserMove;

		draw = true;
		charDraw = '#';

		behavior->trusted = true;
		behavior->level = 100;
		behavior->reason = Reason::NOT_BANNED;
		behavior->time = 0;
	}
private:
	[[maybe_unused]] std::byte __pad0x01[0xFA];
public:
	bool isDead;
	UserMove* userMove;
	Statistics statistics;
private:
	[[maybe_unused]] std::byte __pad0x02[0x85];
public:
	int health;
private:
	[[maybe_unused]] std::byte __pad0x03[0xA9];
public:
	std::string nickname;
private:
	[[maybe_unused]] std::byte __pad0x04[0xCC];
public:
	Behavior* behavior;

	void randomSpawn(int r) {
		origin = { r % 74 , r % 26 };
	}
};