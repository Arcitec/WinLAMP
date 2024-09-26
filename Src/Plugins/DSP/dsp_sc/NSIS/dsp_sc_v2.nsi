;--------------------------------
;Include Modern UI

!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "WinVer.nsh"
;--------------------------------
; this is the version for WinLAMP 5.9.1
!define MINIMAL_VERSION "5.9.1.10021"

; The name of the installer
!define NAME "Shoutcast Source DSP Plug-in"
!define VERSION "2.4.2"
!define BUILD "449"
!define UNINSTALL "Shoutcast Source DSP"
!define UNINSTALLER "uninstall_shoutcast-source-dsp-v2.exe"
Name "${NAME}"

BrandingText "${NAME} v${VERSION} Build ${BUILD}"

; detect winlamp path from uninstall string if available
InstallDirRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinLAMP" "UninstallString"

; The file to write
OutFile "shoutcast-dsp-2-4-2-windows.exe"

; The default installation directory
InstallDir "$PROGRAMFILES32\WinLAMP"

; The text to prompt the user to enter a directory
DirText "Please select your WinLAMP path below (you will be able to proceed when WinLAMP is detected):"
# currently doesn't work - DirShow hide

; Request application privileges for Windows Vista+
RequestExecutionLevel admin

; Set the compressor (get installer as small as possible)
SetCompressor /SOLID lzma

; Set the install types available
InstType "Full Install"
InstType "Base Install"

; global variables
Var /GLOBAL WINLAMP_INI_DIR
Var GetInstalledSize.total

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages
  !define MUI_ICON "modern-install.ico"
  !define MUI_UNICON "modern-install.ico"

  !define MUI_WELCOMEPAGE_TITLE_3LINES
  !define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of the ${NAME}.$\n$\nIt is recommended that you close all instances of WinLAMP before starting Setup. This will make it possible to install relevant files within your WinLAMP installation without issues.$\n$\nClick Next to continue."
  !define MUI_WELCOMEFINISHPAGE_BITMAP "win.bmp"
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "dsp_sc_license.txt"
; is best to call the version check when leaving the directory page so it will be working against correct path
  !define MUI_PAGE_CUSTOMFUNCTION_LEAVE CheckWinLAMPVersion
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_INSTFILES

  !define MUI_FINISHPAGE_RUN
  !define MUI_FINISHPAGE_RUN_FUNCTION SetAsCurrentDSP
  !define MUI_FINISHPAGE_RUN_TEXT "Set as the current DSP plug-in"

  !define MUI_FINISHPAGE_SHOWREADME ; "$INSTDIR\winlamp.exe"
  !define MUI_FINISHPAGE_SHOWREADME_FUNCTION RunWinLAMP
  !define MUI_FINISHPAGE_SHOWREADME_TEXT "Run WinLAMP"

  !define MUI_FINISHPAGE_TEXT_LARGE
  !define MUI_FINISHPAGE_TITLE_3LINES
  !define MUI_FINISHPAGE_TEXT "${NAME} has been installed.$\n$\nTo enable the plug-in if it is not your current DSP plug-in, goto WinLAMP Preferences -> Plug-ins -> DSP/Effect and select the '${NAME}' entry.$\n$\nClick Finish to close this wizard."
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW RestoreCheckedStates
  !define MUI_PAGE_CUSTOMFUNCTION_LEAVE SaveCheckedStates
  !insertmacro MUI_PAGE_FINISH

  !define MUI_UNWELCOMEFINISHPAGE_BITMAP "win.bmp"
  !define MUI_UNWELCOMEPAGE_TEXT "This wizard will guide you through the uninstallation of the ${NAME}.$\n$\nBefore starting the uninstalltion, make sure WinLAMP and the ${NAME} are not running.$\n$\nClick Next to continue."
  !define MUI_UNFINISHPAGE_TEXT "${NAME} has been uninstalled from your WinLAMP install.$\n$\nClick Finish to close this wizard."
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English" ;first language is the default language

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL
;---------------------------------

Section "Shoutcast Source DSP" Core
  SectionIn 1 2 RO

  SetOutPath "$INSTDIR\Plugins"

  File "dsp_sc.dll"

  ; look for lamedll.dll and remove as we now use lame_enc.dll
  IfFileExists "$INSTDIR\Plugins\lamedll.dll" 0 +2
    Delete "lamedll.dll"

  ; Let's upgrade to Lame 3.100.1
  SetOutPath "$INSTDIR\Shared"
  File "..\..\..\..\resources\libraries\lame_enc.dll"

