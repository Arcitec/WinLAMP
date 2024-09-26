#include <precomp.h>
#include "WinLAMPConfigScriptObject.h"

// {B2AD3F2B-31ED-4e31-BC6D-E9951CD555BB}
static const GUID winlampConfigScriptGuid = 
{ 0xb2ad3f2b, 0x31ed, 0x4e31, { 0xbc, 0x6d, 0xe9, 0x95, 0x1c, 0xd5, 0x55, 0xbb } };

// {FC17844E-C72B-4518-A068-A8F930A5BA80}
static const GUID winlampConfigGroupScriptGuid = 
{ 0xfc17844e, 0xc72b, 0x4518, { 0xa0, 0x68, 0xa8, 0xf9, 0x30, 0xa5, 0xba, 0x80 } };

static WinLAMPConfigScriptController _winlampConfigController;
ScriptObjectController *winlampConfigController = &_winlampConfigController;

static WinLAMPConfigGroupScriptController _winlampConfigGroupController;
ScriptObjectController *winlampConfigGroupController = &_winlampConfigGroupController;

BEGIN_SERVICES(WinLAMPConfig_svcs);
DECLARE_SERVICETSINGLE(svc_scriptObject, WinLAMPConfigScriptObjectSvc);
END_SERVICES(WinLAMPConfig_svcs, _WinLAMPConfig_svcs);

#ifdef _X86_
extern "C"
{
	int _link_WinLAMPConfig_svcs;
}
#else
extern "C"
{
	int __link_WinLAMPConfig_svcs;
}
#endif
// -----------------------------------------------------------------------------------------------------
// Service

ScriptObjectController *WinLAMPConfigScriptObjectSvc::getController(int n)
{
	switch (n)
	{
		case 0:
			return winlampConfigController;
		case 1:
			return winlampConfigGroupController;
	}
	return NULL;
}

// -- Functions table -------------------------------------
function_descriptor_struct WinLAMPConfigScriptController::exportedFunction[] = 
{
  {L"getGroup", 1, (void*)WinLAMPConfig::script_vcpu_getGroup },
};

// --------------------------------------------------------

const wchar_t *WinLAMPConfigScriptController::getClassName() 
{
	return L"WinLAMPConfig";
}

const wchar_t *WinLAMPConfigScriptController::getAncestorClassName() 
{
  return L"Object";
}

ScriptObject *WinLAMPConfigScriptController::instantiate() 
{
  WinLAMPConfig *wc = new WinLAMPConfig;
  ASSERT(wc != NULL);
  return wc->getScriptObject();
}

void WinLAMPConfigScriptController::destroy(ScriptObject *o) 
{
  WinLAMPConfig *wc = static_cast<WinLAMPConfig *>(o->vcpu_getInterface(winlampConfigScriptGuid));
	if (wc)
		delete wc;
}

void *WinLAMPConfigScriptController::encapsulate(ScriptObject *o) 
{
  return NULL; // no encapsulation yet
}

void WinLAMPConfigScriptController::deencapsulate(void *o) 
{
}

int WinLAMPConfigScriptController::getNumFunctions() 
{
  return sizeof(exportedFunction) / sizeof(function_descriptor_struct); 
}

const function_descriptor_struct *WinLAMPConfigScriptController::getExportedFunctions() 
{
  return exportedFunction;                                                        
}

GUID WinLAMPConfigScriptController::getClassGuid() 
{
  return winlampConfigScriptGuid;
}

/* ------------- */

WinLAMPConfig::WinLAMPConfig()
{
		getScriptObject()->vcpu_setInterface(winlampConfigScriptGuid, (void *)static_cast<WinLAMPConfig *>(this));
	getScriptObject()->vcpu_setClassName(L"WinLAMPConfig");
	getScriptObject()->vcpu_setController(winlampConfigController);

	waServiceFactory *sf = WASABI_API_SVC->service_getServiceByGuid(Agave::AgaveConfigGUID);
	if (sf)
		config = (Agave::api_config *)sf->getInterface();
}

WinLAMPConfig::~WinLAMPConfig()
{
	waServiceFactory *sf = WASABI_API_SVC->service_getServiceByGuid(Agave::AgaveConfigGUID);
	if (sf)
		sf->releaseInterface(config);
}

scriptVar WinLAMPConfig::script_vcpu_getGroup(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar groupguid)
{
	SCRIPT_FUNCTION_INIT

	const wchar_t *g = GET_SCRIPT_STRING(groupguid);
  GUID _g = nsGUID::fromCharW(g);

	WinLAMPConfig *wc = static_cast<WinLAMPConfig *>(o->vcpu_getInterface(winlampConfigScriptGuid));
	if (wc)
	{
		Agave::ifc_configgroup *group = wc->config->GetGroup(_g);
		if (group)
		{
			WinLAMPConfigGroup *winlampConfigGroup = new WinLAMPConfigGroup(group);
			return MAKE_SCRIPT_OBJECT(winlampConfigGroup->getScriptObject());
		}
	}
	RETURN_SCRIPT_NULL;
}

/* ------------- */

