#ifndef _PATH_H_
#define _PATH_H

#include "../Kits/Vector2.h"

#include <vector>

class Path
{
private:
	float _TotalLength = 0; //路径总长度
	std::vector<Vector2> _PointList; // 所有顶点的位置坐标
	std::vector<float> _SegmentLenList; //每两个顶点之间路径片段长度列表

public:
	Path(const std::vector<Vector2>& vecPointList)
	{
		this->_PointList = vecPointList;
		for (size_t i = 1; i < this->_PointList.size(); ++i)
		{
			float segmentLen = (this->_PointList[i] - this->_PointList[i - 1]).GetLength();
			this->_SegmentLenList.push_back(segmentLen);
			this->_TotalLength += segmentLen;
		}
	};

	~Path() = default;

public:
	Vector2 GetPositionAtProgress(float fProgress)const
	{
		//边界设计
		if (fProgress <= 0)return _PointList.front();
		if (fProgress >= 1)return _PointList.back();

		//计算已完成的长度
		float targetDistance = this->_TotalLength * fProgress;

		//
		float accumulatedLen = 0.0f;

		//遍历顶点列表
		for (size_t i = 1; i < this->_PointList.size(); ++i)
		{
			//累加每一段路径片段的长度
			accumulatedLen += this->_SegmentLenList[i - 1];

			//找到目标距离所在的线段
			if (accumulatedLen >= targetDistance)
			{
				// 计算目标距离在当前线段内的相对进度（0~1）
				float segmentProgress = (targetDistance - (accumulatedLen - this->_SegmentLenList[i - 1])) / this->_SegmentLenList[i - 1];

				// 线性插值：从线段起点向终点移动segmentProgress比例的距离
				return this->_PointList[i - 1] + (this->_PointList[i] - this->_PointList[i - 1]) * segmentProgress;
			}
		}

		return this->_PointList.back();
	}

};

#endif // !_PATH_H_
