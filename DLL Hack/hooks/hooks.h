#pragma once

class Hooks {
public:
	void init() noexcept;
	void create() noexcept;
	void enable() noexcept;
	void disableAndExit() noexcept;
};

