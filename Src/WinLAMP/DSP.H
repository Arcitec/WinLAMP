#ifndef NULLSOFT_WINLAMP_DSP_H
#define NULLSOFT_WINLAMP_DSP_H
// DSP plugin interface

typedef struct winlampDSPModule
{
	char *description;		// description
	HWND hwndParent;			// parent window (filled in by calling app)
	HINSTANCE hDllInstance;	// instance handle to this DLL (filled in by calling app)

	void( __cdecl *Config )( struct winlampDSPModule *this_mod );  // configuration dialog (if needed)
	int( __cdecl *Init )( struct winlampDSPModule *this_mod );     // 0 on success, creates window, etc (if needed)

	// modify waveform samples: returns number of samples to actually write
	// (typically numsamples, but no more than twice numsamples, and no less than half numsamples)
	// numsamples should always be at least 128. should, but I'm not sure
	int( __cdecl *ModifySamples )( struct winlampDSPModule *this_mod, short int *samples, int numsamples, int bps, int nch, int srate );

	void( __cdecl *Quit )( struct winlampDSPModule *this_mod );    // called when unloading

	void *userData; // user data, optional
} winlampDSPModule;

typedef struct
{
	int version;       // DSP_HDRVER
	char *description; // description of library
	winlampDSPModule *( __cdecl *getModule )( int );	// module retrieval function
	int( __cdecl *sf )( int key ); // DSP_HDRVER == 0x21
} winlampDSPHeader;

// exported symbols
#ifdef USE_DSP_HDR_HWND
typedef winlampDSPHeader *( __cdecl *winlampDSPGetHeaderType )( HWND );
#define DSP_HDRVER 0x22
#else
// Note: Unless using USE_DSP_HDR_HWND or a WinLAMP 5.5+ client then winlampDSPGetHeaderType(..)
//       will not receive a HWND parameter & with be called as winlampDSPGetHeaderType(void).
//       This is only defined with an HWND to allow for correct compiling of the client exe.
typedef winlampDSPHeader *( __cdecl *winlampDSPGetHeaderType )( HWND );
// header version: 0x20 == 0.20 == winlamp 2.0
#define DSP_HDRVER 0x20
#endif

// These are the return values to be used with the uninstall plugin export function:
// __declspec(dllexport) int __cdecl winlampUninstallPlugin(HINSTANCE hDllInst, HWND hwndDlg, int param)
// which determines if WinLAMP can uninstall the plugin immediately or on winlamp restart.
// If this is not exported then WinLAMP will assume an uninstall with reboot is the only way.
//
#define DSP_PLUGIN_UNINSTALL_NOW    0x0
#define DSP_PLUGIN_UNINSTALL_REBOOT 0x1
//
// Uninstall support was added from 5.0+ and uninstall now support from 5.5+ though note
// that it is down to you to ensure that if uninstall now is returned that it will not
// cause a crash i.e. don't use if you've been subclassing the main window.
//
// The HWND passed in the calling of winlampUninstallPlugin(..) is the preference page HWND.
//
//
// Version note:
//
// Added passing of WinLAMP's main hwnd in the call to the exported winlampDSPHeader()
// which allows for primarily the use of localisation features with the bundled plugins.
// If you want to use the new version then either you can edit you version of dsp.h or
// you can add USE_DSP_HDR_HWND to your project's defined list or before use of dsp.h
//

// For a DSP plugin to be correctly detected by WinLAMP you need to ensure that
// the exported winlampDSPGetHeader2(..) is exported as an undecorated function
// e.g.
// #ifdef __cplusplus
//   extern "C" {
// #endif
// __declspec(dllexport) winlampDSPGetHeaderType * __cdecl winlampDSPGetHeader2(){ return &plugin; }
// #ifdef __cplusplus
//   }
// #endif
//

#endif