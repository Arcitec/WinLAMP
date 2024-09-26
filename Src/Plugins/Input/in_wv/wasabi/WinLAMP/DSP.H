#ifndef NULLSOFT_WINLAMP_DSP_H
#define NULLSOFT_WINLAMP_DSP_H
// DSP plugin interface

// notes:
// any window that remains in foreground should optimally pass unused
// keystrokes to the parent (winlamp's) window, so that the user
// can still control it. As for storing configuration,
// Configuration data should be stored in <dll directory>\plugin.ini
// (look at the vis plugin for configuration code)

typedef struct winlampDSPModule {
  char *description;		// description
  HWND hwndParent;			// parent window (filled in by calling app)
  HINSTANCE hDllInstance;	// instance handle to this DLL (filled in by calling app)

  void (*Config)(struct winlampDSPModule *this_mod);  // configuration dialog (if needed)
  int (*Init)(struct winlampDSPModule *this_mod);     // 0 on success, creates window, etc (if needed)

  // modify waveform samples: returns number of samples to actually write
  // (typically numsamples, but no more than twice numsamples, and no less than half numsamples)
  // numsamples should always be at least 128. should, but I'm not sure
  int (*ModifySamples)(struct winlampDSPModule *this_mod, short int *samples, int numsamples, int bps, int nch, int srate);
			
  void (*Quit)(struct winlampDSPModule *this_mod);    // called when unloading

  void *userData; // user data, optional
} winlampDSPModule;

typedef struct {
  int version;       // DSP_HDRVER
  char *description; // description of library
  winlampDSPModule* (*getModule)(int);	// module retrieval function
  int (*sf)(int key); // DSP_HDRVER == 0x21
} winlampDSPHeader;

// exported symbols
#ifdef USE_DSP_HDR_HWND
typedef winlampDSPHeader* (*winlampDSPGetHeaderType)(HWND);
#define DSP_HDRVER 0x22

#else

typedef winlampDSPHeader* (*winlampDSPGetHeaderType)(HWND);
// header version: 0x20 == 0.20 == winlamp 2.0
#define DSP_HDRVER 0x20
#endif

// return values from the winlampUninstallPlugin(HINSTANCE hdll, HWND parent, int param)
// which determine if we can uninstall the plugin immediately or on winlamp restart
#define DSP_PLUGIN_UNINSTALL_NOW    0x0
#define DSP_PLUGIN_UNINSTALL_REBOOT 0x1
//
// uninstall support was added from 5.0+ and uninstall now support from 5.5+
// it is down to you to ensure that if uninstall now is returned that it will not cause a crash
// (ie don't use if you've been subclassing the main window)

// Version note:
//
// Added passing of WinLAMP's main hwnd in the call to the exported winlampDSPHeader()
// which allows for primarily the use of localisation features with the bundled plugins.
// If you want to use the new version then either you can edit you version of dsp.h or
// you can add USE_DSP_HDR_HWND to your project's defined list or before use of dsp.h
//
#endif