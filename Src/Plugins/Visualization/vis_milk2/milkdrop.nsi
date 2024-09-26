;   LICENSE
;   -------
; Copyright 2005-2013 Nullsoft, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without modification, 
; are permitted provided that the following conditions are met:
; 
;   * Redistributions of source code must retain the above copyright notice,
;     this list of conditions and the following disclaimer. 
; 
;   * Redistributions in binary form must reproduce the above copyright notice,
;     this list of conditions and the following disclaimer in the documentation
;     and/or other materials provided with the distribution. 
; 
;   * Neither the name of Nullsoft nor the names of its contributors may be used to 
;     endorse or promote products derived from this software without specific prior written permission. 
;  
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
; FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
; CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
; IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
; OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; ----------------------------------------------------------------
; NOTE: this .nsi script was last built with 'makensis.exe' 
; version 1.98.  If you go to install it (unchanged) and get errors,
; try tracking down that older version, or try to figure out what
; has changed since then.  You can get NSIS at:
;
;               http://www.nullsoft.com/free/nsis/
; 
; This installer will produce a single EXE file that, when run, 
; will decompress and install your plugin and all of its data files 
; to WinLAMP 2 or WinLAMP 3.
; ----------------------------------------------------------------
; MODIFYING THIS SCRIPT TO CREATE AN INSTALLER FOR YOUR OWN PLUGIN:
; ----------------------------------------------------------------
;   1) there are three sections below, each marked 'EDIT THIS SECTION 
;        [x/3]' where 'x' is the section number.  These are the 3
;        places where you need to make changes to customize this
;        installer script for your particular plugin.  Go to each
;        and make the changes, reading the comments there for
;        more information.
;   2) download NSIS from http://www.nullsoft.com/free/nsis/, if you
;        haven't already.
;   3) run the command 'makensis.exe installer.nsi' to build
;        the executable.  (note: ignore the warning message about
;        'InstallRegKey' being used multiple times; this is necessary
;        to determine whether WinLAMP 2 or 3 is a better candidate
;        for the install.)
; ----------------------------------------------------------------
; WHAT THIS INSTALLER SCRIPT DOES:
; ----------------------------------------------------------------
;   If WinLAMP 2 is installed, it will install your plugin to WinLAMP 2, 
;   in the directory WinLAMP\Plugins (or whatever is specified as the 
;   vis plugins path, in WinLAMP\winlamp.ini).  It will also select
;   the plugin as the current plugin.  (Note that WinLAMP must be
;   closed to do this, so if it's open, the installer will ask the 
;   user to close it before proceeding.)  At the end of a successful
;   install, it asks if they'd like to run WinLAMP.
; 
;   If WinLAMP 2 is not present but WinLAMP 3 is, or if the user manually
;   selects the WinLAMP 3 directory to install to, the plugin will be 
;   installed to WinLAMP 3 as a classic visualization plugin, to the 
;   directory WinLAMP3\Plugins.  At install time, if ClassicVis is not
;   installed, it will prompt the user to go download it.  If they
;   don't download it, it will tell them the installation failed.
;   If they already had it, or after they presumably download it,
;   the installer will briefly tell them how to select their new 
;   plugin and run it in WinLAMP 3, using ClassicVis.  Finally, at 
;   the end of a successful install, it asks if they'd like to run 
;   WinLAMP 3.
; ----------------------------------------------------------------



; -------------------- EDIT THIS SECTION [1/3] --------------------
; -------------------- EDIT THIS SECTION [1/3] --------------------
; -------------------- EDIT THIS SECTION [1/3] --------------------

  !define PLUGIN_NAME     "MilkDrop 2"                  ; Brief name of the component.  Can have spaces in it.
  !define INSTALL_CAPTION "MilkDrop 2.2 Setup"       ; Caption for the installer dialog
  !define PLUGIN_DLL      "vis_milk2.dll"              ; The filename of the actual plugin
  !define PLUGIN_OUTFILE  "milkdrop_2.exe"          ; Name of the installer to create

; ----------------------- END SECTION [1/3] -----------------------
; ----------------------- END SECTION [1/3] -----------------------
; ----------------------- END SECTION [1/3] -----------------------

; ----------------------------------------------------------------
Name ${PLUGIN_NAME}
Caption "${INSTALL_CAPTION}"
OutFile ${PLUGIN_OUTFILE}
; ----------------------------------------------------------------

