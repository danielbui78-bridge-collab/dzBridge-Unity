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

#include "DzUnityDialog.h"
#include "DzUnityAction.h"

#ifdef WIN32
	#include <shellapi.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SUBDIVISION
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __APPLE__
    #define USING_LIBSTDCPP 1
#endif
#include "OpenFBXInterface.h"
#include "OpenSubdivInterface.h"


bool DzBridgeUnityAction::UpgradeToHD(QString baseFilePath, QString hdFilePath, QString outFilePath, std::map<std::string, int>* pLookupTable)
{
	OpenFBXInterface* openFBX = OpenFBXInterface::GetInterface();
	FbxScene* baseMeshScene = openFBX->CreateScene("Base Mesh Scene");
	if (openFBX->LoadScene(baseMeshScene, baseFilePath.toLocal8Bit().data()) == false)
	{
		if (NonInteractiveMode == 0) QMessageBox::warning(0, "Error",
			"An error occurred while loading the base scene...", QMessageBox::Ok);
		printf("\n\nAn error occurred while loading the base scene...");
		return false;
	}
	SubdivideFbxScene subdivider = SubdivideFbxScene(baseMeshScene, pLookupTable);
	subdivider.ProcessScene();
	FbxScene* hdMeshScene = openFBX->CreateScene("HD Mesh Scene");
	if (openFBX->LoadScene(hdMeshScene, hdFilePath.toLocal8Bit().data()) == false)
	{
		if (NonInteractiveMode == 0) QMessageBox::warning(0, "Error",
			"An error occurred while loading the base scene...", QMessageBox::Ok);
		printf("\n\nAn error occurred while loading the base scene...");
		return false;
	}
	subdivider.SaveClustersToScene(hdMeshScene);
	if (openFBX->SaveScene(hdMeshScene, outFilePath.toLocal8Bit().data()) == false)
	{
		if (NonInteractiveMode == 0) QMessageBox::warning(0, "Error",
			"An error occurred while saving the scene...", QMessageBox::Ok);

		printf("\n\nAn error occurred while saving the scene...");
		return false;
	}

	return true;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SUBDIVISION
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DzBridgeUnityAction::DzBridgeUnityAction() :
	 DzRuntimePluginAction(tr("&DazBridge-Unity (Daz To Unity)"), tr("Send the selected node to Unity."))
{
/*
	 NonInteractiveMode = 0;
	 BridgeDialog = nullptr;
//	 m_morphSelectionDialog = nullptr;
//	 m_subdivisionDialog = nullptr;
	 QAction::setIcon(QIcon(":/DazToUnity/Images/icon"));
*/
	 BridgeDialog = nullptr;
	 NonInteractiveMode = 0;
	 AssetType = QString("SkeletalMesh");
	 //Setup Icon
	 QString iconName = "icon";
	 QPixmap basePixmap = QPixmap::fromImage(getEmbeddedImage(iconName.toLatin1()));
	 QIcon icon;
	 icon.addPixmap(basePixmap, QIcon::Normal, QIcon::Off);
	 QAction::setIcon(icon);

}

bool DzBridgeUnityAction::CreateUI()
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
	if (!BridgeDialog) BridgeDialog = new DzBridgeUnityDialog(mw);
	if (!m_subdivisionDialog) m_subdivisionDialog = DzBridgeSubdivisionDialog::Get(BridgeDialog);
	if (!m_morphSelectionDialog) m_morphSelectionDialog = DzBridgeMorphSelectionDialog::Get(BridgeDialog);

	return true;
}

