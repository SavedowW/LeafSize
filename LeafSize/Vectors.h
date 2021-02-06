#pragma once
#include <cmath>
#include <iostream>

const float PI = 3.1415;

struct Vector2
{
	float x, y;
	Vector2(float nx = 0, float ny = 0);
	Vector2 operator+(const Vector2& rhs) const;
	Vector2 operator-(const Vector2& rhs) const;
	Vector2 operator*(const float& num) const;
	void operator+=(const Vector2& rhs);
	void operator-=(const Vector2& rhs);
	void operator*=(const float& num);
	void operator/=(const float& num);
	float getLen() const;
	void normalise();
};

std::ostream& operator<< (std::ostream& out, const Vector2& vec);