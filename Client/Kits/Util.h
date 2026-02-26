#pragma once
#pragma comment(lib, "WINmm.lib")		// mciSendString() 
#pragma comment(lib, "MSIMG32.LIB")		// AlphaBlend()

#include <vector>
#include "../Camera/Camera.h"
#include <graphics.h>

// 矩形区域
struct Rect
{
	int X, Y;
	int W, H;
};


inline void PutimageAlphaEx(const Camera& camera,IMAGE* image,const Rect* rectDst, const Rect* rectSrc = nullptr)
{
	// 渲染逻辑
	static BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	const Vector2& posCamera = camera.GetPosition();

	AlphaBlend(GetImageHDC(GetWorkingImage()), 
		(int)(rectDst->X- posCamera.X), 
		(int)(rectDst->Y - posCamera.Y),
		rectDst->W,
		rectDst->H,
		GetImageHDC(image),
		rectSrc ? rectSrc->X : 0, rectSrc ? rectSrc->Y : 0,
		rectSrc ? rectSrc->W : image->getwidth(),
		rectSrc ? rectSrc->H : image->getheight(), blendFunc);
}


inline void LoadAudio(LPCTSTR lpctstrPath, LPCTSTR lpctstrId)
{
	static TCHAR buff[512] = { 0 };
	_stprintf_s(buff, _T("open %s alias %s"), lpctstrPath, lpctstrId);
	mciSendString(buff, nullptr, 0, nullptr);
}

inline void PlayAudio(LPCTSTR lpctstrId, bool bIsLoop = false)
{
	// 为了避免延时,使用异步模式async
	static TCHAR buff[512] = { 0 };
	_stprintf_s(buff, _T("play %s %s from 0"), lpctstrId, bIsLoop ? _T("repeat") : _T(""));
	mciSendString(buff, nullptr, 0, nullptr);
}

inline void StopAudio(LPCTSTR lpctstrId)
{
	static TCHAR buff[512] = { 0 };
	_stprintf_s(buff, _T("stop %s"), lpctstrId);
	mciSendString(buff, nullptr, 0, nullptr);
}