; VC142 runtimes are required for Win7 & 8 installations
; Chances are these will already exist, but let's make sure anyway...
	${If} ${AtLeastWin7}
	${AndIf} ${AtMostWin8.1}
	SetOutPath "$INSTDIR\Microsoft.VC142.CRT"
	File ..\..\..\..\resources\libraries\msvcp140.dll
	File ..\..\..\..\resources\libraries\vcruntime140.dll
	File ..\..\..\..\resources\libraries\msvcp140_1.dll
	File ..\..\..\..\resources\libraries\msvcp140_2.dll
	File ..\..\..\..\resources\libraries\msvcp140_atomic_wait.dll
	File ..\..\..\..\resources\libraries\msvcp140_codecvt_ids.dll
	File ..\..\..\..\resources\libraries\vccorlib140.dll
	File ..\..\..\..\resources\libraries\concrt140.dll
	${EndIf}

 ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "DisplayName" "Shoutcast Source DSP Plug-in v2"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "UninstallString" "$\"$INSTDIR\${UNINSTALLER}$\""
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "NoRepair" 1
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "Publisher" "Radionomy SA"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "HelpLink" "http://forums.shoutcast.com/forumdisplay.php?f=140"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "InstallLocation" "$INSTDIR\Plugins"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "DisplayVersion" "${VERSION}.${BUILD}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "URLInfoAbout" "http://wiki.shoutcast.com/wiki/Source_DSP_Plug-in"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "URLUpdateInfo" "https://www.shoutcast.com"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "DisplayIcon" "$\"$INSTDIR\${UNINSTALLER}$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "QuietUninstallString" "$\"$INSTDIR\${UNINSTALLER}$\" /S"
  Call GetInstalledSize
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}" "EstimatedSize" "$GetInstalledSize.total"

  IfFileExists "$INSTDIR\Uninstallers\${UNINSTALLER}" 0 +2
    Delete "$INSTDIR\Uninstallers\${UNINSTALLER}"

  SetOutPath "$INSTDIR\"

  WriteUninstaller "$INSTDIR\${UNINSTALLER}"

SectionEnd

Section "Documentation" Docs
  SectionIn 1

SetOutPath "$INSTDIR\Plugins\Shoutcast Source DSP"
  File ..\docs\Source_DSP_Plug-in.html
  File ..\docs\Source_DSP_Plug-in_Config_Examples.html
  File ..\docs\Source_DSP_Changelog.html
SetOutPath "$INSTDIR\Plugins\Shoutcast Source DSP\res"
  File ..\docs\res\*.png
SectionEnd

;--------------------------------
;Section description text

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Core} "The ${NAME} file.$\n$\n(This is always required)"
  !insertmacro MUI_DESCRIPTION_TEXT ${Docs} "This will be put in 'Plugins\Shoutcast Source DSP' in the selected destination.$\n$\nThese will show you the features available in the plug-in as well as how to make it connect with the Shoutcast 2 tools."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${UNINSTALL}"
  Delete "$INSTDIR\Plugins\dsp_sc.dll"
  Delete "$INSTDIR\Plugins\lamedll.dll" ; we don't actually install this any more
  ; Delete "$INSTDIR\Shared\lame_enc.dll"
  Delete "$INSTDIR\Plugins\Shoutcast Source DSP\dsp_sc.txt"
  Delete "$INSTDIR\Plugins\Shoutcast Source DSP\dsp_sc_config.txt"
  Delete "$INSTDIR\Plugins\Shoutcast Source DSP\Source_DSP_Plug-in.html"
  Delete "$INSTDIR\Plugins\Shoutcast Source DSP\Source_DSP_Changelog.html"
  Delete "$INSTDIR\Plugins\Shoutcast Source DSP\Source_DSP_Plug-in_Config_Examples.html"
  Delete "$INSTDIR\Plugins\Shoutcast Source DSP\res\*.png"
  Delete "$INSTDIR\Plugins\Shoutcast Source DSP\res\docs.css"
  RMDir "$INSTDIR\Plugins\Shoutcast Source DSP\res"
  RMDir "$INSTDIR\Plugins\Shoutcast Source DSP"
  ; Delete "$INSTDIR\Microsoft.VC142.CRT\*.dll" ; not wise, because then WinLAMP won't run on Win7-Win8.1 - - - why did this line exist for the VC90 Runtime?
  Delete "$INSTDIR\${UNINSTALLER}"

