#pragma once
#include "stdafx.h"
#include "DataItem.h"
namespace neuralNet {
	using std::vector;
	template<class T>
	class ILearningStrategy {
	public:
		virtual void train(T* network, vector<DataItem<float>>& data) = 0;
	};
}