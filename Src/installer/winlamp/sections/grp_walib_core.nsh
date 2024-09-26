SectionGroup $(IDS_GRP_WALIB_CORE) IDX_GRP_WALIB_CORE  ;  Core Media Library Components
	${WinLAMPSection} "mediaLibraryLocalMedia" $(sec_ML_LOCAL) IDX_SEC_ML_LOCAL                            ; >>> [Local Media]
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\nde.dll
		File ${FILES_PATH}\Shared\nxlite.dll
		File ${FILES_PATH}\Shared\jnetlib.dll
		SetOutPath $INSTDIR\System
		; File ${FILES_PATH}\System\dlmgr.w5s
		; File /nonfatal ${FILES_PATH}\System\dlmgr.wbm
		File "${FILES_PATH}\System\wac_downloadManager.w5s"
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_local.dll
		${If} ${FileExists} "$INSTDIR\Plugins\gen_pcfix.dll"
			Rename "$INSTDIR\Plugins\gen_pcfix.dll" "$INSTDIR\Plugins\gen_pcfix.dll.off" ; disable ShaneH's playcount tracking plugin
		${EndIf}
	${WinLAMPSectionEnd}                                                             ; <<< [Local Media]

	${WinLAMPSection} "mediaLibraryPlaylists" $(sec_ML_PLAYLISTS) IDX_SEC_ML_PLAYLISTS                   ; >>> [Playlists]
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_playlists.dll
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\nxlite.dll
	${WinLAMPSectionEnd}                                                            ; <<< [Playlists]
  
!ifndef WINLAMP64
!ifdef full
	${WinLAMPSection} "mediaLibraryRipBurn" $(sec_ML_DISC) IDX_SEC_ML_DISC                             ; >>> [CD Rip  & Burn]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_disc.dll
	${WinLAMPSectionEnd}                                                            ; <<< [CD Rip  & Burn]
!endif
!endif

!ifndef WINLAMP64
!ifdef full
	${WinLAMPSection} "mediaLibraryBookmarks" $(sec_ML_BOOKMARKS) IDX_SEC_ML_BOOKMARKS                   ; >>> [Bookmarks]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_bookmarks.dll
	${WinLAMPSectionEnd}                                                  ; <<< [Bookmarks]
!endif
!endif ;WINLAMP64

!ifndef WINLAMP64
!ifdef full
	${WinLAMPSection} "mediaLibraryHistory" $(sec_ML_HISTORY) IDX_SEC_ML_HISTORY                       ; >>> [History]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\nde.dll
		File ${FILES_PATH}\Shared\nxlite.dll
		File ${FILES_PATH}\Shared\jnetlib.dll
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_history.dll
	${WinLAMPSectionEnd}                                                           ; <<< [History]
!endif
!endif ;WINLAMP64

SectionGroupEnd ; Core Media Library Components