#ifndef _Player_H_
#define _Player_H_

#include "../Animation/Animation.h"

class Player
{
public:
	enum class Facing
	{
		Up, Down, Left, Right
	};

private:
	const float _SpeedRun = 100.0f; //奔跑速度常量：100 px/s

private:
	Vector2 _Position; //当前位置
	Vector2 _Velocity; //移动速度
	Vector2 _PosTarget; //目标位置

	//动画
	Animation _AnimIdleUp;
	Animation _AnimIdleDown;
	Animation _AnimIdleLeft;
	Animation _AnimIdleRight;
	Animation _AnimRunUp;
	Animation _AnimRunDown;
	Animation _AnimRunLeft;
	Animation _AnimRunRight;
	Animation* _CurrentAnim = nullptr; // 当前动画指针
	Facing _facing = Facing::Down; //当前朝向枚举变量


public:
	Player(Atlas* atlasIdleUp, Atlas* atlasIdleDown, Atlas* atlasIdleLeft, Atlas* atlasIdleRight
		, Atlas* atlasRunUp, Atlas* atlasRunDown, Atlas* atlasRunLeft, Atlas* atlasRunRight) {

		_AnimIdleUp.SetLoop(true);
		_AnimIdleUp.SetInterval(0.1f);
		_AnimIdleUp.AddFrame(atlasIdleUp);

		_AnimIdleDown.SetLoop(true);
		_AnimIdleDown.SetInterval(0.1f);
		_AnimIdleDown.AddFrame(atlasIdleDown);

		_AnimIdleLeft.SetLoop(true);
		_AnimIdleLeft.SetInterval(0.1f);
		_AnimIdleLeft.AddFrame(atlasIdleLeft);

		_AnimIdleRight.SetLoop(true);
		_AnimIdleRight.SetInterval(0.1f);
		_AnimIdleRight.AddFrame(atlasIdleRight);

		_AnimRunUp.SetLoop(true);
		_AnimRunUp.SetInterval(0.1f);
		_AnimRunUp.AddFrame(atlasRunUp);

		_AnimRunDown.SetLoop(true);
		_AnimRunDown.SetInterval(0.1f);
		_AnimRunDown.AddFrame(atlasRunDown);

		_AnimRunLeft.SetLoop(true);
		_AnimRunLeft.SetInterval(0.1f);
		_AnimRunLeft.AddFrame(atlasRunLeft);

		_AnimRunRight.SetLoop(true);
		_AnimRunRight.SetInterval(0.1f);
		_AnimRunRight.AddFrame(atlasRunRight);

	}
	~Player() = default;

public:
	void OnUpdate(float fDelta)
	{
		if (!this->_Position.Approx(this->_PosTarget))
			this->_Velocity = (this->_PosTarget - this->_Position).Normalize() * this->_SpeedRun;
		else
			this->_Velocity = Vector2(0, 0);

		if ((this->_PosTarget - this->_Position).GetLength() <= (this->_Velocity * fDelta).GetLength())
			this->_Position = this->_PosTarget;
		else
			this->_Position += this->_Velocity * fDelta;

		if (this->_Velocity.Approx(Vector2(0, 0)))
		{
			switch (this->_facing)
			{
			case Player::Facing::Up:
				this->_CurrentAnim= &_AnimIdleUp;
				break;
			case Player::Facing::Down:
				this->_CurrentAnim= &_AnimIdleDown;
				break;
			case Player::Facing::Left:
				this->_CurrentAnim= &_AnimIdleLeft;
				break;
			case Player::Facing::Right:
				this->_CurrentAnim= &_AnimIdleRight;
				break;
			}
		}
		else
		{
			if (abs(this->_Velocity.Y) >= 0.0001f)
				this->_facing = (this->_Velocity.Y > 0) ? Player::Facing::Down : Player::Facing::Up;
			if (abs(this->_Velocity.X) >= 0.0001f)
				this->_facing = (this->_Velocity.X > 0) ? Player::Facing::Right : Player::Facing::Left;

			switch (this->_facing)
			{
			case Player::Facing::Up:
				this->_CurrentAnim= &_AnimRunUp;
				break;
			case Player::Facing::Down:
				this->_CurrentAnim= &_AnimRunDown;
				break;
			case Player::Facing::Left:
				this->_CurrentAnim= &_AnimRunLeft;
				break;
			case Player::Facing::Right:
				this->_CurrentAnim= &_AnimRunRight;
				break;
			}
		}

		if (!this->_CurrentAnim)return;
		this->_CurrentAnim->SetPosition(this->_Position);
		this->_CurrentAnim->OnUpdate(fDelta);
	}

	void OnRender(const Camera& camera)
	{
		if (!this->_CurrentAnim)return;
		this->_CurrentAnim->OnRender(camera);
	}

	void SetPosition(const Vector2& vecPosition)
	{
		this->_Position = vecPosition;
	}

	const Vector2 GetPosition() const
	{
		return this->_Position;
	}

	void SetTarget(const Vector2& vecPosTarget)
	{
		this->_PosTarget = vecPosTarget;
	}
};

#endif
