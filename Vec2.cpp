#include "Vec2.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Vec2::Vec2()
{

}

Vec2::Vec2(float xin, float yin)
	:x(xin), y(yin)
{

}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}
bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x && y != rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}
Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}
Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}
Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x / val, y / val);

}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;

}
void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}
void Vec2::operator *= (const Vec2& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
}
void Vec2::operator/= (const Vec2& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
}

Vec2 Vec2::differenceVec(const Vec2& rhs) const
{

	return Vec2(rhs.x - x, rhs.y - y);
}

float Vec2::lengthDist() const
{
	return sqrtf((x * x) + (y * y));
	
	
}

float Vec2::distSqrt() const
{
	return (x * x) + (y * y);
}

void Vec2::vecNorm(const float val)
{
	if (val != 0)  // Prevent division by zero
	{
		x /= val;
		y /= val;
	}
}

float Vec2::angle(const Vec2& differenceV)
{
	return atan2f(differenceV.y, differenceV.x);
}

Vec2 Vec2::velocity(const int speed, const float angleInDegrees)
{
	// Convert angle from degrees to radians
	float angleInRadians = angleInDegrees * (M_PI / 180.0f);

	return Vec2(speed * cos(angleInRadians), speed * sin(angleInRadians));
}




