Name "CDDB2 test, beta 1"

; The file to write
OutFile "cddb2.exe"

InstallDir $PROGRAMFILES\WinLAMP
InstallDirRegKey HKLM \
                 "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinLAMP" \
                 "UninstallString"

; The text to prompt the user to enter a directory
DirText "Please select your WinLAMP path below (you will be able to proceed when WinLAMP is detected):"
DirShow hide

; automatically close the installer when done.
AutoCloseWindow true
; hide the "show details" box
ShowInstDetails nevershow

BGGradient 000000 308030 FFFFFF
InstallColors FF8080 000000
InstProgressFlags smooth colored

Function .onInit
  MessageBox MB_YESNO|MB_ICONQUESTION "Install CDDB2 update test?" IDYES update
   MessageBox MB_OK|MB_ICONINFORMATION "Install aborted."
   Abort
  update:
FunctionEnd

Function .onVerifyInstDir
  IfFileExists $INSTDIR\WinLAMP.exe Good
    Abort
  Good:
FunctionEnd

Function CloseWinLAMP
  Push $0
  loop:
    FindWindow $0 "WinLAMP v1.x"
    IntCmp $0 0 done
     SendMessage $0 16 0 0
     StrCpy $9 "yes"
     Sleep 100
     Goto loop
  done:
  Pop $0
FunctionEnd


Section "ThisNameIsIgnoredSoWhyBother?"
  StrCpy $9 "no"
  Call CloseWinLAMP
  SetOutPath $INSTDIR
  File "C:\program files\winlamp\winlamp.exe"
  SetOutPath $INSTDIR\Plugins

  UnRegDll $OUTDIR\cddbcontrolwinlamp.dll
  UnRegDll $OUTDIR\cddbuiwinlamp.dll
  File "C:\program files\winlamp\plugins\in_cdda.dll"
  File "C:\program files\winlamp\plugins\in_mp3.dll"
  File "cddbcontrolwinlamp.dll"
  File "cddbuiwinlamp.dll"
  RegDll $OUTDIR\cddbcontrolwinlamp.dll
  RegDll $OUTDIR\cddbuiwinlamp.dll

  DetailPrint Completed.
SectionEnd


Function .onInstSuccess
  MessageBox MB_OK|MB_ICONINFORMATION "Update installed."
  StrCmp $9 "no" nope
    Exec '"$INSTDIR\WinLAMP.exe"'
  nope:
FunctionEnd

; eof
