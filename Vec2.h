#pragma once
class Vec2
{
public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float xin, float yin);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator * (const float val) const;
	Vec2 operator / (const float val) const;

	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const Vec2& rhs);
	void operator/= (const Vec2& rhs);

	Vec2 differenceVec(const Vec2& rhs) const;
	float lengthDist() const;
	float distSqrt() const;
	void vecNorm(const float val);  
	float angle(const Vec2& differenceV);


	Vec2 velocity(const int speed, const float angle);
};