void DzBridgeUnityAction::executeAction()
{

	 // Create and show the dialog. If the user cancels, exit early,
	 // otherwise continue on and do the thing that required modal
	 // input from the user.
	 if (CreateUI() == false)
		 return;

	 // If the Accept button was pressed, start the export
	 int dlgResult = 0;
	 if (NonInteractiveMode == 0)
	 {
		 dlgResult = BridgeDialog->exec();
	 }
	 if (NonInteractiveMode == 1 || dlgResult == QDialog::Accepted)
	 {
		  // DB 2021-10-11: Progress Bar
		  DzProgress exportProgress( "Sending to Unity...", 5 );

		  //Create Daz3D folder if it doesn't exist
		  QDir dir;
		  RootFolder = BridgeDialog->assetsFolderEdit->text() + "/Daz3D";
		  dir.mkpath(RootFolder);

		  // Collect the values from the dialog fields
		  CharacterName = BridgeDialog->getAssetNameEdit()->text();
		  ExportFolder = CharacterName;
		  DestinationPath = RootFolder + "/" + ExportFolder + "/";
		  CharacterFBX = DestinationPath + CharacterName + ".fbx";
		  AssetType = BridgeDialog->getAssetTypeCombo()->currentText().replace(" ", "");
		  MorphString = BridgeDialog->GetMorphString();
		  ExportMorphs = BridgeDialog->getMorphsEnabledCheckBox()->isChecked();
		  ExportSubdivisions = BridgeDialog->getSubdivisionEnabledCheckBox()->isChecked();
		  MorphMapping = BridgeDialog->GetMorphMapping();

		  InstallUnityFiles = BridgeDialog->installUnityFilesCheckBox->isChecked();

		  CreateUnityFiles(true);
		  exportProgress.step();

		  FBXVersion = QString("FBX 2014 -- Binary");

		  if (ExportSubdivisions)
		  {
			  m_subdivisionDialog->LockSubdivisionProperties(false);
			  ExportBaseMesh = true;
			  Export();
			  m_subdivisionDialog->UnlockSubdivisionProperties();
			  exportProgress.step();

		  }


		  m_subdivisionDialog->LockSubdivisionProperties(ExportSubdivisions);
		  ExportBaseMesh = false;
		  Export();
		  exportProgress.step();

		  if (ExportSubdivisions)
		  {
              std::map<std::string, int>* pLookupTable = m_subdivisionDialog->GetLookupTable();
              QString BaseCharacterFBX = DestinationPath + CharacterName + "_base.fbx";
			  // DB 2021-10-02: Upgrade HD
			  if (UpgradeToHD(BaseCharacterFBX, CharacterFBX, CharacterFBX, pLookupTable) == false)
			  {
				  if (NonInteractiveMode == 0) QMessageBox::warning(0, tr("Error"),
						  tr("There was an error during the Subdivision Surface refinement operation, the exported Daz model may not work correctly."), QMessageBox::Ok);
			  }
			  else
			  {
				  // remove intermediate base character fbx
				  // Sanity Check
				  if (QFile::exists(BaseCharacterFBX))
				  {
					  QFile::remove(BaseCharacterFBX);
				  }
			  }
              delete(pLookupTable);
			  exportProgress.step();

		  }

		  // DB 2021-09-02: Unlock and Undo subdivision changes
		  m_subdivisionDialog->UnlockSubdivisionProperties();

		  //Rename the textures folder
		  QDir textureDir(DestinationPath + "/" + CharacterName + ".images");
		  textureDir.rename(DestinationPath + "/" + CharacterName + ".images", DestinationPath + "/Textures");

		  // DB 2021-10-11: Progress Bar
		  exportProgress.finish();

		  // DB 2021-09-02: messagebox "Export Complete"
		  if (NonInteractiveMode == 0)
		  {
			  QMessageBox::information(0, "DazBridge: Unity",
				  tr("Export phase from Daz Studio complete. Please switch to Unity to begin Import phase."), QMessageBox::Ok);
			  QString destPath = RootFolder + "/daztounity.unitypackage";
			  if (InstallUnityFiles)
			  {
#ifdef WIN32
				  ShellExecute(0, 0, destPath.toLocal8Bit().data(), 0, 0, SW_SHOW);
#endif
			  }
		  }

	 }
}

QString DzBridgeUnityAction::GetMD5(const QString &path)
{
	auto algo = QCryptographicHash::Md5;
	QFile sourceFile(path);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly))
    {
        char buffer[bufferSize];
        int bytesRead;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(algo);
        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0)
        {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}

