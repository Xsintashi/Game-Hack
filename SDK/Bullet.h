#pragma once
#include "Entity.h"
#include "Directions.h"
#include "Macros.h"
#include "Vector.h"

class Bullet : virtual public Entity {
public:
	Bullet(bool draw_, Directions d, Vector2D org) {
		identify = new EntityIdentify;
		identify->name = "Bullet";
		identify->classname = "entity_bullet";
		direction = d;
		origin = org;
		draw = draw_;

	}

	void update() {
		switch (direction) {
		default:
		case Directions::LEFT:
			origin.x--;
			break;
		case Directions::RIGHT:
			origin.x++;
			break;
		case Directions::UP:
			origin.y--;
			break;
		case Directions::DOWN:
			origin.y++;
			break;
		}
		if (origin.y <= TOPBORDER || origin.y >= BOTTOMBORDER || origin.x <= LEFTBORDER || origin.x >= RIGHTBORDER)
			draw = false;
	}
	Directions direction;

};