; to determine the install directory, we start with Program Files\WinLAMP.
; then, if winlamp 3 is found, override this and use that directory.
; finally, if winlamp 2 is found, override again and use that directory.
InstallDir $PROGRAMFILES\WinLAMP
InstallDirRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinLAMP3" "UninstallString"
InstallDirRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinLAMP" "UninstallString"
DirText \
    "The installer has determined that this is the path to WinLAMP.$\rIf it is not correct, please change it.  You will not be able to proceed$\runtil a valid path is found." \
    "Select the directory in which WinLAMP 2 or 3 is installed:"

Function .onInit
  IfFileExists $INSTDIR\winlamp.exe End      ; else if winlamp.exe found (winlamp2), we're good
  IfFileExists $INSTDIR\winlamp3.exe End     ; if winlamp3.exe found (winlamp3), we're good
  IfFileExists $INSTDIR\studio.exe End      ; or if studio.exe found (older version of winlamp3), we're good
  ; the above will work fine if winlamp 2 or 3 is installed,
  ; but will break if winlamp 2 or 3 was uninstalled *but the directory remains.*
  IfFileExists $PROGRAMFILES\WinLAMP\winlamp.exe SelectNaturalWinLAMP2         ; last but not least, try the default winlamp 2 directory
  IfFileExists $PROGRAMFILES\WinLAMP3\winlamp3.exe SelectNaturalWinLAMP3       ; otherwise, try the default winlamp 3 directory
  IfFileExists $PROGRAMFILES\WinLAMP3\studio.exe SelectNaturalWinLAMP3        ; otherwise, try the default winlamp 3 directory
  ; if all of these failed, no good path to WinLAMP (2 or 3) could be found,
  ; and the 'Next' button will be disabled until the user can specify
  ; the correct folder.
  Goto End
  
  SelectNaturalWinLAMP3:
    strcpy $INSTDIR $PROGRAMFILES\WinLAMP3
    goto End
    
  SelectNaturalWinLAMP2:
    strcpy $INSTDIR $PROGRAMFILES\WinLAMP
    goto End  
  
  End:
FunctionEnd


Function .onVerifyInstDir
  IfFileExists $INSTDIR\WinLAMP.exe DirOk
  IfFileExists $INSTDIR\WinLAMP3.exe DirOk
  IfFileExists $INSTDIR\Studio.exe DirOk
  Abort   ; leaves the directory as the selected one, but disables the 'Next' button...
  
  DirOk:
FunctionEnd


  
  

Function QueryWinLAMPVisPath 
  ; input: $INSTDIR, the currently-selected install dir (path to winlamp)
  ; output: $1, the path to the winlamp vis plugins subdirectory
  ;           -for winlamp 3x, this is just $INSTDIR\plugins
  ;           -for winlamp 2x, it comes from the winlamp.ini file
  ;             (or just $INSTDIR\plugins if there is an error reading it.)
  IfFileExists $INSTDIR\WinLAMP.exe CaseWinLAMP2
  IfFileExists $INSTDIR\WinLAMP3.exe CaseWinLAMP3
  IfFileExists $INSTDIR\Studio.exe CaseWinLAMP3  ; legacy check
  goto CaseImpossible
          
  CaseWinLAMP2:
    StrCpy $1 $INSTDIR\Plugins\MilkDrop2
    ReadINIStr $8 $INSTDIR\winlamp.ini WinLAMP VisDir
    StrCmp $8 "" End
    IfFileExists $8 0 End
      StrCpy $1 $8 ; update dir
    goto end
  
  CaseWinLAMP3: 
  CaseImpossible:
    StrCpy $1 $INSTDIR\Plugins\MilkDrop2
    goto end
    
  End: 
FunctionEnd


