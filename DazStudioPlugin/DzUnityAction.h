#pragma once
#include <dzaction.h>
#include <dznode.h>
#include <dzjsonwriter.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <DzRuntimePluginAction.h>
#include "DzUnityDialog.h"


class DzBridgeUnityAction : public DzRuntimePluginAction {
	 Q_OBJECT
	 Q_PROPERTY(bool InstallUnityFiles READ getInstallUnityFiles WRITE setInstallUnityFiles)
	 Q_PROPERTY(DzBridgeUnityDialog* BridgeDialog READ getBridgeDialog)

public:
	DzBridgeUnityAction();

	void setInstallUnityFiles(bool arg) { InstallUnityFiles = arg; }
	bool getInstallUnityFiles() { return InstallUnityFiles; }
	DzBridgeUnityDialog* getBridgeDialog() { return BridgeDialog; }

	QString ProjectFolder;

protected:
	 DzBridgeUnityDialog* BridgeDialog;
	 bool InstallUnityFiles;

	 void executeAction();
	 Q_INVOKABLE bool CreateUI();
	 Q_INVOKABLE void WriteConfiguration();
	 Q_INVOKABLE void SetExportOptions(DzFileIOSettings& ExportOptions);
	 Q_INVOKABLE QString CreateUnityFiles(bool replace = true);
	 QString readGUIRootFolder();

};
