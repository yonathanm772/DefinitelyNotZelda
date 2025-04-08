#include "Physics.h"
#include "Components.h"
#include <iostream>


Vec2 Physics::getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return the overlap rectangle size of the bounding boxes of entity a and b
	//		first: check to see if a and b actually have bounding boxes &&
	//			   check to see if a and b have Transform components
	//		second: if the above statement returns true, then compute and return Vec2
	// 
	//
	auto& transformA = a->getComponent<CTransform>();
	auto& transformB = b->getComponent<CTransform>();
	auto& boundingA = a->getComponent<CBoundingBox>();
	auto& boundingB = b->getComponent<CBoundingBox>();

	
	if (boundingA.has && boundingB.has)
	{
		if (transformA.has && transformB.has)
		{
			float overlapXa = boundingA.halfSize.x;
			float overlapXb = boundingB.halfSize.x;
			float overlapYa = boundingA.halfSize.y;
			float overlapYb = boundingB.halfSize.y;
			float dx = abs(transformA.pos.x - transformB.pos.x);
			float dy = abs(transformA.pos.y - transformB.pos.y);
			float overlapInX = (overlapXa + overlapXb)-dx;
			float overlapInY = (overlapYa + overlapYb)-dy;


			if (overlapInX > 0 && overlapInY > 0)
			{
				return Vec2(overlapInX, overlapInY);
			}

		}
	}
	
	return Vec2(0, 0);
	
	
}

Vec2 Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
	//		previous overlap uses the entity's previous position
	//		EXACT same function GetOverlap(), but using the previous positions instead of the current
	//		ones
	//
	auto& transformA = a->getComponent<CTransform>();
	auto& transformB = b->getComponent<CTransform>();
	auto& boundingA = a->getComponent<CBoundingBox>();
	auto& boundingB = b->getComponent<CBoundingBox>();


	if (boundingA.has && boundingB.has)
	{
		if (transformA.has && transformB.has)
		{
			float overlapXa = boundingA.halfSize.x;
			float overlapXb = boundingB.halfSize.x;
			float overlapYa = boundingA.halfSize.y;
			float overlapYb = boundingB.halfSize.y;
			float dx = abs(transformA.prevPos.x - transformB.prevPos.x);
			float dy = abs(transformA.prevPos.y - transformB.prevPos.y);
			float overlapInX = (overlapXa + overlapXb) - dx;
			float overlapInY = (overlapYa + overlapYb) - dy;


			if (overlapInX > 0)
			{
				return Vec2(overlapInX, overlapInY);
			}
			if (overlapInY > 0)
			{
				return Vec2(overlapInX, overlapInY);
			}

		}
	}

	return Vec2(0, 0);
}

Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
	return { false, Vec2(0,0) };
}
 
bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
	auto ePos = e->getComponent<CTransform>().pos;
	auto size = e->getComponent<CAnimation>().animation.getSize();

	float dx = fabs(pos.x - ePos.x);
	float dy = fabs(pos.y - ePos.y);

	//return (dx <= size.x / 2) && (dy <= size.y / 2);

	bool inside = (dx <= size.x / 2) && (dy <= size.y / 2);
	std::cout << "Mouse Click at (" << pos.x << ", " << pos.y << "), "
		<< "Entity Position: (" << ePos.x << ", " << ePos.y << "), "
		<< "Size: (" << size.x << ", " << size.y << "), "
		<< "Inside?: " << inside << "\n";

	return inside;
	
}

// does the line segment ab, intersect with the bounding box of the entity(e)
bool Physics::entityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
	// An entity block vision if the line b/n the player and the entity
	// is intesecting with any of the lines of the bounding box of the blocking entity
	// each entity has a center position and a bounding box, along with their half size
	// figure out the (x,y) of every point of the blocking entity
	// then do four intersection points
	//		1. does the line segment player/endity(ab) instersect with (e1,e2) top of the blocking entity
	//		2. does the line segment player/endity(ab) instersect with (e2,e3) right of the blocking entity
	//		3. does the line segment player/endity(ab) instersect with (e3,e4) bottom of the blocking entity
	//		4. does the line segment player/endity(ab) instersect with (e4,e1) left of the blocking entity

	Vec2 e1;
	Vec2 e2;
	Vec2 e3;
	Vec2 e4;

	/*if (LineIntersect(a, b, e1, e2)) { return true };
	if (LineIntersect(a, b, e2, e3)) { return true };
	if (LineIntersect(a, b, e3, e4)) { return true };
	if (LineIntersect(a, b, e4, e1)) { return true };
	*/
	//if any of the above are true, then return true
	return false;
}

