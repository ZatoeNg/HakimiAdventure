#pragma once
#include <functional>

// 通用定时器
class Timer
{
private:
	float _PassTime = 0;					// 已过时间
	float _WaitTime = 0;					// 等待时间
	bool _IsPause = false;					// 是否暂停
	bool _Shotted = false;					// 是否触发
	bool _OneShot = false;					// 单次触发
	std::function<void()> _OnTimeout;		// 触发回调

public:
	Timer() = default;
	~Timer() = default;

	void Restart()
	{
		_PassTime = 0;
		_Shotted = false;
	}

	void SetWaitTime(float fInterval)
	{
		_WaitTime = fInterval;
	}

	void SetOnTimeout(const std::function<void()>& funCallback)
	{
		this->_OnTimeout = funCallback;
	}

	void Pause()
	{
		_IsPause = true;
	}

	void Resume()
	{
		_IsPause = false;
	}

	void SetOneShot(bool bFlag)
	{
		_OneShot = bFlag;
	}

	void OnUpdate(float fDelta)
	{
		if (_IsPause)return;
			
		_PassTime += fDelta;
		if (_PassTime >= _WaitTime)
		{
			bool canShot =( !_OneShot || (_OneShot && !_Shotted));
			_Shotted = true;
			if (canShot && _OnTimeout)_OnTimeout();
			_PassTime -= _WaitTime;
		}
	}

};