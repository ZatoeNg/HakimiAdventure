#ifndef _ATLAS_H_
#define _ATLAS_H_

#include <vector>
#include <graphics.h>


class Atlas
{
private:
	std::vector<IMAGE> _ImageList;

public:
	Atlas() = default;
	~Atlas() = default;

	void Load(LPCTSTR  lpctstrPathImage, int iNum)
	{
		// º”‘ÿ∂Øª≠÷°Õº∆¨
		_ImageList.clear();
		_ImageList.resize(iNum);

		TCHAR filePath[512] = { 0 };
		for (int i = 0; i < iNum; i++) {
			_stprintf_s(filePath, lpctstrPathImage, i + 1);
			loadimage(&_ImageList[i], filePath);
		}
	}

	void Clear()
	{
		_ImageList.clear();
	}

	int GetSize() const
	{
		return (int)_ImageList.size();
	}

	IMAGE* GetImage(int iIdx)
	{
		if (iIdx < 0 || iIdx >= _ImageList.size())return nullptr;

		return &_ImageList[iIdx];
	}

	void AddImage(const IMAGE& img)
	{
		_ImageList.push_back(img);
	}

};

#endif