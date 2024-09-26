!ifndef NULLSOFT_WINLAMP_UNINSTALLER_COMPONENTS_HEADER
!define NULLSOFT_WINLAMP_UNINSTALLER_COMPONENTS_HEADER

!ifndef MUI_UNCONFIRMPAGE
	!define MUI_UNCONFIRMPAGE
!endif

!include ".\utils\skins.nsh"

SectionGroup "un.$(MUI_UNTEXT_CONFIRM_TITLE)" IDX_UNINSTALL_COMPONENTS_GROUP
	Section "-un.ComponentsInit" IDX_UNINSTALL_COMPONENTS_INIT
		StrCpy $SETTINGSDIR $INSTDIR
		StrCpy $PREVINSTINI "$INSTDIR\install.ini"
		StrCpy $INSTINI $PREVINSTINI
  
		${If} ${FileExists} "$INSTDIR\paths.ini"
			Push $SETTINGSDIR
			nsis_winlamp::ReadSettings /NOUNLOAD
			nsis_winlamp::GetIniPath
			Pop $SETTINGSDIR
		${EndIf}
	SectionEnd
	
	!include  ".\uninstall\player.nsh"
	!include  ".\uninstall\userData.nsh"
SectionGroupEnd

!endif ;NULLSOFT_WINLAMP_UNINSTALLER_COMPONENTS_HEADER