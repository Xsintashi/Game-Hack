#pragma once

class LocalPlayer {
public:
	int& health() {
		return *reinterpret_cast<int*>(this + 0x3B4);
	}
};