bool DzBridgeUnityAction::CopyFile(QFile *file, QString *dst, bool replace, bool compareFiles)
{
	bool dstExists = QFile::exists(*dst);

	if(replace)
	{
		if(compareFiles && dstExists)
		{
			auto srcFileMD5 = GetMD5(file->fileName());
			auto dstFileMD5 = GetMD5(*dst);

			if(srcFileMD5.length() > 0 && dstFileMD5.length() > 0 && srcFileMD5.compare(dstFileMD5) == 0)
			{
				return false;
			}
		}

		if(dstExists)
		{
			QFile::remove(*dst);
		}
	}

	/*
	if(dstExists)
	{
		QFile::setPermissions(QFile::ReadOther | QFile::WriteOther);
	}
	*/

	//ensure our output destination file has the correct file permissions
	//file->setPermissions(QFile::ReadOther | QFile::WriteOther);

	auto result = file->copy(*dst);

	if(QFile::exists(*dst))
	{
#if __APPLE__
        QFile::setPermissions(*dst, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
#else
        QFile::setPermissions(*dst, QFile::ReadOther | QFile::WriteOther);
#endif
    }

	return result;
}

void DzBridgeUnityAction::CreateUnityFiles(bool replace)
{
	 if (!InstallUnityFiles)
		  return;

	 QString destPath = RootFolder + "/daztounity.unitypackage";
	 QFile file(":/DazBridgeUnity/daztounity.unitypackage");
	 this->CopyFile(&file, &destPath, replace);
	 file.close();

	 ////Create scripts folder if it doesn't exist
	 //QDir dir;
	 //QString scriptsFolder = ExportFolder + "/Scripts";
	 //dir.mkpath(scriptsFolder);

	 //QStringList scripts = QDir(":/DazBridgeUnity/Scripts/").entryList();
	 //for (int i = 0; i < scripts.size(); i++)
	 //{
		//  QString script = scriptsFolder + "/" + scripts[i];
		//  QFile file(":/DazBridgeUnity/Scripts/" + scripts[i]);
		//  CopyFile(&file, &script, replace);
		//  file.close();
	 //}

}

void DzBridgeUnityAction::WriteConfiguration()
{
	 QString DTUfilename = DestinationPath + CharacterName + ".dtu";
	 QFile DTUfile(DTUfilename);
	 DTUfile.open(QIODevice::WriteOnly);
	 DzJsonWriter writer(&DTUfile);
	 writer.startObject(true);
	 writer.addMember("Asset Id", Selection->getAssetId());
	 writer.addMember("Asset Name", CharacterName);
	 writer.addMember("Asset Type", AssetType);
	 writer.addMember("FBX File", CharacterFBX);
	 writer.addMember("Import Folder", DestinationPath);

	 writer.startMemberArray("Materials", true);
	 WriteMaterials(Selection, writer);
	 writer.finishArray();

	 writer.startMemberArray("Morphs", true);
	 if (ExportMorphs)
	 {
		  for (QMap<QString, QString>::iterator i = MorphMapping.begin(); i != MorphMapping.end(); ++i)
		  {
				writer.startObject(true);
				writer.addMember("Name", i.key());
				writer.addMember("Label", i.value());
				writer.finishObject();
		  }
	 }
	 writer.finishArray();

	 writer.startMemberArray("Subdivisions", true);
	 if (ExportSubdivisions)
		  m_subdivisionDialog->WriteSubdivisions(writer);

	 if (AssetType.toLower().contains("skeletalmesh"))
	 {
		 bool ExportDForce = true;
		 writer.startMemberArray("dForce WeightMaps", true);
		 if (ExportDForce)
		 {
			 WriteWeightMaps(Selection, writer);
		 } 
	 }

	 writer.finishArray();
	 writer.finishObject();

	 DTUfile.close();
}

// Setup custom FBX export options
void DzBridgeUnityAction::SetExportOptions(DzFileIOSettings& ExportOptions)
{
	 ExportOptions.setBoolValue("doEmbed", false);
	 ExportOptions.setBoolValue("doDiffuseOpacity", false);
	 ExportOptions.setBoolValue("doCopyTextures", false);

}

// Write out all the surface properties
void DzBridgeUnityAction::WriteMaterials(DzNode* Node, DzJsonWriter& Writer)
{
	 DzObject* Object = Node->getObject();
	 DzShape* Shape = Object ? Object->getCurrentShape() : NULL;

	 // DB (2021-05-24): dForce Additions
	 // 1. check: for dForce modifier in object
	 // 2. if dForce modifier exists, look for SimulationSettingsProvider
	 // 2a. prep script: load FindSimulationProvider script from file/resource
	 // 2b. prep arguments: pass Node object + Material string to script
	 // 2c. execute script
	 // 2d. wait for return
	 // 3. if script call successful, get returnvalue into DzElement* SimulationSettingsProvider
	 // 4. process SimulationsSettingsProvider after other Material properties


	 // 1. check: for dForce modifier in object
	 bool bDForceSettingsAvailable = false;
	 if (Shape)
	 {
		 DzModifierIterator modIter = Object->modifierIterator();
		 while (modIter.hasNext())
		 {
			 DzModifier* modifier = modIter.next();
			 QString mod_Class = modifier->className();
			 if (mod_Class.toLower().contains("dforce"))
			 {
				 bDForceSettingsAvailable = true;
				 break;
			 }
		 }
	 }

	 if (Shape)
	 {
		  for (int i = 0; i < Shape->getNumMaterials(); i++)
		  {
				DzMaterial* Material = Shape->getMaterial(i);
				if (Material)
				{
					 Writer.startObject(true);
					 Writer.addMember("Version", 2);
					 Writer.addMember("Asset Name", Node->getLabel());
					 Writer.addMember("Material Name", Material->getName());
					 Writer.addMember("Material Type", Material->getMaterialName());

					 DzPresentation* presentation = Node->getPresentation();
					 if (presentation != nullptr)
					 {
						  const QString presentationType = presentation->getType();
						  Writer.addMember("Value", presentationType);
					 }
					 else
					 {
						  Writer.addMember("Value", QString("Unknown"));
					 }

					 Writer.startMemberArray("Properties", true);
					 for (int propertyIndex = 0; propertyIndex < Material->getNumProperties(); propertyIndex++)
					 {
						  DzProperty* Property = Material->getProperty(propertyIndex);
						  DzImageProperty* ImageProperty = qobject_cast<DzImageProperty*>(Property);
						  if (ImageProperty)
						  {
								QString Name = Property->getName();
								QString TextureName = "";

								if (ImageProperty->getValue())
								{
									 TextureName = ImageProperty->getValue()->getFilename();
								}

								Writer.startObject(true);
								Writer.addMember("Name", Name);
								Writer.addMember("Value", Material->getDiffuseColor().name());
								Writer.addMember("Data Type", QString("Texture"));
								Writer.addMember("Texture", TextureName);
								Writer.finishObject();
								continue;
						  }

						  DzColorProperty* ColorProperty = qobject_cast<DzColorProperty*>(Property);
						  if (ColorProperty)
						  {
								QString Name = Property->getName();
								QString TextureName = "";

								if (ColorProperty->getMapValue())
								{
									 TextureName = ColorProperty->getMapValue()->getFilename();
								}

								Writer.startObject(true);
								Writer.addMember("Name", Name);
								Writer.addMember("Value", ColorProperty->getColorValue().name());
								Writer.addMember("Data Type", QString("Color"));
								Writer.addMember("Texture", TextureName);
								Writer.finishObject();
								continue;
						  }

						  DzNumericProperty* NumericProperty = qobject_cast<DzNumericProperty*>(Property);
						  if (NumericProperty)
						  {
								QString Name = Property->getName();
								QString TextureName = "";

								if (NumericProperty->getMapValue())
								{
									 TextureName = NumericProperty->getMapValue()->getFilename();
								}

								Writer.startObject(true);
								Writer.addMember("Name", Name);
								Writer.addMember("Value", QString::number(NumericProperty->getDoubleValue()));
								Writer.addMember("Data Type", QString("Double"));
								Writer.addMember("Texture", TextureName);
								Writer.finishObject();
						  }
					 }

					 /***********************************************************************/
					 // Add DForce Settings to Material Properties Array
					 /***********************************************************************/
					 if (bDForceSettingsAvailable)
					 {
						 // look up all Shape methods, find closest match for "findSimulationSettingsProvider"
						 //const QMetaObject *metaObj = Shape->metaObject();
						 //int searchResult = -1;
						 //for (int i = 0; i < metaObj->methodCount(); i++)
						 //{
							//const char* methodSig = metaObj->method(i).signature();
							//if (QString(methodSig).toLower().contains("findsimulationsettingsprovider"))
							//{
							//	searchResult = i;
							//	break;
							//}
						 //}
						 DzElement* elSimulationSettingsProvider;
						 bool ret = false;
						 int methodIndex = -1;
						 methodIndex = Shape->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("findSimulationSettingsProvider(QString)"));
						 if (methodIndex != -1)
						 {
							 QMetaMethod method = Shape->metaObject()->method(methodIndex);
							 QGenericReturnArgument returnArgument(
								 method.typeName(),
								 &elSimulationSettingsProvider
							 );
							 ret = method.invoke(Shape, returnArgument, Q_ARG(QString, Material->getName()));
							 if (elSimulationSettingsProvider)
							 {
								int numProperties = elSimulationSettingsProvider->getNumProperties();
								DzPropertyListIterator propIter = elSimulationSettingsProvider->propertyListIterator();
								QString propString = "";
								int propIndex = 0;
								while (propIter.hasNext())
								{
									DzProperty* Property = propIter.next();
									DzNumericProperty* NumericProperty = qobject_cast<DzNumericProperty*>(Property);
									if (NumericProperty)
									{
										QString Name = Property->getName();
										QString TextureName = "";
										if (NumericProperty->getMapValue())
										{
											TextureName = NumericProperty->getMapValue()->getFilename();
										}
										Writer.startObject(true);
										Writer.addMember("Name", Name);
										Writer.addMember("Value", QString::number(NumericProperty->getDoubleValue()));
										Writer.addMember("Data Type", QString("Double"));
										Writer.addMember("Texture", TextureName);
										Writer.finishObject();
									}
								}

							 }

						 }

						 ////*******************************************************************************
						 //// Below is the old, unused hybrid script/C++ implementation of "findSimulationSettingsProvider"
						 ////*******************************************************************************
						 //// 2. if dForce modifier exists, look for SimulationSettingsProvider
						 //// 2a. prep script: load FindSimulationProvider script from file/resource
						 //DzScript* script = new DzScript();
						 //if (script->loadFromFile(":/UnofficialDaz/dsa/ScriptFunctionFindSimulationSettingsProvider.dsa"))
						 //{
							// // 2b. prep arguments: pass Node object + Material string to script
							// QVariantList args;
							// QVariant varNode;
							// varNode.setValue((QObject*)Node);
							// args.append(varNode);
							// args.append(Material->getName());
							//
							// // 2c. execute script
							// m_ScriptReturn_ReturnCode = 0;
							// m_ScriptReturn_Object = NULL;
							// bool callResult = script->call("ScriptedFindSimulationSettingsProvider", args);
							// if (callResult)
							// {
							//	 // 2d. wait for return
							//	 int timeout = 5;
							//	 bool bTimeout = false;
							//	 while (m_ScriptReturn_Object == NULL && m_ScriptReturn_ReturnCode == 0)
							//	 {
							//		 delay(100);
							//		 if (timeout-- <= 0)
							//		 {
							//			 bTimeout = true;
							//			 break;
							//		 }
							//	 }
							//
							//	 if (bTimeout == false && m_ScriptReturn_ReturnCode > 0)
							//	 {
							//		 // 3. if script call successful, get returnvalue into DzElement* SimulationSettingsProvider
							//		 // 4. process SimulationsSettingsProvider after other Material properties
							//		 DzElement* elSimulationSettingsProvider = (DzElement*) m_ScriptReturn_Object;
							//		 int numProperties = elSimulationSettingsProvider->getNumProperties();
							//		 DzPropertyListIterator* propIter = &elSimulationSettingsProvider->propertyListIterator();
							//		 QString propString = "";
							//		 int propIndex = 0;
							//		 while (propIter->hasNext())
							//		 {
							//			 DzProperty* Property = propIter->next();
							//			 DzNumericProperty* NumericProperty = qobject_cast<DzNumericProperty*>(Property);
							//			 if (NumericProperty)
							//			 {
							//				 QString Name = Property->getName();
							//				 QString TextureName = "";
							//				 if (NumericProperty->getMapValue())
							//				 {
							//					 TextureName = NumericProperty->getMapValue()->getFilename();
							//				 }
							//				 Writer.startObject(true);
							//				 Writer.addMember("Name", Name);
							//				 Writer.addMember("Value", QString::number(NumericProperty->getDoubleValue()));
							//				 Writer.addMember("Data Type", QString("Double"));
							//				 Writer.addMember("Texture", TextureName);
							//				 Writer.finishObject();
							//			 }
							//		 }
							//	 }
							// }
						 //}

					 } // if (bDForceSettingsAvailable)

					 Writer.finishArray();

					 Writer.finishObject();
				}
		  }
	 }

	 DzNodeListIterator Iterator = Node->nodeChildrenIterator();
	 while (Iterator.hasNext())
	 {
		  DzNode* Child = Iterator.next();
		  WriteMaterials(Child, Writer);
	 }
}

