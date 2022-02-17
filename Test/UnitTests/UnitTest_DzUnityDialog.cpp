#ifdef UNITTEST_DZBRIDGE

#include "UnitTest_DzUnityDialog.h"
#include "DzUnityDialog.h"


bool UnitTest_DzBridgeUnityDialog::runUnitTests()
{
	DzBridgeUnityDialog* testObject = new DzBridgeUnityDialog();

	if (!testObject)
	{
		return false;
	}

	return true;
}

#include "moc_UnitTest_DzUnityDialog.cpp"

#endif