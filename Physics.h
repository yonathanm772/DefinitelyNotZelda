#pragma once
#include "Physics.h"
#include <memory>
#include "Entity.h"
#include "Vec2.h"

struct Intersect
{
	bool resut = false;
	Vec2 pos;
};
class Physics
{

public:
	static Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	static Vec2 getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	static Intersect LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
	static bool IsInside(const Vec2& pos, std::shared_ptr<Entity> e);
	static bool entityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e);

};