// Write weightmaps - recursively traverse parent/children, and export all associated weightmaps
void DzBridgeUnityAction::WriteWeightMaps(DzNode* Node, DzJsonWriter& Writer)
{
	DzObject* Object = Node->getObject();
	DzShape* Shape = Object ? Object->getCurrentShape() : NULL;

	bool bDForceSettingsAvailable = false;

	if ( Shape && Shape->inherits("DzFacetShape") )
	{
		DzModifier *dforceModifier;
		DzModifierIterator modIter = Object->modifierIterator();
		while (modIter.hasNext())
		{
			DzModifier* modifier = modIter.next();
			QString mod_Class = modifier->className();
			if (mod_Class.toLower().contains("dforce"))
			{
				bDForceSettingsAvailable = true;
				dforceModifier = modifier;
				break;
			}
		}

		if (bDForceSettingsAvailable)
		{
			//////////////////////////////////////////
			//// OLD Method for obtaining weightmaps, relying on dForce Weight Modifier Node
			//////////////////////////////////////////
			//// 1. check if weightmap modifier present
			//// 2. if not add an undoable weightnode
			//// 3. use weightnode to find weightmap
			//// 4. extract weightmap weights to file --> tied to Shape?
			//// 5. undo any added weightmap modifier
			//DzWeightMapPtr weightMap = getWeightMapPtr(Node);
			//if (weightMap != NULL)
			//{
			//	int numVerts = Shape->getAssemblyGeometry()->getNumVertices();
			//	unsigned short* weights = weightMap->getWeights();
			//	char* buffer = (char*)weights;
			//	int byte_length = numVerts * sizeof(unsigned short);
			//	// export to raw file
			//	QString filename = QString("%1-old.raw_dforce_map.bytes").arg(Node->getLabel());
			//	QFile rawWeight(CharacterFolder + filename);
			//	if (rawWeight.open(QIODevice::ReadWrite))
			//	{
			//		int bytesWritten = rawWeight.write(buffer, byte_length);
			//		if (bytesWritten != byte_length)
			//		{
			//			// write error
			//			QString errString = rawWeight.errorString();
			//			QMessageBox::warning(0, tr("Error"),
			//				errString, QMessageBox::Ok);
			//		}
			//		rawWeight.close();
			//	}
			//}

			////////////////////////////////////////////
			//// EVEN OLDER Method
			////////////////////////////////////////////
			//DzNodeListIterator Iterator = Node->nodeChildrenIterator();
			//while (Iterator.hasNext())
			//{
			//	DzNode* Child = Iterator.next();
			//	if (Child->className().contains("DzDForceModifierWeightNode"))
			//	{
			//		QObject *handler;
			//		if (metaInvokeMethod(Child, "getWeightMapHandler()", handler))
			//		{
			//			QObject* weightGroup;
			//			if (metaInvokeMethod(handler, "currentWeightGroup()", weightGroup))
			//			{
			//				QObject* context;
			//				if (metaInvokeMethod(weightGroup, "currentWeightContext()", context))
			//				{
			//					// DzWeightMapPtr
			//					QMetaMethod metaMethod = context->metaObject()->method(30); // getWeightMap()
			//					DzWeightMapPtr weightMap;
			//					QGenericReturnArgument returnArgument(
			//						metaMethod.typeName(),
			//						&weightMap
			//					);
			//					int result = metaMethod.invoke((QObject*)context, returnArgument);
			//					if (result != -1)
			//					{
			//						if (weightMap != NULL)
			//						{										 
			//							int numVerts = Shape->getAssemblyGeometry()->getNumVertices();
			//							unsigned short* weights = weightMap->getWeights();
			//							char* buffer = (char*)weights;
			//
			//							// export to raw file
			//							QString filename = QString("%1.raw_dforce_map").arg(Node->getLabel());
			//							QFile rawWeight(CharacterFolder + filename);
			//							if (rawWeight.open(QIODevice::WriteOnly))
			//							{
			//								int bytesWritten = rawWeight.write(buffer, sizeof(weights) * numVerts);
			//								if (bytesWritten != sizeof(weights) * numVerts)
			//								{
			//									// write error
			//									QString errString = rawWeight.errorString();
			//									QMessageBox::warning(0, tr("Error"),
			//										errString, QMessageBox::Ok);
			//								}
			//								rawWeight.close();
			//							}
			//						}
			//
			//					}
			//				}
			//			}
			//		}
			//
			//		//// DzDForceModifierWeightHandler
			//		//QMetaMethod metaMethod = Child->metaObject()->method(372); // getWeightMapHandler
			//		//DzBase* handler;
			//		//QGenericReturnArgument returnArgument(
			//		//	metaMethod.typeName(),
			//		//	&handler
			//		//);
			//		//int result = metaMethod.invoke((QObject*)Child, returnArgument);
			//		//if (result)
			//		//{
			//		//	if (handler != NULL)
			//		//	{
			//		//		// DzDForceModifierWeightGroup
			//		//		QMetaMethod metaMethod = handler->metaObject()->method(18); // getWeightGroup
			//		//		QObject* weightGroup;
			//		//		QGenericReturnArgument returnArgument(
			//		//			metaMethod.typeName(),
			//		//			&weightGroup
			//		//		);
			//		//		int result = metaMethod.invoke((QObject*)handler, returnArgument, Q_ARG(int, 0));
			//		//		if (result)
			//		//		{
			//		//			if (weightGroup != NULL)
			//		//			{
			//		//				// DzWeightMapContext
			//		//				QMetaMethod metaMethod = weightGroup->metaObject()->method(19); // getWeightMapContext
			//		//				QObject* context;
			//		//				QGenericReturnArgument returnArgument(
			//		//					metaMethod.typeName(),
			//		//					&context
			//		//				);
			//		//				int result = metaMethod.invoke((QObject*)weightGroup, returnArgument, Q_ARG(int, 0));
			//		//				if (result)
			//		//				{
			//		//					if (context != NULL)
			//		//					{
			//		//						// DzWeightMapPtr
			//		//						QMetaMethod metaMethod = context->metaObject()->method(30); // getWeightMap
			//		//						DzWeightMapPtr weightMap;
			//		//						QGenericReturnArgument returnArgument(
			//		//							metaMethod.typeName(),
			//		//							&weightMap
			//		//						);
			//		//						int result = metaMethod.invoke((QObject*)context, returnArgument);
			//		//						if (result)
			//		//						{
			//		//							if (weightMap != NULL)
			//		//							{
			//		//								int numVerts = Shape->getAssemblyGeometry()->getNumVertices();
			//		//								unsigned short *weights = weightMap->getWeights();
			//		//								char *buffer = (char*)weights;
			//		//
			//		//								// export to raw file
			//		//								QString filename = QString("%1.raw_dforce_map").arg(Node->getLabel());
			//		//								QFile rawWeight(CharacterFolder + filename);
			//		//								if (rawWeight.open(QIODevice::WriteOnly))
			//		//								{
			//		//									int bytesWritten = rawWeight.write(buffer, sizeof(weights) * numVerts);
			//		//									if (bytesWritten != sizeof(weights) * numVerts)
			//		//									{
			//		//										// write error
			//		//										QString errString = rawWeight.errorString();
			//		//										QMessageBox::warning(0, tr("Error"),
			//		//											errString, QMessageBox::Ok);
			//		//									}
			//		//									rawWeight.close();
			//		//								}
			//		//							}
			//		//						}
			//		//					}
			//		//				}
			//		//			}
			//		//		}
			//		//	}
			//		//}
			//	}
			//
			//}

			///////////////////////////////////////////////
			// NEW Method for obtaining weightmaps, grab directly from dForce Modifier Node
			///////////////////////////////////////////////
			int methodIndex = dforceModifier->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("getInfluenceWeights()"));
			if (methodIndex != -1)
			{
				QMetaMethod method = dforceModifier->metaObject()->method(methodIndex);
				DzWeightMap *weightMap;
				QGenericReturnArgument returnArg(
					method.typeName(),
					&weightMap
				);
				int result = method.invoke((QObject*)dforceModifier, returnArg);
				if (result != -1)
				{
					if (weightMap)
					{
						int numVerts = Shape->getAssemblyGeometry()->getNumVertices();
						unsigned short* daz_weights = weightMap->getWeights();
						int byte_length = numVerts * sizeof(unsigned short);

						char* buffer = new char[byte_length];
						unsigned short* unity_weights = (unsigned short*) buffer;

						// load material groups to remap weights to unity's vertex order
						DzFacetMesh* facetMesh = dynamic_cast<DzFacetShape*>(Shape)->getFacetMesh();
						if (facetMesh)
						{
							// sanity check
							if (numVerts != facetMesh->getNumVertices())
							{
								// throw error if needed
								dzApp->log("Unofficial DTU Bridge: ERROR Exporting Weight Map to file.");
								return;
							}
							int numMaterials = facetMesh->getNumMaterialGroups();
							std::list<MaterialGroupExportOrderMetaData> exportQueue;
							DzFacet* facetPtr = facetMesh->getFacetsPtr();

							// generate export order queue
							// first, populate export queue with materialgroups
							for (int i = 0; i < numMaterials; i++)
							{
								DzMaterialFaceGroup* materialGroup = facetMesh->getMaterialGroup(i);
								int numFaces = materialGroup->count();
								const int* indexPtr = materialGroup->getIndicesPtr();
								int offset = facetPtr[indexPtr[0]].m_vertIdx[0];
								int count = -1;
								MaterialGroupExportOrderMetaData *metaData = new MaterialGroupExportOrderMetaData(i, offset);
								exportQueue.push_back(*metaData);
							}

							// sort: uses operator< to order by vertex_offset
							exportQueue.sort();

							/////////////////////////////////////////
							// for building vertex index lookup tables
							/////////////////////////////////////////
							int material_vertex_count = 0;
							int material_vertex_offset = 0;
							int* DazToUnityLookup = new int[numVerts];
							for (int i = 0; i < numVerts; i++) { DazToUnityLookup[i] = -1; }
							int* UnityToDazLookup = new int[numVerts];
							for (int i = 0; i < numVerts; i++) { UnityToDazLookup[i] = -1; }

							int unity_weightmap_vertexindex = 0;
							// iterate through sorted material groups...
							for (std::list<MaterialGroupExportOrderMetaData>::iterator export_iter = exportQueue.begin(); export_iter != exportQueue.end(); export_iter++)
							{
								// update the vert_offset for each materialGroup
								material_vertex_offset = material_vertex_offset + material_vertex_count;
								material_vertex_count = 0;
								int check_offset = export_iter->vertex_offset;

								// retrieve material group based on sorted material index list
								int materialGroupIndex = export_iter->materialIndex;
								DzMaterialFaceGroup* materialGroup = facetMesh->getMaterialGroup(materialGroupIndex);
								int numFaces = materialGroup->count();
								// pointer for faces in materialGroup
								const int* indexPtr = materialGroup->getIndicesPtr();

								// get each face in materialGroup, then iterate through all vertex indices in the face
								// copy out weights into buffer using material group's vertex ordering, but cross-referenced with internal vertex array indices

								// get the i-th index into the index array of faces, then retrieve the j-th index into the vertex index array
								// i is 0 to number of faces (aka facets), j is 0 to number of vertices in the face
								for (int i = 0; i < numFaces; i++)
								{
									int vertsPerFacet = (facetPtr->isQuad()) ? 4 : 3;
									for (int j = 0; j < vertsPerFacet; j++)
									{
										// retrieve vertex index into daz internal vertex array (probably a BST in array format)
										int vert_index = facetPtr[indexPtr[i]].m_vertIdx[j];

										///////////////////////////////////
										// NOTE: Since the faces will often share/re-use the same vertex, we need to skip
										// any vertex that has already been recorded, since we want ONLY unique vertices
										// in weightmap.  This is done by creating checking back into a DazToUnity vertex index lookup table
										///////////////////////////////////
										// unique vertices will not yet be written and have default -1 value
										if (DazToUnityLookup[vert_index] == -1)
										{
											// This vertex is unique, record into the daztounity lookup table and proceed with other operations
											// to be performend on unqiue verts.
											DazToUnityLookup[vert_index] = unity_weightmap_vertexindex;

											// use the vertex index to cross-reference to the corresponding weightmap value and copy out to buffer for exporting
											// (only do this for unique verts)
											unity_weights[unity_weightmap_vertexindex] = weightMap->getWeight(vert_index);
											//unity_weights[unity_weightmap_vertexindex] = daz_weights[vert_index];

											// Create the unity to daz vertex lookup table (only do this for unique verts)
											UnityToDazLookup[unity_weightmap_vertexindex] = vert_index;

											// increment the unity weightmap vertex index (only do this for unique verts)
											unity_weightmap_vertexindex++;
										}

									} //for (int j = 0; j < vertsPerFace; j++)

								} // for (int i = 0; i < numFaces; i++)

							} // for (std::list<MaterialGroupExportOrderMetaData>::iterator export_iter = exportQueue.begin(); export_iter != exportQueue.end(); export_iter++)

							// export to dforce_weightmap file
							QString filename = QString("%1.dforce_weightmap.bytes").arg(Node->getLabel());
							QFile rawWeight(DestinationPath + filename);
							if (rawWeight.open(QIODevice::WriteOnly))
							{
								int bytesWritten = rawWeight.write(buffer, byte_length);
								if (bytesWritten != byte_length)
								{
									// write error
									QString errString = rawWeight.errorString();
									if (NonInteractiveMode == 0) QMessageBox::warning(0, 
										tr("Error writing dforce weightmap. Incorrect number of bytes written."), 
										errString, QMessageBox::Ok);
								}
								rawWeight.close();
							}

						} // if (facetMesh) /** facetMesh null? */
					} // if (weightMap) /** weightmap null? */
				} // if (result != -1) /** invokeMethod failed? */
			} // if (methodIndex != -1) /** findMethod failed? */
		} // if (bDForceSettingsAvailable) /** no dforce data found */
	} // if (Shape)

	DzNodeListIterator Iterator = Node->nodeChildrenIterator();
	while (Iterator.hasNext())
	{
		DzNode* Child = Iterator.next();
		WriteWeightMaps(Child, Writer);
	}

}


