#pragma once

#include "Entity.h"

class Point : public Entity {
public:
	Point(int r) {
		identify = new EntityIdentify;
		identify->name = "Point";
		identify->classname = "entity_scorepoint";
		origin = { r % 74 , r % 26 };
		draw = false;
		charDraw = '$';
	}
	void recreate(int r) {
		origin = { r % 74 , r % 26 };
	}
};