#pragma once

#include <cstdint>
#include <memory>

class MemoryH {
public:
	MemoryH() noexcept;
	uintptr_t base;

	uintptr_t update;
	uintptr_t userMove;
};

inline std::unique_ptr<const MemoryH> memoryHooks;