// OLD Method for obtaining weightmap, relying on dForce Weight Modifier Node
DzWeightMapPtr DzBridgeUnityAction::getWeightMapPtr(DzNode* Node)
{
	// 1. check if weightmap modifier present
	DzNodeListIterator Iterator = Node->nodeChildrenIterator();
	while (Iterator.hasNext())
	{
		DzNode* Child = Iterator.next();
		if (Child->className().contains("DzDForceModifierWeightNode"))
		{
			QObject* handler;
			if (metaInvokeMethod(Child, "getWeightMapHandler()", (void**) &handler))
			{
				QObject* weightGroup;
				if (metaInvokeMethod(handler, "currentWeightGroup()", (void**) &weightGroup))
				{
					QObject* context;
					if (metaInvokeMethod(weightGroup, "currentWeightContext()", (void**) &context))
					{
						DzWeightMapPtr weightMap;
						// DzWeightMapPtr
						QMetaMethod metaMethod = context->metaObject()->method(30); // getWeightMap()
						QGenericReturnArgument returnArgument(
							metaMethod.typeName(),
							&weightMap
						);
						int result = metaMethod.invoke((QObject*)context, returnArgument);
						if (result != -1)
						{
							return weightMap;
						}
					}
				}
			}
		}

	}

	return NULL;

}

