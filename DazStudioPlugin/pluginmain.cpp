#include "dzplugin.h"
#include "dzapp.h"

#include "version.h"
#include "DzUnityAction.h"
#include "DzUnityDialog.h"

#ifdef __APPLE__
DZ_PLUGIN_DEFINITION("Daz To Unity Bridge");
#else
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
static DzPlugin s_pluginDef("Daz To Unity Bridge");
extern "C" __declspec(dllexport) DzVersion getSDKVersion() { return DZ_SDK_VERSION; }
extern "C" __declspec(dllexport) DzPlugin * getPluginDefinition() { return &s_pluginDef; }
#endif

DZ_PLUGIN_AUTHOR("Daz 3D, Inc");

DZ_PLUGIN_VERSION(PLUGIN_MAJOR, PLUGIN_MINOR, PLUGIN_REV, PLUGIN_BUILD);

DZ_PLUGIN_DESCRIPTION(QString(
	"<b>Pre-Release DazToUnity Bridge v%1.%2.%3.%4 </b><br>\
Bridge Collaboration Project<br><br>\
<a href = \"https://github.com/danielbui78-bridge-collab/DazToRuntime/tree/unreal-main\">Github</a><br><br>"
).arg(PLUGIN_MAJOR).arg(PLUGIN_MINOR).arg(PLUGIN_REV).arg(PLUGIN_BUILD));

DZ_PLUGIN_CLASS_GUID(DzUnityAction, 2C2AA695-652C-4FA9-BE48-E0AB954E28AB);
DZ_PLUGIN_CUSTOM_CLASS_GUID(DzUnityDialog, 06cf5776-8e81-4a81-bad8-619ed1205b58);


static QWidget* GetParentArg0(const QVariantList& args)
{
	QWidget* parent = nullptr;
	QVariant qvar = args[0];
	QObject* obj = qvar.value<QObject*>();
	if (obj && obj->inherits("QWidget"))
	{
		parent = (QWidget*)obj;
	}

	return parent;
}

QObject* DzUnityDialogFactory::createInstance(const QVariantList& args) const
{
	QWidget* parent = GetParentArg0(args);
	return (QObject*) new DzUnityDialog(parent);
}
QObject* DzUnityDialogFactory::createInstance() const
{
	return (QObject*) new DzUnityDialog();
}

#ifdef UNITTEST_DZBRIDGE

#include "UnitTest_DzUnityAction.h"
#include "UnitTest_DzUnityDialog.h"

DZ_PLUGIN_CLASS_GUID(UnitTest_DzUnityAction, 17637434-188f-46eb-81e2-8829f2440742);
DZ_PLUGIN_CLASS_GUID(UnitTest_DzUnityDialog, ca9c9f54-236d-4ab6-bca3-1cf6c3f93f6a);

#endif