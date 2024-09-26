; waplugin.nsi
;
; This script will generate an installer that installs a WinLAMP 2 plug-in.
;
; This installer will automatically alert the user that installation was
; successful, and ask them whether or not they would like to make the 
; plug-in the default and run WinLAMP.

;--------------------------------

;Header Files

!include "Sections.nsh"
!include "WinMessages.nsh"

; common defines for a generic dro installer :o)
!define VERSION "0.6"
!define ALT_VER "0_6"
!define PLUG "Album Art Viewer"
!define PLUG_ALT "Album_Art_Viewer"
!define PLUG_FILE "gen_classicart"

; use leet compression
SetCompressor lzma

; adds xp style support
XPStyle on

; The name of the installer
Name "${PLUG} v${VERSION}"

; The file to write
OutFile "${PLUG_ALT}_v${ALT_VER}.exe"

InstType "Plugin only"
InstType "Plugin + language file"
InstType /NOCUSTOM
InstType /COMPONENTSONLYONCUSTOM

; The default installation directory
InstallDir $PROGRAMFILES\WinLAMP
InstProgressFlags smooth

; detect winlamp path from uninstall string if available
InstallDirRegKey HKLM \
                 "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinLAMP" \
                 "UninstallString"

; The text to prompt the user to enter a directory
DirText "Please select your WinLAMP path below (you will be able to proceed when WinLAMP is detected):"
# currently doesn't work - DirShow hide

; automatically close the installer when done.
AutoCloseWindow true

; hide the "show details" box
ShowInstDetails show

;--------------------------------

;Pages

PageEx directory
Caption " "
PageExEnd
Page components
Page instfiles

;--------------------------------


; The stuff to install
Section ""
  SetOverwrite on
  SetOutPath "$INSTDIR\Plugins"
  ; File to extract
  File "x86_Release\${PLUG_FILE}.dll"
  SetOverwrite off
SectionEnd

Section "Example language file"
;  SectionSetFlags 0  SF_BOLD
  SectionIn 2

  SetOverwrite on
  SetOutPath "$INSTDIR\Plugins\${PLUG_FILE}"
  ; File to extract
  File "x86_Release\LangFiles\${PLUG_FILE}.lng"
  SetOverwrite off
SectionEnd

;--------------------------------

Function .onInit
  ;Detect running WinLAMP instances and close them
  !define WINLAMP_FILE_EXIT 40001

  FindWindow $R0 "WinLAMP v1.x"
  IntCmp $R0 0 ok
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "Please close all instances of WinLAMP before installing$\n\
					    ${PLUG} v${VERSION}. Attempt to close WinLAMP now?" IDYES checkagain IDNO no
    checkagain:
      FindWindow $R0 "WinLAMP v1.x"
      IntCmp $R0 0 ok
      SendMessage $R0 ${WM_COMMAND} ${WINLAMP_FILE_EXIT} 0
      Goto checkagain
    no:
       Abort
  ok:
FunctionEnd

Function .onInstSuccess
  MessageBox MB_YESNO \
             '${PLUG} was installed. Do you want to run WinLAMP now?' \
	 IDNO end
    ExecShell open "$INSTDIR\WinLAMP.exe"
  end:
FunctionEnd

Function .onVerifyInstDir
  ;Check for WinLAMP installation
  IfFileExists $INSTDIR\WinLAMP.exe Good
    Abort
  Good:
FunctionEnd