; The stuff to install
Section ""

  CloseWinLAMP2:
    ; make sure winlamp is closed before we 1) try to install files
    ; and 2) (later) edit winlamp.ini.  for 1), if they're running
    ; (or were running) some other vms-based plugin using 
    ; vms_desktop.dll, then if winlamp is still open, the installer
    ; could have trouble overwriting vms_desktop.dll, or other files;
    ; the user would get an abort/retry/ignore box, but it's easier 
    ; to just play it safe.
    FindWindow $R0 "winlamp v1.x"
    StrCmp $R0 0 "" RequestCloseWinLAMP2
    goto WinLAMP2Closed
  RequestCloseWinLAMP2:
    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "WinLAMP is currently running; please close it,$\rand then hit OK to continue..." \
        IDCANCEL WarnWinLAMPStillOpen
    goto CloseWinLAMP2
  WarnWinLAMPStillOpen:
    FindWindow $R0 "winlamp v1.x"
    StrCmp $R0 0 "" WarnWinLAMPStillOpen2
    goto WinLAMP2Closed
  WarnWinLAMPStillOpen2:
    MessageBox MB_OK|MB_ICONEXCLAMATION "Warning: WinLAMP is still open; as a result, the installer$\rwill not be able to set ${PLUGIN_NAME} as the default plugin; you will$\rhave to do this yourself.$\r$\rTo do so, wait until the installation is finished.  Then bring up $\rWinLAMP and hit CTRL+K.  From there, you will be able to select$\r${PLUGIN_NAME} from the list of visualization plug-ins, and it will$\rbecome the new default."
    goto WinLAMP2Closed
  WinLAMP2Closed:    

  ; this function sets $1 to point to the 'winlamp\plugins\milkdrop2' folder:
  ; (see function definition above)
  Call QueryWinLAMPVisPath

  ; -------------------- EDIT THIS SECTION [2/3] --------------------
  ; -------------------- EDIT THIS SECTION [2/3] --------------------
  ; -------------------- EDIT THIS SECTION [2/3] --------------------

  ; LIST FILES TO INCLUDE WITH THE INSTALLER

  ; For each file we want to install onto the destination system,
  ; we first set the output directory (relative to $1, which is
  ; the PLUGINS directory) and then list files.  The paths for
  ; the files will be local paths on your hard disk, but fear not -
  ; the files will be placed in the current output directory 
  ; (as last set by SetOutPath) on the destination system.
  
  ; So, first, we set the current output path (the folder to which 
  ; files will be decompressed on the user's system) to '$1', 
  ; which is the path to their winlamp plugins folder.
  
  SetOutPath $1
  File "C:\program files\winlamp\plugins\${PLUGIN_DLL}"
  
  SetOutPath $1\Milkdrop2\config
  File "C:\program files\winlamp\plugins\milkdrop2\config\milk_msg.ini"
  File "C:\program files\winlamp\plugins\milkdrop2\config\milk_img.ini"
  ;File "C:\program files\winlamp\plugins\milkdrop2\config\milkdrop.ini"  ;this one will be generated - do not install

  SetOutPath $1\Milkdrop2\data
  File "C:\program files\winlamp\plugins\milkdrop2\data\vms_desktop.dll"
  File "C:\program files\winlamp\plugins\milkdrop2\data\comp_ps.fx"
  File "C:\program files\winlamp\plugins\milkdrop2\data\comp_vs.fx"
  File "C:\program files\winlamp\plugins\milkdrop2\data\warp_ps.fx"
  File "C:\program files\winlamp\plugins\milkdrop2\data\warp_vs.fx"
  File "C:\program files\winlamp\plugins\milkdrop2\data\include.fx"

  SetOutPath $1\Milkdrop2\docs
  File "C:\program files\winlamp\plugins\milkdrop2\docs\milkdrop.html"
  File "C:\program files\winlamp\plugins\milkdrop2\docs\milkdrop_preset_authoring.html"
  File "C:\program files\winlamp\plugins\milkdrop2\docs\q_and_t_vars.gif"

  SetOutPath $1\Milkdrop2\textures
  File "C:\program files\winlamp\plugins\milkdrop2\textures\*.*"
  
  SetOutPath $1\Milkdrop2\presets
  File "C:\program files\winlamp\plugins\milkdrop2\presets\*.milk"
  
  hmmm
  SetOutPath $1\MilkDrop2\presets\3d
  File "C:\program files\winlamp\plugins\milkdrop2\presets\3d\*.milk"
  
  hmmm
  SetOutPath $1\MilkDrop2\presets\inverted
  File "C:\program files\winlamp\plugins\milkdrop2\presets\inverted\*.milk"
  
  ; ----------------------- END SECTION [2/3] -----------------------
  ; ----------------------- END SECTION [2/3] -----------------------
  ; ----------------------- END SECTION [2/3] -----------------------
  

  ; now time to create the Uninstaller:

  IfFileExists $INSTDIR\WinLAMP.exe UninstWinLAMP2
  IfFileExists $INSTDIR\WinLAMP3.exe UninstWinLAMP3
  IfFileExists $INSTDIR\Studio.exe UninstWinLAMP3  ; legacy check
  goto UninstDone

  UninstWinLAMP3:
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}WinLAMP3 "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}WinLAMP3 "Install_Plugins_Dir" $1
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}WinLAMP3" "DisplayName" "${PLUGIN_NAME} for WinLAMP 3 (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}WinLAMP3" "UninstallString" '"$INSTDIR\uninst-${PLUGIN_DLL}.exe"'
    WriteUninstaller "$INSTDIR\uninst-${PLUGIN_DLL}.exe"
    CreateDirectory "$SMPROGRAMS\WinLAMP3\Vis Plugin Uninstallers"
    CreateShortCut "$SMPROGRAMS\WinLAMP3\Vis Plugin Uninstallers\Uninstall ${PLUGIN_NAME}.lnk" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" "" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" 0
    goto UninstDone
  
  UninstWinLAMP2:
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}WinLAMP "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}WinLAMP "Install_Plugins_Dir" $1
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}WinLAMP" "DisplayName" "${PLUGIN_NAME} for WinLAMP 2x (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}WinLAMP" "UninstallString" '"$INSTDIR\uninst-${PLUGIN_DLL}.exe"'
    WriteUninstaller "$INSTDIR\uninst-${PLUGIN_DLL}.exe"
    CreateDirectory "$SMPROGRAMS\WinLAMP\Vis Plugin Uninstallers"
    CreateShortCut "$SMPROGRAMS\WinLAMP\Vis Plugin Uninstallers\Uninstall ${PLUGIN_NAME}.lnk" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" "" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" 0
    goto UninstDone
    
  UninstDone:

