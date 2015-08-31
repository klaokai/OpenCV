#include "FGDetectorFactory.h"
#include "SimpleFGDetector.h"
#include "VibeFGDetector.h"
#include "OCVFGDetector.h"

FGDetectorFactory::FGDetectorFactory()
{
}


FGDetectorFactory::~FGDetectorFactory()
{
}


FGDetector* FGDetectorFactory::Make(int breadType)
{
	FGDetector* tempFGDetector;
	switch ( breadType )
	{
		case 1:
			tempFGDetector =new SimpleFGDetector();
			break;
		case 2:
			tempFGDetector = new VibeFGDetector();
			break;
		case	3:
			tempFGDetector = new OCVFGDetector();
			break;
		default:
			tempFGDetector = new FGDetector();
			break;
	}
	return tempFGDetector;
}