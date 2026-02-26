#pragma once
#include <vector>
#include <functional>

#include "../Kits/Util.h"
#include "../Kits/Vector2.h"
#include "../Kits/Timer.h"
#include "Atlas.h"

class Animation
{
private:
	// 动画帧
	struct Frame
	{
		Rect rectSrc;
		IMAGE* image = nullptr;

		Frame() = default;
		Frame(IMAGE* img, Rect rect)
			:image(img), rectSrc(rect) {}
		~Frame() = default;
	};

private:
	Timer _Timer;										// 动画播放发计时器		
	Vector2 _Position;
	size_t _IdxFrame = 0;								// 帧索引,当前正在播放的动画帧
	bool _IsLoop = false;								// 是否循环播放					
	std::vector<Frame> _FrameList;						// 动画帧序列
	std::function<void()> _OnFinished;					// 动画结束处理	

public:
	Animation()
	{
		_Timer.SetOneShot(false);
		_Timer.SetOnTimeout(
			[&]()
			{
				this->_IdxFrame++;
				if (this->_IdxFrame >= this->_FrameList.size())
				{
					this->_IdxFrame = this->_IsLoop ? 0 : this->_FrameList.size() - 1;
					if (!this->_IsLoop && this->_OnFinished)this->_OnFinished();
				}
			}
		);
	}
	~Animation() = default;

	void Reset()
	{
		this->_Timer.Restart();
		this->_IdxFrame = 0;
	}

	void SetPosition(const Vector2& vecPosition)
	{
		this->_Position = vecPosition;
	}

	void SetLoop(bool bIsLoop)
	{
		this->_IsLoop = bIsLoop;
	}

	void SetInterval(float fVal)
	{
		this->_Timer.SetWaitTime(fVal);
	}

	void SetOnFinished(std::function<void()>& funCallback)
	{
		this->_OnFinished = funCallback;
	}

	void AddFrame(IMAGE* image, int iNumH)
	{
		int height = image->getheight();
		int width = image->getwidth();
		int widthFrame = width / iNumH; //每帧图片宽度

		for (int i = 0; i < iNumH; i++)
		{
			Rect rectSrc;
			/*
			* X为每帧图片的像素位置，假设widthFrame为200，则第一帧图片的位置是0，第二帧则是200，i则是确定裁切的哪个帧图片
			* Y为0，是因为每一帧的图片都处于同一水平上
			*/
			rectSrc.X = widthFrame * i, rectSrc.Y = 0;
			rectSrc.W = widthFrame, rectSrc.H = height;

			_FrameList.emplace_back(image, rectSrc);
		}
	}

	void AddFrame(Atlas* atlas)
	{
		for (int i = 0; i < atlas->GetSize(); i++)
		{
			IMAGE* image = atlas->GetImage(i);

			Rect rectSrc;
			rectSrc.X = 0, rectSrc.Y = 0;
			rectSrc.W = image->getwidth(), rectSrc.H = image->getheight();

			_FrameList.emplace_back(image, rectSrc);
		}
	}

	void OnUpdate(float fDelta)
	{
		this->_Timer.OnUpdate(fDelta);
	}

	void OnRender(const Camera& camera) const
	{
		if(_IdxFrame >= _FrameList.size() || nullptr == _FrameList[_IdxFrame].image)
		{
			return;
			// throw std::invalid_argument("render frame idx out of range, or is nullptr");
		}

		const Frame& frame = this->_FrameList[_IdxFrame];

		Rect rectDst;
		rectDst.W = frame.rectSrc.W, rectDst.H = frame.rectSrc.H;
		//将图片中心点与设置中心点重合
		rectDst.X = (int)this->_Position.X - frame.rectSrc.W/ 2; //设置中心点 - 图片宽 / 2 = 图片左上角X的坐标
		rectDst.Y = (int)this->_Position.Y - frame.rectSrc.H / 2;//设置中心点 - 图片高 / 2 = 图片左上角Y的坐标

		/*
		* Dst目标图片，作用设置图片位置与缩放
		* Src源图片，主要用于雪碧图裁切图片成帧，不需要裁切则不需要填写
		*/
		PutimageAlphaEx(camera,frame.image, &rectDst, &frame.rectSrc);
	}

};