SectionEnd

;----------------------------------------------------------------------

UninstallText "This will uninstall the ${PLUGIN_NAME} plugin.  Hit next to continue."
ShowUninstDetails Show

Section "Uninstall"

  ; This section is the code that will be run when the user goes
  ; to Uninstall the plugin.

  IfFileExists $INSTDIR\WinLAMP.exe UninstStep1WinLAMP2
  IfFileExists $INSTDIR\WinLAMP3.exe UninstStep1WinLAMP3
  IfFileExists $INSTDIR\Studio.exe UninstStep1WinLAMP3  ; legacy check
  goto UninstScriptDone

  UninstStep1WinLAMP3:
    ReadRegStr $1 HKLM SOFTWARE\${PLUGIN_DLL}WinLAMP3 "Install_Plugins_Dir"
    goto UninstStep2
  UninstStep1WinLAMP2:
    ReadRegStr $1 HKLM SOFTWARE\${PLUGIN_DLL}WinLAMP "Install_Plugins_Dir"
    goto UninstStep2

  UninstStep2:
  
    ; -------------------- EDIT THIS SECTION [3/3] --------------------
    ; -------------------- EDIT THIS SECTION [3/3] --------------------
    ; -------------------- EDIT THIS SECTION [3/3] --------------------

    ; LIST OF FILES TO DELETE WHEN USER RUNS THE UNINSTALL

    Delete "$1\${PLUGIN_DLL}"
    ;Delete "$1\vms_desktop.dll"     ** DO NOT DELETE! **
    Delete "$1\milkdrop.html"
    Delete "$1\milkdrop_preset_authoring.html"
    Delete "$1\q_and_t_vars.gif"

    MessageBox MB_YESNO|MB_ICONQUESTION \
            "Clear your saved settings?:$\r     $1\milkdrop_config.ini$\r     $1\milk_msg.ini$\r     $1\milk_img.ini" \
            IDNO SaveSettings

    Delete "$1\milkdrop_config.ini"
    Delete "$1\milk_msg.ini"
    Delete "$1\milk_img.ini"

    SaveSettings:

    MessageBox MB_YESNO|MB_ICONQUESTION \
            "Delete all presets in these 3 directories?:$\r     $1\milkdrop$\r     $1\milkdrop\3d$\r     $1\milkdrop\inverted" \
            IDNO SavePresets
            
    Delete "$1\milkdrop2\inverted\*.milk"
    Delete "$1\milkdrop2\3d\*.milk"
    Delete "$1\milkdrop2\*.milk"
    RMDir  "$1\milkdrop2\inverted"
    RMDir  "$1\milkdrop2\3d"
    RMDir  "$1\milkdrop2"
    
    SavePresets:
      
    ; ----------------------- END SECTION [3/3] -----------------------
    ; ----------------------- END SECTION [3/3] -----------------------
    ; ----------------------- END SECTION [3/3] -----------------------
  
  IfFileExists $INSTDIR\WinLAMP.exe UninstStep3WinLAMP2
  IfFileExists $INSTDIR\WinLAMP3.exe UninstStep3WinLAMP3
  IfFileExists $INSTDIR\Studio.exe UninstStep3WinLAMP3  ; legacy check
  goto UninstScriptDone
    
  UninstStep3WinLAMP3:
    DeleteRegKey HKLM SOFTWARE\${PLUGIN_DLL}WinLAMP3
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}WinLAMP3"
    Delete $INSTDIR\uninst-${PLUGIN_DLL}.exe
    Delete "$SMPROGRAMS\WinLAMP3\Vis Plugin Uninstallers\Uninstall ${PLUGIN_NAME}.lnk"
    RMDir "$SMPROGRAMS\WinLAMP3\Vis Plugin Uninstallers"
    goto UninstScriptDone
  UninstStep3WinLAMP2:
    DeleteRegKey HKLM SOFTWARE\${PLUGIN_DLL}WinLAMP
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}WinLAMP"
    Delete $INSTDIR\uninst-${PLUGIN_DLL}.exe
    Delete "$SMPROGRAMS\WinLAMP\Vis Plugin Uninstallers\Uninstall ${PLUGIN_NAME}.lnk"
    RMDir "$SMPROGRAMS\WinLAMP\Vis Plugin Uninstallers"
    goto UninstScriptDone

  UninstScriptDone:
