#pragma once
#include "FGDetector.h"
class FGDetectorFactory
{
public:
	FGDetectorFactory();
	~FGDetectorFactory();

	static FGDetector* Make(int breadType);
};

