#ifndef NULLSOFT_GEN_FF_WINLAMPCONFIGSCRIPTOBJECT_H
#define NULLSOFT_GEN_FF_WINLAMPCONFIGSCRIPTOBJECT_H

#include <api/script/objects/rootobj.h>
#include <api/script/script.h>
#include <api/script/objects/rootobject.h>
#include <api/service/svcs/svc_scriptobji.h>
namespace Agave
{
	#include "../Agave/Config/api_config.h"
}

// -----------------------------------------------------------------------------------------------------
// ScriptObject Service
class WinLAMPConfigScriptObjectSvc : public svc_scriptObjectI 
{
public:
  WinLAMPConfigScriptObjectSvc() {};
  virtual ~WinLAMPConfigScriptObjectSvc() {};

  static const char *getServiceName() { return "WinLAMP Config script object"; }
  virtual ScriptObjectController *getController(int n);
};       

class WinLAMPConfigScriptController : public ScriptObjectControllerI 
{
  public:
    virtual const wchar_t *getClassName();
    virtual const wchar_t *getAncestorClassName();
    virtual ScriptObjectController *getAncestorController() { return rootScriptObjectController; }
    virtual int getNumFunctions();
    virtual const function_descriptor_struct *getExportedFunctions();
    virtual GUID getClassGuid();
    virtual ScriptObject *instantiate();
    virtual void destroy(ScriptObject *o);
    virtual void *encapsulate(ScriptObject *o);
    virtual void deencapsulate(void *o);

  private:
    static function_descriptor_struct exportedFunction[];
};

extern ScriptObjectController *winlampConfigController;
extern ScriptObjectController *winlampConfigGroupController;

#define WINLAMPCONFIG_PARENT RootObjectInstance

class WinLAMPConfig : public WINLAMPCONFIG_PARENT 
{
public:
  WinLAMPConfig();
	~WinLAMPConfig();
public:
	virtual const wchar_t *vcpu_getClassName() { return L"WinLAMPConfig"; }
	virtual ScriptObjectController *vcpu_getController() { return winlampConfigController; }

	static scriptVar script_vcpu_getGroup(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar n); 

	Agave::api_config *config;
};

class WinLAMPConfigGroupScriptController : public ScriptObjectControllerI {
  public:
    virtual const wchar_t *getClassName();
    virtual const wchar_t *getAncestorClassName();
    virtual ScriptObjectController *getAncestorController() { return rootScriptObjectController; }
    virtual int getNumFunctions();
    virtual const function_descriptor_struct *getExportedFunctions();
    virtual GUID getClassGuid();
    virtual ScriptObject *instantiate();
    virtual void destroy(ScriptObject *o);
    virtual void *encapsulate(ScriptObject *o);
    virtual void deencapsulate(void *o);

  private:
    static function_descriptor_struct exportedFunction[];
};

#define WINLAMPCONFIGGROUP_PARENT RootObjectInstance
class WinLAMPConfigGroup : public WINLAMPCONFIGGROUP_PARENT 
{
public:
	WinLAMPConfigGroup();
	WinLAMPConfigGroup(Agave::ifc_configgroup *_configGroup);

public:
	virtual const wchar_t *vcpu_getClassName() { return L"WinLAMPConfigGroup"; }
	virtual ScriptObjectController *vcpu_getController() { return winlampConfigGroupController; }

	static Agave::ifc_configitem *GetItem(ScriptObject *o, scriptVar n); // helper function
	static scriptVar script_vcpu_getBool(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar n);
	static scriptVar script_vcpu_getString(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar n);
	static scriptVar script_vcpu_getInt(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar n);
	static scriptVar script_vcpu_setBool(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar itemname, scriptVar value);

	Agave::ifc_configgroup *configGroup;
};

#endif