SectionEnd

;----------------------------------------------------------------------

Function .onInstSuccess
  IfFileExists $INSTDIR\WinLAMP.exe CaseWinLAMP2
  IfFileExists $INSTDIR\WinLAMP3.exe CaseWinLAMP3
  IfFileExists $INSTDIR\Studio.exe CaseWinLAMP3  ; legacy check
  goto CaseImpossible

  CaseWinLAMP3:
    IfFileExists $INSTDIR\wacs\classicvis.wac ClassicVisOk
    ; no classicvis -> give them instructions
    MessageBox MB_YESNO|MB_ICONEXCLAMATION \
            "IMPORTANT: You must download and install the Classic Visualization$\rComponent before ${PLUGIN_NAME} will work with WinLAMP 3.$\r$\rWould you like to download it now?" \
            IDNO FailNoClassicVis
    ExecShell "open" "http://www.winlamp.com/components3/detail.jhtml?componentId=122130"
    MessageBox MB_OK|MB_ICONINFORMATION \
            "Your web browser will now open and allow you to download$\rthe Classic Visualization Component.  Please download$\rand install it.$\r$\rOnce it is installed, open WinLAMP 3 and hit CTRL+P$\rto open the Preferences screen.  Then, on the left,$\rscroll to 'Classic Visualizations' and select it.  From there,$\ryou can easily select, configure and run any plugins$\rinstalled to WinLAMP 2 or 3."
    goto end            
  FailNoClassicVis:
    MessageBox MB_OK|MB_ICONSTOP "Installation failed."
    goto end            
  ClassicVisOk:
    FindWindow $R0 "STUDIO"
    StrCmp $R0 0 "" DoneWinLAMP3
    MessageBox MB_YESNO|MB_ICONQUESTION \
            "${PLUGIN_NAME} was installed successfully.$\rWould you like to run WinLAMP 3 now?" \
            IDNO DoneWinLAMP3
    ; to do here: update 'oldvisname' string in the *xml* file winlamp3\studio.xnf,
    ; and set 'oldvisidx' to "0"
    IfFileExists $INSTDIR\WinLAMP3.exe CaseWinLAMP3b
      Exec '"$INSTDIR\studio.exe"'
      goto DoneWinLAMP3
    CaseWinLAMP3b:
      Exec '"$INSTDIR\winlamp3.exe"'
      goto DoneWinLAMP3
  DoneWinLAMP3:
    MessageBox MB_OK "While in WinLAMP 3, press CTRL+P to bring up the$\rPreferences screen, then scroll down and select$\rthe 'Classic Visualizations' option.  From there,$\ryou can select, configure, and run the ${PLUGIN_NAME} plugin."
    goto end
      
  CaseWinLAMP2:
    ; note: winlamp 2 should already be closed at this point.
    WriteINIStr "$INSTDIR\WinLAMP.ini" "WinLAMP" "visplugin_name" ${PLUGIN_DLL}
    WriteINIStr "$INSTDIR\WinLAMP.ini" "WinLAMP" "visplugin_num" "0"
    MessageBox MB_YESNO|MB_ICONQUESTION \
            "${PLUGIN_NAME} was installed successfully.$\r$\rWhile in WinLAMP, press ALT+K to configure it (optional);$\rpress CTRL+SHIFT+K to execute it.$\r$\rWould you like to run WinLAMP now?" \
            IDNO end
        Exec '"$INSTDIR\WinLAMP.exe"'
    Goto end
  
  CaseImpossible:  
    MessageBox MB_OK|MB_ICONEXCLAMATION "ERROR: unable to find winlamp.exe (winlamp2) or studio.exe/winlamp3.exe (winlamp3) in the install directory..."
    Goto end
    
  End:
FunctionEnd

; eof

