#pragma once
#ifdef UNITTEST_DZBRIDGE

#include <QObject>
#include "UnitTest.h"

class UnitTest_DzBridgeUnityAction : public UnitTest {
	Q_OBJECT
public:
	bool runUnitTests();
};

#endif