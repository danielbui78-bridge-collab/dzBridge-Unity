#pragma once
#ifdef UNITTEST_DZBRIDGE

#include <QObject>
#include "UnitTest.h"

class UnitTest_DzBridgeUnityDialog : public UnitTest {
	Q_OBJECT
public:
	UnitTest_DzBridgeUnityDialog();
	bool runUnitTests();

private:
	bool _DzBridgeUnityDialog(UnitTest::TestResult* testResult);
	bool addButton(UnitTest::TestResult* testResult);
	bool addLayout(UnitTest::TestResult* testResult);
	bool addSpacing(UnitTest::TestResult* testResult);
	bool addStretch(UnitTest::TestResult* testResult);
	bool addWidget(UnitTest::TestResult* testResult);
	bool getAssetsFolderEdit(UnitTest::TestResult* testResult);
	bool resetToDefaults(UnitTest::TestResult* testResult);
	bool loadSavedSettings(UnitTest::TestResult* testResult);
	bool HandleSelectAssetsFolderButton(UnitTest::TestResult* testResult);
	bool HandleInstallUnityFilesCheckBoxChange(UnitTest::TestResult* testResult);
	bool HandleAssetTypeComboChange(UnitTest::TestResult* testResult);
	bool HandleAssetFolderChanged(UnitTest::TestResult* testResult);

};


#endif