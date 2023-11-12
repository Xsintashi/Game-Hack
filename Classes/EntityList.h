#pragma once

#include <vector>
#include <string>

class Entity;

class EntityList {
private:
	typedef unsigned idx;
	idx maxEntityIndex;

	std::vector<Entity*> entitylist;

public:

	struct Searcher {
		int size = 0;
		std::vector<Entity*> result;
	};

	void addEntity(Entity* entity) noexcept;
	bool removeEntity(Entity* entity) noexcept;
	bool removeEntity(idx index) noexcept;
	std::vector<Entity*> findEntitiesByName(std::string name) noexcept;
	std::vector<Entity*> findEntitiesByClassName(std::string classname) noexcept;
	int findIndexByAddress(Entity* address) noexcept;
	Entity* getEntity(idx index) noexcept;

	std::vector<Entity*> get() {
		return this->entitylist;
	}

	int getHighestEntityIndex() {
		return this->maxEntityIndex;
	}

	EntityList() = default;
};

