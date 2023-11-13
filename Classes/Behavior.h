#pragma once

#include "Entity.h"

enum class Reason : char {
	NOT_BANNED = 0,
	MEMORY,
	DEBUGGER_ATTACH,
	DEBUGGER_SLEEP,
};

class Behavior {
private:
	[[maybe_unused]] std::byte __pad0x01[0xAA];
public:
	bool trusted;
	int level;
private:
	[[maybe_unused]] std::byte __pad0x02[0x10];
public:
	unsigned long long time;
	Reason reason;
};