SectionEnd

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

  ;Detect running WinLAMP instances and close them
  !define WINLAMP_FILE_EXIT 40001

  FindWindow $R0 "WinLAMP v1.x"
  IntCmp $R0 0 ok
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "Please close all instances of WinLAMP before installing$\n${NAME}.$\n$\nAttempt to close WinLAMP now?" IDYES checkagain IDNO no
    checkagain:
      FindWindow $R0 "WinLAMP v1.x"
      IntCmp $R0 0 ok
      SendMessage $R0 ${WM_COMMAND} ${WINLAMP_FILE_EXIT} 0
      Goto checkagain
    no:
       Abort
  ok:

FunctionEnd

Function .onVerifyInstDir

  ;Check for WinLAMP installation

  IfFileExists $INSTDIR\WinLAMP.exe Good
    Abort
  Good:

FunctionEnd

;Uninstaller Functions
/* Function un.GetParent

  Exch $R0
  Push $R1
  Push $R2
  Push $R3

  StrCpy $R1 0
  StrLen $R2 $R0

  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
  Goto loop

  get:
    StrCpy $R0 $R0 -$R1
 
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0

FunctionEnd

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE

  Push "$INSTDIR"
  Call un.GetParent
  Pop $R0
  StrCpy $INSTDIR $R0

FunctionEnd */

Function GetWinLAMPIniPath
  StrCpy $WINLAMP_INI_DIR $INSTDIR
  ${If} $0 == ""
    StrCpy $WINLAMP_INI_DIR "$PROGRAMFILES\WinLAMP"
  ${EndIf}
  ClearErrors

  ${If} ${FileExists} "$WINLAMP_INI_DIR\paths.ini"
    ReadINIStr $0 "$WINLAMP_INI_DIR\paths.ini" "WinLAMP" "inidir"
    ${If} $0 != ""
      ${WordFind2X} $0 "{" "}" "E+1" $2
      ${If} ${Errors}
        ${IfNot} ${FileExists} "$0\*.*"
          ${WordFind2X} $0 "%" "%" "E+1" $2

          ${If} $2 == "WINLAMP_ROOT_DIR"
            ClearErrors
            ${GetRoot} "$WINLAMP_INI_DIR" $3
            ${WordReplace} "$0" "%$2%" "$3" "E+1" $R0
            ${If} ${Errors}
              Return
            ${Else}
              StrCpy $WINLAMP_INI_DIR $R0
            ${EndIf}
          ${ElseIf} $2 == "WINLAMP_PROGRAM_DIR"
            ClearErrors
            ${WordReplace} "$0" "%$2%" "$WINLAMP_INI_DIR" "E+1" $R0
            ${If} ${Errors}
              Return
            ${Else}
              StrCpy $WINLAMP_INI_DIR $R0
            ${EndIf}
          ${Else}
            ClearErrors
            ReadEnvStr $R0 "$2"
            ${If} $R0 != ""
              ${WordReplace} "$0" "%$2%" "$R0" "E+1" $R0
              ${If} ${Errors}
                Return
              ${Else}
                StrCpy $WINLAMP_INI_DIR $R0
              ${EndIf}
            ${Else}
              Return
            ${EndIf}
          ${EndIf}
        ${Else}
          StrCpy $WINLAMP_INI_DIR $0
        ${EndIf}
      ${Else}
        System::Call "shell32::SHGetSpecialFolderPath(i $HWNDPARENT, t .r4, i $2, i0) i .r3"
        ClearErrors
        ${WordReplace} "$0" "{$2}" "$4" "E+1" $R0
        ${If} ${Errors}
          Return
        ${Else}
          StrCpy $WINLAMP_INI_DIR $R0
        ${EndIf}
      ${EndIf}
    ${Else}
      Return
    ${EndIf}
  ${Else}
    Return
  ${EndIf}
FunctionEnd

; set as the current DSP
Function SetAsCurrentDSP
  WriteINIStr "$WINLAMP_INI_DIR\winlamp.ini" "winlamp" "dspplugin_name" "dsp_sc.dll"
  WriteINIStr "$WINLAMP_INI_DIR\winlamp.ini" "winlamp" "dspplugin_num" "0"
