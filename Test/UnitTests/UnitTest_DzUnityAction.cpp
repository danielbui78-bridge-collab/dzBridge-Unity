#ifdef UNITTEST_DZBRIDGE

#include "UnitTest_DzUnityAction.h"
#include "DzUnityAction.h"


bool UnitTest_DzBridgeUnityAction::runUnitTests()
{
	DzBridgeUnityAction* testObject = new DzBridgeUnityAction();

	if (!testObject)
	{
		return false;
	}

	return true;
}


#include "moc_UnitTest_DzUnityAction.cpp"

#endif