// -- Functions table -------------------------------------
function_descriptor_struct WinLAMPConfigGroupScriptController::exportedFunction[] = 
{
  {L"getBool", 1, (void*)WinLAMPConfigGroup::script_vcpu_getBool },
	{L"getString", 1, (void*)WinLAMPConfigGroup::script_vcpu_getString },
	{L"getInt", 1, (void*)WinLAMPConfigGroup::script_vcpu_getInt },
  {L"setBool", 2, (void*)WinLAMPConfigGroup::script_vcpu_setBool },
};
// --------------------------------------------------------

const wchar_t *WinLAMPConfigGroupScriptController::getClassName() 
{
	return L"WinLAMPConfigGroup";
}

const wchar_t *WinLAMPConfigGroupScriptController::getAncestorClassName() 
{
  return L"Object";
}

ScriptObject *WinLAMPConfigGroupScriptController::instantiate() 
{
  WinLAMPConfigGroup *wc = new WinLAMPConfigGroup;
  ASSERT(wc != NULL);
  return wc->getScriptObject();
}

void WinLAMPConfigGroupScriptController::destroy(ScriptObject *o) 
{
  WinLAMPConfigGroup *wc = static_cast<WinLAMPConfigGroup *>(o->vcpu_getInterface(winlampConfigGroupScriptGuid));
	if (wc)
		delete wc;
}

void *WinLAMPConfigGroupScriptController::encapsulate(ScriptObject *o) 
{
  return NULL; // no encapsulation yet
}

void WinLAMPConfigGroupScriptController::deencapsulate(void *o) 
{
}

int WinLAMPConfigGroupScriptController::getNumFunctions() 
{
  return sizeof(exportedFunction) / sizeof(function_descriptor_struct); 
}

const function_descriptor_struct *WinLAMPConfigGroupScriptController::getExportedFunctions() 
{
  return exportedFunction;                                                        
}

GUID WinLAMPConfigGroupScriptController::getClassGuid() 
{
  return winlampConfigGroupScriptGuid;
}

/* ------------- */

WinLAMPConfigGroup::WinLAMPConfigGroup()
{
getScriptObject()->vcpu_setInterface(winlampConfigGroupScriptGuid, (void *)static_cast<WinLAMPConfigGroup *>(this));
	getScriptObject()->vcpu_setClassName(L"WinLAMPConfigGroup");
	getScriptObject()->vcpu_setController(winlampConfigGroupController);
	configGroup = 0;
}

WinLAMPConfigGroup::WinLAMPConfigGroup(Agave::ifc_configgroup *_configGroup)
{
	getScriptObject()->vcpu_setInterface(winlampConfigGroupScriptGuid, (void *)static_cast<WinLAMPConfigGroup *>(this));
	getScriptObject()->vcpu_setClassName(L"WinLAMPConfigGroup");
	getScriptObject()->vcpu_setController(winlampConfigGroupController);
	configGroup = _configGroup;
}

Agave::ifc_configitem *WinLAMPConfigGroup::GetItem(ScriptObject *o, scriptVar itemname)
{
	const wchar_t *item = GET_SCRIPT_STRING(itemname);
	WinLAMPConfigGroup *group = static_cast<WinLAMPConfigGroup *>(o->vcpu_getInterface(winlampConfigGroupScriptGuid));
	if (group)
	{
		Agave::ifc_configitem *configitem = group->configGroup->GetItem(item);
		return configitem;
	}
	return 0;
}

scriptVar WinLAMPConfigGroup::script_vcpu_getBool(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar itemname)
{
	SCRIPT_FUNCTION_INIT
	Agave::ifc_configitem *configitem = GetItem(o, itemname);
		if (configitem)
			return MAKE_SCRIPT_BOOLEAN(configitem->GetBool());
	
	RETURN_SCRIPT_ZERO;
}

scriptVar WinLAMPConfigGroup::script_vcpu_getString(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar itemname)
{
	SCRIPT_FUNCTION_INIT
	Agave::ifc_configitem *configitem = GetItem(o, itemname);
		if (configitem)
			return MAKE_SCRIPT_STRING(configitem->GetString());
	
	RETURN_SCRIPT_ZERO;
}

scriptVar WinLAMPConfigGroup::script_vcpu_getInt(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar itemname)
{
	SCRIPT_FUNCTION_INIT
	Agave::ifc_configitem *configitem = GetItem(o, itemname);
		if (configitem)
			return MAKE_SCRIPT_INT(configitem->GetInt());
	
	RETURN_SCRIPT_ZERO;
}

scriptVar WinLAMPConfigGroup::script_vcpu_setBool(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar itemname, scriptVar value)
{
	SCRIPT_FUNCTION_INIT
	const wchar_t *item = GET_SCRIPT_STRING(itemname);
	WinLAMPConfigGroup *group = static_cast<WinLAMPConfigGroup *>(o->vcpu_getInterface(winlampConfigGroupScriptGuid));
	if (group)
	{
		Agave::ifc_configitem *configitem = group->configGroup->GetItem(item);
		if (configitem)
			configitem->SetBool(!!GET_SCRIPT_BOOLEAN(value));
	}
	RETURN_SCRIPT_VOID;
}