#pragma once
#include "../Kits/Vector2.h"
#include "../Kits/Timer.h"

class Camera
{
private:
	Vector2 _Size;
	Vector2 _Position;

public:
	Camera() = default;
	~Camera() = default;

	void SetSize(const Vector2 vecSize)
	{
		this->_Size = vecSize;
	}


	const Vector2& GetSize()const
	{
		return this->_Size;
	}

	void SetPosition(const Vector2& vecPosition)
	{
		this->_Position = vecPosition;
	}

	// 窗口坐标 = 世界坐标 - 摄像机坐标
	const Vector2& GetPosition() const
	{
		return this->_Position;
	}

	void LookAt(const Vector2& vecTarget)
	{
		_Position = vecTarget - this->_Size / 2.0f;
	}
	

};