#pragma once

class Hooks {
public:
	void init() noexcept;
	void initMemory() noexcept;
	void create() noexcept;
	void enable() noexcept;
	void disable() noexcept;
};

