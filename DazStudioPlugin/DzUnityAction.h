#pragma once
#include <dzaction.h>
#include <dznode.h>
#include <dzjsonwriter.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <DzBridgeAction.h>
#include "DzUnityDialog.h"


class DzBridgeUnityAction : public DzBridgeAction {
	 Q_OBJECT
	 Q_PROPERTY(bool InstallUnityFiles READ getInstallUnityFiles WRITE setInstallUnityFiles)
//	 Q_PROPERTY(DzBridgeUnityDialog* BridgeDialog READ getBridgeDialog)

public:
	DzBridgeUnityAction();

	void setInstallUnityFiles(bool arg) { InstallUnityFiles = arg; }
	bool getInstallUnityFiles() { return InstallUnityFiles; }
//	DzBridgeUnityDialog* getBridgeDialog() { return BridgeDialog; }

	QString ProjectFolder;

protected:
//	 DzBridgeUnityDialog* BridgeDialog;
	 bool InstallUnityFiles;

	 void executeAction();
	 Q_INVOKABLE bool createUI();
	 Q_INVOKABLE void writeConfiguration();
	 Q_INVOKABLE void setExportOptions(DzFileIOSettings& ExportOptions);
	 Q_INVOKABLE QString createUnityFiles(bool replace = true);
	 QString readGUIRootFolder();

};
