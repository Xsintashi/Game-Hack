#pragma once
#include <cstddef>

enum UserMoveButtons {
	ATTACK		= ( 1 << 0),
	DEFEND		= ( 1 << 1),
	FORWARD		= ( 1 << 2),
	BACKWARD	= ( 1 << 3),
	LEFT		= ( 1 << 4),
	RIGHT		= ( 1 << 5),
	ROTATELEFT	= ( 1 << 6),
	ROTATERIGHT	= ( 1 << 7),
};

enum class Rotations : char {
	LEFT = 0,
	RIGHT,
	UP,
	DOWN
};

class UserMove {
public:
	int backup = -1;
private:
	std::byte __pad0x01[0x21];
public:
	int buttons;
private:
	std::byte __pad0x02[0x9];
public:
	Rotations rotation;
};