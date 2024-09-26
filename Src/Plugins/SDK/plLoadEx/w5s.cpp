#include "ExComponent.h" // the component we're registering is defined here

ExComponent exComponent; // our component

// WinLAMP GetProcAddress()'s this after loading your w5s file
extern "C" __declspec(dllexport) ifc_wa5component *GetWinLAMP5SystemComponent()
{
	return &exComponent;
}
