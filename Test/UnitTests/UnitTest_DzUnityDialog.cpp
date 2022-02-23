#ifdef UNITTEST_DZBRIDGE

#include "UnitTest_DzUnityDialog.h"
#include "DzUnityDialog.h"


UnitTest_DzBridgeUnityDialog::UnitTest_DzBridgeUnityDialog()
{
	m_testObject = (QObject*) new DzBridgeUnityDialog();
}

bool UnitTest_DzBridgeUnityDialog::runUnitTests()
{
	RUNTEST(_DzBridgeUnityDialog);
	RUNTEST(addButton);
	RUNTEST(addLayout);
	RUNTEST(addSpacing);
	RUNTEST(addStretch);
	RUNTEST(addWidget);
	RUNTEST(getAssetsFolderEdit);
	RUNTEST(resetToDefaults);
	RUNTEST(loadSavedSettings);
	RUNTEST(HandleSelectAssetsFolderButton);
	RUNTEST(HandleInstallUnityFilesCheckBoxChange);
	RUNTEST(HandleAssetTypeComboChange);
	RUNTEST(HandleAssetFolderChanged);

	return true;
}

bool UnitTest_DzBridgeUnityDialog::_DzBridgeUnityDialog(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(new DzBridgeUnityDialog());
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::addButton(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL_NULLPTR(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->addButton(nullptr));
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::addLayout(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL_NULLPTR(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->addLayout(nullptr));
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::addSpacing(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->addSpacing(0));
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::addStretch(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->addStretch(0));
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::addWidget(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL_NULLPTR(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->addWidget(nullptr));
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::getAssetsFolderEdit(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->getAssetsFolderEdit());
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::resetToDefaults(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->resetToDefaults());
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::loadSavedSettings(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->loadSavedSettings());
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::HandleSelectAssetsFolderButton(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->HandleSelectAssetsFolderButton());
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::HandleInstallUnityFilesCheckBoxChange(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->HandleInstallUnityFilesCheckBoxChange(0));
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::HandleAssetTypeComboChange(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->HandleAssetTypeComboChange(0));
	return bResult;
}

bool UnitTest_DzBridgeUnityDialog::HandleAssetFolderChanged(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzBridgeUnityDialog*>(m_testObject)->HandleAssetFolderChanged(""));
	return bResult;
}



#include "moc_UnitTest_DzUnityDialog.cpp"
#endif