bool DzBridgeUnityAction::metaInvokeMethod(QObject* object, const char* methodSig, void** returnPtr)
{
	if (object == NULL)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////
	// REFERENCE Signatures obtained by QMetaObject->method() query
	//////////////////////////////////////////////////////////////////
	//
	// DzDForceModifierWeightNode::getWeightMapHandler() = 372
	//
	// DzDForceModifierWeightHandler::getWeightGroup(int) = 18
	// DzDForceModifierWeightHandler::currentWeightGroup() = 20
	//
	// DzDForceModifierWeightGroup::getWeightMapContext(int) = 19
	// DzDForceModifierWeightGroup::currentWeightContext() = 22
	//
	// DzDForceModiferMapContext::getWeightMap() = 30
	/////////////////////////////////////////////////////////////////////////

	// find the metamethod
	const QMetaObject *metaObject = object->metaObject();
	int methodIndex = metaObject->indexOfMethod(QMetaObject::normalizedSignature(methodSig));
	if (methodIndex == -1)
	{
		// use fuzzy search
		// look up all methods, find closest match for methodSig
		int searchResult = -1;
		QString fuzzySig = QString(QMetaObject::normalizedSignature(methodSig)).toLower().remove("()");
		for (int i = 0; i < metaObject->methodCount(); i++)
		{
			const char* sig = metaObject->method(i).signature();
			if (QString(sig).toLower().contains(fuzzySig))
			{
				searchResult = i;
				break;
			}
		}
		if (searchResult == -1)
		{
			return false;
		}
		else
		{
			methodIndex = searchResult;
		}

	}

	// invoke metamethod
	QMetaMethod metaMethod = metaObject->method(methodIndex); 
	void* returnVal;
	QGenericReturnArgument returnArgument(
		metaMethod.typeName(),
		&returnVal
	);
	int result = metaMethod.invoke((QObject*)object, returnArgument);
	if (result)
	{
		// set returnvalue
		*returnPtr = returnVal;

		return true;
	}

	return false;
}


#include "moc_DzUnityAction.cpp"
