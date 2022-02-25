#ifdef UNITTEST_DZBRIDGE

#include "UnitTest_DzUnityDialog.h"
#include "DzUnityDialog.h"


UnitTest_DzUnityDialog::UnitTest_DzUnityDialog()
{
	m_testObject = (QObject*) new DzUnityDialog();
}

bool UnitTest_DzUnityDialog::runUnitTests()
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

bool UnitTest_DzUnityDialog::_DzBridgeUnityDialog(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(new DzUnityDialog());
	return bResult;
}

bool UnitTest_DzUnityDialog::addButton(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL_NULLPTR(qobject_cast<DzUnityDialog*>(m_testObject)->addButton(nullptr));
	return bResult;
}

bool UnitTest_DzUnityDialog::addLayout(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL_NULLPTR(qobject_cast<DzUnityDialog*>(m_testObject)->addLayout(nullptr));
	return bResult;
}

bool UnitTest_DzUnityDialog::addSpacing(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->addSpacing(0));
	return bResult;
}

bool UnitTest_DzUnityDialog::addStretch(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->addStretch(0));
	return bResult;
}

bool UnitTest_DzUnityDialog::addWidget(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL_NULLPTR(qobject_cast<DzUnityDialog*>(m_testObject)->addWidget(nullptr));
	return bResult;
}

bool UnitTest_DzUnityDialog::getAssetsFolderEdit(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->getAssetsFolderEdit());
	return bResult;
}

bool UnitTest_DzUnityDialog::resetToDefaults(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->resetToDefaults());
	return bResult;
}

bool UnitTest_DzUnityDialog::loadSavedSettings(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->loadSavedSettings());
	return bResult;
}

bool UnitTest_DzUnityDialog::HandleSelectAssetsFolderButton(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->HandleSelectAssetsFolderButton());
	return bResult;
}

bool UnitTest_DzUnityDialog::HandleInstallUnityFilesCheckBoxChange(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->HandleInstallUnityFilesCheckBoxChange(0));
	return bResult;
}

bool UnitTest_DzUnityDialog::HandleAssetTypeComboChange(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->HandleAssetTypeComboChange(0));
	return bResult;
}

bool UnitTest_DzUnityDialog::HandleAssetFolderChanged(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityDialog*>(m_testObject)->HandleAssetFolderChanged(""));
	return bResult;
}



#include "moc_UnitTest_DzUnityDialog.cpp"
#endif