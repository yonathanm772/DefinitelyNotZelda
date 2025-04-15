#pragma once

#include "Animation.h"
#include "Assets.h"


class Component
{
public:
		bool has = false;
};

class CDraggable : public Component
{
public:
	bool dragging = false;
	CDraggable() {}
};

// tells where an entity is and how fast its going, and at what angle
class CTransform : public Component
{
public:
	Vec2 pos				= { 0.0, 0.0 };
	Vec2 prevPos			= { 0.0, 0.0 };
	Vec2 scale				= { 1.0, 1.0 };
	Vec2 velocity			= { 0.0, 0.0 };
	Vec2 facing				= { 0.0, 1.0 }; //1 = facing right, -1 = facing left
	float angle				= 0; 
	
	

	CTransform() {}
	CTransform(const Vec2& p)
		:pos(p) {}
	CTransform(const Vec2& p, const Vec2& sp)
		:pos(p), velocity(sp) {}
	CTransform(const Vec2& p, const Vec2 &sp, const Vec2& sc,float a)
		:pos(p), prevPos(p),velocity(sp), scale(sc), angle(a){}
};

class CLifespan : public Component
{
public:
	int lifespan = 0;			// amount of lifespan remaining on the entity
	int frameCreated = 0;		// the total initial amount of lifespan
	CLifespan() {}
	CLifespan(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {}
};
class CInvincibility : public Component
{
public:
	int iframes = 0;
	CInvincibility() {}
	CInvincibility(int f)
		: iframes(f) {}
};

class CHealth : public Component
{
public:
	int max = 1;
	int current = 1;
	CHealth() {}
	CHealth(int m, int c)
		: max(m), current(c) {}
};

class CDamage : public Component
{
public:
	int damage = 1;
	CDamage() {}
	CDamage(int d)
		: damage(d) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool attack = false;
	bool canAttack = true;

	CInput() {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	bool blockMove = false;
	bool blockVision = false;
	CBoundingBox() {}
	CBoundingBox(const Vec2& s)
		:size(s), halfSize(s.x / 2, s.y / 2) {}
	CBoundingBox(const Vec2& s, bool m, bool v)
		:size(s), blockMove(m), blockVision(v), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& animation, bool r)
		:animation(animation), repeat(r) {}
};

class CState : public Component
{
public:
	std::string state = "stand";
	//bool isGrounded = false;
	//bool isRunning = false;
	CState() = default;
	CState(const std::string &s) : state(s) {}
};

class CFollowPlayer : public Component
{
public:
	Vec2 home = { 0, 0 };
	float speed = 0.0f;
	//bool isGrounded = false;
	//bool isRunning = false;
	CFollowPlayer() = default;
	CFollowPlayer(Vec2 p, float s)
		: home(p), speed(s) {}
};

class CPatrol : public Component
{
public:
	std::vector<Vec2> positions;
	// this is the index of the position vector
	// it needs to increment whenever the enemy is withing 5px of its destination
	// the index is going to be currentposition + 1 % positions.length();
	size_t currentPosition = 0;		
	float speed = 0;
	//bool isGrounded = false;
	//bool isRunning = false;
	CPatrol() = default;
	CPatrol(std::vector<Vec2>& pos, float s)
		: positions(pos), speed(s) {}
};


