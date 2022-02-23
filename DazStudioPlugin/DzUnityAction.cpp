#include <QtGui/qcheckbox.h>
#include <QtGui/QMessageBox>
#include <QtNetwork/qudpsocket.h>
#include <QtNetwork/qabstractsocket.h>
#include <QCryptographicHash>
#include <QtCore/qdir.h>

#include <dzapp.h>
#include <dzscene.h>
#include <dzmainwindow.h>
#include <dzshape.h>
#include <dzproperty.h>
#include <dzobject.h>
#include <dzpresentation.h>
#include <dznumericproperty.h>
#include <dzimageproperty.h>
#include <dzcolorproperty.h>
#include <dpcimages.h>

#include "QtCore/qmetaobject.h"
#include "dzmodifier.h"
#include "dzgeometry.h"
#include "dzweightmap.h"
#include "dzfacetshape.h"
#include "dzfacetmesh.h"
#include "dzfacegroup.h"
#include "dzprogress.h"

#include "DzUnityAction.h"
#include "DzUnityDialog.h"
#include "DzBridgeMorphSelectionDialog.h"
#include "DzBridgeSubdivisionDialog.h"

#ifdef WIN32
	#include <shellapi.h>
#endif


DzBridgeUnityAction::DzBridgeUnityAction() :
	 DzBridgeAction(tr("Daz To &Unity"), tr("Send the selected node to Unity."))
{
	 m_bridgeDialog = nullptr;
	 NonInteractiveMode = 0;
	 AssetType = QString("SkeletalMesh");
	 //Setup Icon
	 QString iconName = "icon";
	 QPixmap basePixmap = QPixmap::fromImage(getEmbeddedImage(iconName.toLatin1()));
	 QIcon icon;
	 icon.addPixmap(basePixmap, QIcon::Normal, QIcon::Off);
	 QAction::setIcon(icon);

}

bool DzBridgeUnityAction::createUI()
{
	// Check if the main window has been created yet.
	// If it hasn't, alert the user and exit early.
	DzMainWindow* mw = dzApp->getInterface();
	if (!mw)
	{
		if (NonInteractiveMode == 0) QMessageBox::warning(0, tr("Error"), 
			tr("The main window has not been created yet."), QMessageBox::Ok);

		return false;
	}

	// m_subdivisionDialog creation REQUIRES valid Character or Prop selected
	if (dzScene->getNumSelectedNodes() != 1)
	{
		if (NonInteractiveMode == 0) QMessageBox::warning(0, tr("Error"), 
			tr("Please select one Character or Prop to send."), QMessageBox::Ok);

		return false;
	}

	 // Create the dialog
	if (!m_bridgeDialog)
	{
		m_bridgeDialog = new DzBridgeUnityDialog(mw);
	}
	else
	{
		DzBridgeUnityDialog* unityDialog = qobject_cast<DzBridgeUnityDialog*>(m_bridgeDialog);
		if (unityDialog)
		{
			unityDialog->resetToDefaults();
			unityDialog->loadSavedSettings();
		}
	}

	if (!m_subdivisionDialog) m_subdivisionDialog = DzBridgeSubdivisionDialog::Get(m_bridgeDialog);
	if (!m_morphSelectionDialog) m_morphSelectionDialog = DzBridgeMorphSelectionDialog::Get(m_bridgeDialog);

	return true;
}

