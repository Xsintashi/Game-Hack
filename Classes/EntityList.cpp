#include "EntityList.h"
#include "Entity.h"

void EntityList::addEntity(Entity* entity) noexcept {
    this->entitylist.push_back(entity);
    this->maxEntityIndex++; 
}

bool EntityList::removeEntity(Entity* entity)  noexcept {
    if (this->entitylist.empty())
        return false;

    this->entitylist.erase(std::find(this->entitylist.begin(), this->entitylist.end(), entity));
    this->maxEntityIndex--;
    delete entity;
    return true;
}

bool EntityList::removeEntity(idx index) noexcept {
    if (this->entitylist.empty())
        return false;
    const auto& toDelete = this->entitylist[index];
    this->entitylist.erase(this->entitylist.begin() + index);
    this->maxEntityIndex--;
    delete toDelete;
    return true;
}

std::vector<Entity*> EntityList::findEntitiesByName(std::string name) noexcept {

    EntityList::Searcher searcher;

    for (const auto& it : this->entitylist) {
        if (it->identify->name == name) {
            if (it == nullptr)
                continue;
            searcher.result.push_back(it);
            searcher.size++;
        }
    }

    return searcher.result;
}

std::vector<Entity*> EntityList::findEntitiesByClassName(std::string classname) noexcept {

    EntityList::Searcher search;

    for (const auto& it : this->entitylist) {
        if (it->identify->classname == classname) {
            if (it == nullptr)
                continue;
            search.result.push_back(it);
            search.size++;
        }
    }
    return search.result;
}


int EntityList::findIndexByAddress(Entity* address) noexcept {
    if (this->entitylist.empty() || address == nullptr)
        return -1;

    int i = 0;
    for (const auto& it : this->entitylist) {
        if (it == address)
            return i;
        i++;
    }

    return -1;
}

Entity* EntityList::getEntity(idx index) noexcept {
    if (this->entitylist.empty())
        return 0;
    return this->entitylist[index];
}