FunctionEnd

Function RunWinLAMP
  StrCpy $1 1
  File "/oname=$PLUGINSDIR\ShellDispatch.dll" "ShellDispatch.dll"
  ${If} ${FileExists} "$PLUGINSDIR\ShellDispatch.dll"
  ${AndIf} ${FileExists} "$INSTDIR\winlamp.exe"
    Push $0
    StrCpy $0 ""
    ClearErrors
    GetFullPathName /SHORT $0 "$PLUGINSDIR\ShellDispatch.dll"
    ${IfNot} ${Errors}
    ${AndIf} $0 != ""
      ExecWait 'rundll32.exe $0,RunDll_ShellExecute "open" "$INSTDIR\winlamp.exe"' $1
      ${If} ${Errors}
        StrCpy $1 1
      ${EndIf}
    ${EndIf}
    Pop $0
  ${EndIf}

  ${If} $1 != 0
    Exec "$INSTDIR\winlamp.exe"
  ${EndIf}
FunctionEnd

; restore the last checked states on the finish page
Function RestoreCheckedStates
  Call GetWinLAMPIniPath

  ReadINIStr $0 "$WINLAMP_INI_DIR\plugins\dsp_sc.ini" "installer" "cur"
  ${If} $0 == "0"
    ${NSD_Uncheck} $mui.FinishPage.Run
  ${EndIf}

  ReadINIStr $0 "$WINLAMP_INI_DIR\plugins\dsp_sc.ini" "installer" "run"
  ${If} $0 == "0"
    ${NSD_Uncheck} $mui.FinishPage.ShowReadme
  ${EndIf}
FunctionEnd

; save the last checked states on the finish page
Function SaveCheckedStates

  ${NSD_GetState} $mui.FinishPage.Run $0
  WriteINIStr "$WINLAMP_INI_DIR\plugins\dsp_sc.ini" "installer" "cur" $0

  ${NSD_GetState} $mui.FinishPage.ShowReadme $0
  WriteINIStr "$WINLAMP_INI_DIR\plugins\dsp_sc.ini" "installer" "run" $0

FunctionEnd

; the check version function
; only issue is that the strings won't be localised as i see the installer in the zip supports it
Function CheckWinLAMPVersion
  ${GetFileVersion} "$INSTDIR\winlamp.exe" $R0 ; Get WinLAMP.exe version information, $R0 = Actual Version
  ${if} $R0 != "" ; check if Version info is not empty
  ${VersionCompare} $R0 ${MINIMAL_VERSION} $R1 ; $R1 = Result $R1=0  Versions are equal, $R1=1  Version1 is newer, $R1=2  Version2 is newer
  ${if} $R1 == "2"
    MessageBox MB_OK "Warning: This plug-in requires at least WinLAMP v${MINIMAL_VERSION} or higher.$\nThe detected version of your WinLAMP install is: $R0$\n$\nThe Shoutcast Source plug-in may not function correctly with the$\n version of winlamp detected.Please update your WinLAMP client!!$\n"
    Abort
  ${EndIf}
  ${Else}
    MessageBox MB_OK "Warning: A valid WinLAMP install was not detected in the specified path.$\n$\nPlease check the WinLAMP directory and either install the latest version$\nfrom WinLAMP.com or choose another directory with a valid WinLAMP install$\nbefore you can install the Shoutcast Source on your machine."
    Abort
  ${EndIf}
FunctionEnd

; Return on top of stack the total size of the selected (installed) sections, formated as DWORD
; Assumes no more than 256 sections are defined
Function GetInstalledSize
	Push $0
	Push $1
	StrCpy $GetInstalledSize.total 0
	${ForEach} $1 0 256 + 1
		StrCpy $0 0
		${if} ${SectionIsSelected} $1
			SectionGetSize $1 $0
			IntOp $GetInstalledSize.total $GetInstalledSize.total + $0
		${Endif}
 
		; Error flag is set when an out-of-bound section is referenced
		${if} ${errors}
			${break}
		${Endif}
	${Next}
 
	ClearErrors
	Pop $1
	Pop $0
	IntFmt $GetInstalledSize.total "0x%08X" $GetInstalledSize.total
	Push $GetInstalledSize.total
FunctionEnd