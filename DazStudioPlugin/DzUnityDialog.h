#pragma once
#include "dzbasicdialog.h"
#include <QtGui/qcombobox.h>
#include <QtCore/qsettings.h>
#include <DzBridgeDialog.h>

class QPushButton;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QWidget;
class DzBridgeUnityAction;

class DzBridgeUnityDialog : public DzBridgeDialog {
	friend DzBridgeUnityAction;
	Q_OBJECT
	Q_PROPERTY(QWidget* assetsFolderEdit READ getAssetsFolderEdit)
public:
	Q_INVOKABLE void addButton(QAbstractButton* btn, int index = 1) { DzBasicDialog::addButton(btn, index); }
	Q_INVOKABLE void addLayout(QLayout* layout, int stretch = 0) { DzBasicDialog::addLayout(layout, stretch); }
	Q_INVOKABLE void addSpacing(int spacing) { DzBasicDialog::addSpacing(spacing); }
	Q_INVOKABLE void addStretch(int stretch=0) { DzBasicDialog::addStretch(stretch); }
	Q_INVOKABLE void addWidget(QWidget* child, int stretch=0, Qt::Alignment alignment=0) { DzBasicDialog::addWidget(child, stretch, alignment); }

	QLineEdit* getAssetsFolderEdit() { return assetsFolderEdit; }

	/** Constructor **/
	 DzBridgeUnityDialog(QWidget *parent=nullptr);

	/** Destructor **/
	virtual ~DzBridgeUnityDialog() {}

	Q_INVOKABLE void resetToDefaults();

	QString getRootFolder();
protected slots:
	void HandleSelectAssetsFolderButton();
	void HandleInstallUnityFilesCheckBoxChange(int state);
	void HandleAssetTypeComboChange(int state);
	void HandleAssetFolderChanged(const QString& directoryName);

protected:
	QLineEdit* projectEdit;
	QPushButton* projectButton;
	QLineEdit* assetsFolderEdit;
	QPushButton* assetsFolderButton;

	QLabel* installOrOverwriteUnityFilesLabel;
	QCheckBox* installUnityFilesCheckBox;

	Q_INVOKABLE bool loadSavedSettings();

};
