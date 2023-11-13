#pragma once

#include "Entity.h"
#include "Player.h"

class LocalPlayer{
public:
	void init(uintptr_t address) noexcept {
		player = reinterpret_cast<Player*>(address);
	}

	void init(Player* address) noexcept {
		player = address;
	}

	void init(Entity* address) noexcept {
		player = reinterpret_cast<Player*>(address);
	}

	constexpr operator bool() noexcept {
		return player != nullptr;
	}

	constexpr auto operator->() noexcept {
		return player;
	}

	Player* get() {
		return player;
	}

	uintptr_t getPtr() {
		return reinterpret_cast<uintptr_t>(player);
	}
private:
	Player* player = 0;
};

inline LocalPlayer localPlayer;