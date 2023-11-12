#pragma once

#include "Entity.h"

enum class WeaponID;

class WeaponData {
public:
	int maxFirstClip;
	int maxSecondaryClip;
};

class BaseWeapon : public Entity {
public:
	WeaponData* weaponData;
	short WeaponID;
};
