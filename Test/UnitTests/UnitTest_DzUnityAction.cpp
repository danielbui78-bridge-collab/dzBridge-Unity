#ifdef UNITTEST_DZBRIDGE

#include "UnitTest_DzUnityAction.h"
#include "DzUnityAction.h"


UnitTest_DzBridgeUnityAction::UnitTest_DzBridgeUnityAction()
{
	m_testObject = (QObject*) new DzBridgeUnityAction();
}

bool UnitTest_DzBridgeUnityAction::runUnitTests()
{
	RUNTEST(_DzBridgeUnityAction);
	RUNTEST(setInstallUnityFiles);
	RUNTEST(getInstallUnityFiles);
	RUNTEST(executeAction);
	RUNTEST(createUI);
	RUNTEST(writeConfiguration);
	RUNTEST(setExportOptions);
	RUNTEST(createUnityFiles);
	RUNTEST(readGuiRootFolder);

	return true;
}

bool UnitTest_DzBridgeUnityAction::_DzBridgeUnityAction(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(new DzBridgeUnityAction());
	return bResult;
}

bool UnitTest_DzBridgeUnityAction::setInstallUnityFiles(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityAction*>(m_testObject)->setInstallUnityFiles(false));
	return bResult;
}

bool UnitTest_DzBridgeUnityAction::getInstallUnityFiles(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityAction*>(m_testObject)->getInstallUnityFiles());
	return bResult;
}

bool UnitTest_DzBridgeUnityAction::executeAction(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityAction*>(m_testObject)->executeAction());
	return bResult;
}

bool UnitTest_DzBridgeUnityAction::createUI(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityAction*>(m_testObject)->createUI());
	return bResult;
}

bool UnitTest_DzBridgeUnityAction::writeConfiguration(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityAction*>(m_testObject)->writeConfiguration());
	return bResult;
}

bool UnitTest_DzBridgeUnityAction::setExportOptions(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	DzFileIOSettings arg;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityAction*>(m_testObject)->setExportOptions(arg));
	return bResult;
}

bool UnitTest_DzBridgeUnityAction::createUnityFiles(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityAction*>(m_testObject)->createUnityFiles());
	return bResult;
}

bool UnitTest_DzBridgeUnityAction::readGuiRootFolder(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityAction*>(m_testObject)->readGuiRootFolder());
	return bResult;
}


#include "moc_UnitTest_DzUnityAction.cpp"

#endif