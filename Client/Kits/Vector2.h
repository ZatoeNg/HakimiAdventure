#pragma once
#include <cmath>

// 二维向量类
class Vector2
{
public:
	float X = 0.0f;
	float Y = 0.0f;
public:
	Vector2() = default;
	~Vector2() = default;

	Vector2(float fX, float fY)
		: X(fX), Y(fY){}


	Vector2 operator+(const Vector2& vec) const
	{
		return Vector2(X + vec.X, Y + vec.Y);
	}

	// 二维向量运算 + - * / 长度len 标准化normalize
	void operator+=(const Vector2& vec)
	{
		X += vec.X;
		Y += vec.Y;
	}

	Vector2 operator-(const Vector2& vec) const
	{
		return Vector2(X - vec.X, Y - vec.Y);
	}

	void operator-=(const Vector2& vec)
	{
		X -= vec.X;
		Y -= vec.Y;
	}

	Vector2 operator*(float fValue) const
	{
		return Vector2(X * fValue, Y * fValue);
	}

	float operator*(const Vector2& vec) const
	{
		return X * vec.X + Y * vec.Y;
	}

	void operator*=(float fValue)
	{
		X *= fValue;
		Y *= fValue;
	}

	void operator/=(float fValue)
	{
		X /= fValue;
		Y /= fValue;
	}

	Vector2 operator/(float fValue) const
	{
		return Vector2(X / fValue, Y / fValue);
	}

	float GetLength() const
	{
		return sqrt(X * X + Y * Y);
	}

	// 标准化分量
	Vector2 Normalize() const 
	{
		float len = GetLength();
		if (len == 0) return Vector2(0, 0);

		return Vector2(X / len, Y / len);
	}

	bool Approx(const Vector2& vecTarget)
	{
		return (*this - vecTarget).GetLength() <= 0.0001f;
	}

};