void DzBridgeUnityAction::executeAction()
{

	 // Create and show the dialog. If the user cancels, exit early,
	 // otherwise continue on and do the thing that required modal
	 // input from the user.
	 if (createUI() == false)
		 return;

	 // If the Accept button was pressed, start the export
	 int dlgResult = 0;
	 if (NonInteractiveMode == 0)
	 {
		 dlgResult = m_bridgeDialog->exec();
	 }
	 if (NonInteractiveMode == 1 || dlgResult == QDialog::Accepted)
	 {
		  // DB 2021-10-11: Progress Bar
		  DzProgress* exportProgress = new DzProgress( "Sending to Unity...", 5 );

		  // Read Common GUI values
		  readGui(m_bridgeDialog);

		  // Read Custom GUI values
		  DzBridgeUnityDialog* unityDialog = qobject_cast<DzBridgeUnityDialog*>(m_bridgeDialog);
		  if (unityDialog)
			InstallUnityFiles = unityDialog->installUnityFilesCheckBox->isChecked();
		  // custom animation filename correction for Unity
		  if (AssetType == "Animation")
		  {
			  if (NonInteractiveMode == 0)
			  {
				  // correct CharacterFolder
				  ExportFolder = CharacterName.left(CharacterName.indexOf("@"));
				  DestinationPath = RootFolder + "/" + ExportFolder + "/";
				  // correct animation filename
				  CharacterFBX = DestinationPath + CharacterName + ".fbx";
			  }
		  }

		  //Create Daz3D folder if it doesn't exist
		  QDir dir;
		  dir.mkpath(RootFolder);
		  exportProgress->step();

		  exportHD(exportProgress);

		  // DB 2021-10-11: Progress Bar
		  exportProgress->finish();

		  // DB 2021-09-02: messagebox "Export Complete"
		  if (NonInteractiveMode == 0)
		  {
			  QMessageBox::information(0, "DazBridge: Unity",
				  tr("Export phase from Daz Studio complete. Please switch to Unity to begin Import phase."), QMessageBox::Ok);
			  if (InstallUnityFiles)
			  {
				  QString destPath = createUnityFiles(true);
#ifdef WIN32
				  ShellExecute(0, 0, destPath.toLocal8Bit().data(), 0, 0, SW_SHOW);
#endif
			  }
		  }

	 }
}

QString DzBridgeUnityAction::createUnityFiles(bool replace)
{
	 if (!InstallUnityFiles)
		  return "";

	 QString srcPath = ":/DazBridgeUnity/daztounity-hdrp.unitypackage";
	 QFile srcFile(srcPath);
	 QString destPath = RootFolder + "/DazToUnity HDRP - Doubleclick to Install.unitypackage";
	 this->copyFile(&srcFile, &destPath, replace);
	 srcFile.close();

	 return destPath;
}

void DzBridgeUnityAction::writeConfiguration()
{
	 QString DTUfilename = DestinationPath + CharacterName + ".dtu";
	 QFile DTUfile(DTUfilename);
	 DTUfile.open(QIODevice::WriteOnly);
	 DzJsonWriter writer(&DTUfile);
	 writer.startObject(true);

	 writeDTUHeader(writer);

	 if (AssetType.toLower().contains("mesh"))
	 {
		 writeAllMaterials(Selection, writer);
		 writeAllMorphs(writer);
		 writeAllSubdivisions(writer);
		 writeAllDforceInfo(Selection, writer);
	 }

	 if (AssetType == "Pose")
	 {
		 writeAllPoses(writer);
	 }

	 if (AssetType == "Environment")
	 {
		 writeEnvironment(writer);
	 }

	 writer.finishObject();
	 DTUfile.close();
}

// Setup custom FBX export options
void DzBridgeUnityAction::setExportOptions(DzFileIOSettings& ExportOptions)
{
	 ExportOptions.setBoolValue("doEmbed", false);
	 ExportOptions.setBoolValue("doDiffuseOpacity", false);
	 ExportOptions.setBoolValue("doCopyTextures", false);

}

QString DzBridgeUnityAction::readGuiRootFolder()
{
	QString rootFolder = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() + "DazToUnity";

	if (m_bridgeDialog)
	{
		QLineEdit* assetsFolderEdit = nullptr;
		DzBridgeUnityDialog* unityDialog = qobject_cast<DzBridgeUnityDialog*>(m_bridgeDialog);

		if (unityDialog)
			assetsFolderEdit = unityDialog->getAssetsFolderEdit();

		if (assetsFolderEdit)
			rootFolder = assetsFolderEdit->text().replace("\\", "/") + "/Daz3D";
	}
	return rootFolder;
}

#include "moc_DzUnityAction.cpp"
