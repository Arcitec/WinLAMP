!ifdef std | full
SectionGroup $(IDS_GRP_WALIB) IDX_GRP_WALIB                            ;  WinLAMP Library
	${WinLAMPSection} "mediaLibrary" $(secML) IDX_SEC_ML              ; >>> [Media Library]
		${SECTIONIN_STD}
		SectionGetFlags ${IDX_GRP_WALIB} $1
		IntOp $1 $1 & 0x0041
		StrCmp $1 "0" done
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\gen_ml.dll

		done:
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\nxlite.dll
	${WinLAMPSectionEnd}                                                           ; <<< [Media Library]

	!include ".\sections\grp_walib_core.nsh"

	${WinLAMPSection} "mediaLibraryTranscode" $(sec_ML_TRANSCODE) IDX_SEC_ML_TRANSCODE  ; >>> [Trancsoding Tool]
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_transcode.dll
		${If} ${FileExists} "$SETTINGSDIR\Plugins\ml_transcode.ini"
			Rename "$SETTINGSDIR\Plugins\ml_transcode.ini" "$SETTINGSDIR\Plugins\ml\ml_transcode.ini"
		${EndIf}
	${WinLAMPSectionEnd}                                                          ; <<< [Trancsoding Tool]

	${WinLAMPSection} "mediaLibraryReplayGain" $(sec_ML_RG) IDX_SEC_ML_RG         ; >>> [Replay Gain Analysis Tool]
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_rg.dll
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\ReplayGainAnalysis.dll
	${WinLAMPSectionEnd}                                                          ; <<< [Replay Gain Analysis Tool]

!ifndef WINLAMP64
!ifdef full
	${WinLAMPSection} "mediaLibraryiTunesImp" $(sec_ML_IMPEX) IDX_SEC_ML_IMPEX                           ; >>> [iTunes Importer]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\System
		File ${FILES_PATH}\System\xml.w5s
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_impex.dll
	${WinLAMPSectionEnd}                                                          ; <<< [iTunes Importer]
!endif
!endif ; WINLAMP64

!if 0
	${WinLAMPSection} "mediaLibraryAutoTag" $(IDS_SEC_ML_AUTOTAG) IDX_SEC_ML_AUTOTAG                   ; >>> [Auto Tag]
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_autotag.dll
	${WinLAMPSectionEnd}                                                           ; <<< [Auto Tag]
!endif

!ifdef full
	${WinLAMPSection} "mediaLibraryPodcast" $(secWire) IDX_SEC_ML_WIRE                                  ; >>> [SHOUTCast Wire]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\nde.dll
		File ${FILES_PATH}\Shared\nxlite.dll
		File ${FILES_PATH}\Shared\jnetlib.dll
		SetOutPath $INSTDIR\System
;		File ${FILES_PATH}\System\jnetlib.w5s
;		File /nonfatal "${FILES_PATH}\System\jnetlib.wbm"
		File ${FILES_PATH}\System\wac_network.w5s
		File ${FILES_PATH}\System\xml.w5s
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_wire.dll
		;!warning "ml_downloads disabled to make 5.5.2 (1 of 2)"
		File ${FILES_PATH}\Plugins\ml_downloads.dll
		WriteRegStr HKEY_CLASSES_ROOT "pcast" "" "URL: Podcast Protocol"
		WriteRegStr HKEY_CLASSES_ROOT "pcast" "URL Protocol" ""
		WriteRegStr HKEY_CLASSES_ROOT "pcast\shell\open\command" "" "$INSTDIR\${WINLAMPEXE} /HANDLE %1"
		WriteRegStr HKEY_CLASSES_ROOT "feed" "" "URL: RSS Protocol"
		WriteRegStr HKEY_CLASSES_ROOT "feed" "URL Protocol" ""
		WriteRegStr HKEY_CLASSES_ROOT "feed\shell\open\command" "" "$INSTDIR\${WINLAMPEXE} /HANDLE %1"
		SetOutPath $INSTDIR\System
		File ${FILES_PATH}\System\omBrowser.w5s
		; File /nonfatal ${FILES_PATH}\System\omBrowser.wbm

		; Qt Components
		SetOutPath $INSTDIR
		File ${FILES_PATH}\Qt5Core.dll
		File ${FILES_PATH}\Qt5Network.dll

/*
		; Placeholder - code is currently not working
		; We need a method to rename the <winlamp:preferences xmlns:winlamp="http://www.winlamp.com" version="2"> node to just <winlamp>,
		; then make the "service url=" change, then rename the node back to the original...

		; Force using default dir for 5.9 by removing any alternate podcast dir user setting

		; make a backup first
		${If} ${FileExists} "$SETTINGSDIR\Plugins\ml\feeds\rss.xml"
		CreateDirectory "$SETTINGSDIR\Plugins\ml\feeds\backup"
		CopyFiles /SILENT "$SETTINGSDIR\Plugins\ml\feeds\rss.xml" "$SETTINGSDIR\Plugins\ml\feeds\backup\rss.xml"
		; backup made, now edit the service url

		nsisXML::create
		nsisXML::load "$SETTINGSDIR\Plugins\ml\feeds\rss.xml"
		  ${If} $0 P<> 0
		    nsisXML::select '/winlamp/service'
		    ;Diagnostics
		    ;MessageBox mb_ok "$1 should not be 0 here"
		    ${If} $1 P<> 0
		      nsisXML::setAttribute "url" ""
		      nsisXML::save "$SETTINGSDIR\Plugins\ml\feeds\rss.xml"
		    ${EndIf}
		    nsisXML::release $0
		  ${Else}
		    ;Diagnostics
		    ;MessageBox mb_ok "load failed"
		    nsisXML::release $1
		  ${EndIf}
		${EndIf}
*/
	${WinLAMPSectionEnd}                                                         ; <<< [SHOUTCast Wire]
!endif

!ifndef WINLAMP64
!ifdef full
	${WinLAMPSection} "mediaLibraryOnlineServices" $(secOM) IDX_SEC_ML_ONLINE                                   ; >>> [Online Media]
		${SECTIONIN_FULL}
		StrCpy $0 "$SETTINGSDIR\Plugins\ml"
		CreateDirectory "$0"

		Delete "$0\radio.*"
		Delete "$0\tv.*"
		Delete "$0\waaudio.*"
		Delete "$0\wamedia.*"
		Delete "$0\watv.*"
		Delete "$0\xmmedia.*"
		Delete "$0\ml_win_media.ini"

		SetOutPath "$INSTDIR\Plugins"
		File "${FILES_PATH}\Plugins\ml_online.dll"
		;!warning "ml_downloads disabled to make 5.5.2 (2 of 2)"
		File "${FILES_PATH}\Plugins\ml_downloads.dll"
 
		StrCpy $0 "$SETTINGSDIR\Plugins\ml\ml_online.ini"
		DeleteINIStr "$0" "Setup" "featuredExtra"
		DeleteINIStr "$0" "Navigation" "openOnce"
		DeleteINIStr "$0" "Navigation" "openOnceMode"

		SetOutPath "$INSTDIR\System"
		File /nonfatal "${FILES_PATH}\System\omBrowser.w5s"
		; File /nonfatal "${FILES_PATH}\System\omBrowser.wbm"
		File ${FILES_PATH}\System\wac_network.w5s
;		File ${FILES_PATH}\System\jnetlib.w5s
;		File /nonfatal "${FILES_PATH}\System\jnetlib.wbm"
	${WinLAMPSectionEnd}                                                             ; <<< [Online Media]
!endif
!endif

!ifndef WINLAMP64
!ifdef full
	${WinLAMPSection} "mediaLibraryNFT" $(sec_ML_NFT) IDX_SEC_ML_NFT                           ; >>> [NFT Library]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_nft.dll
	${WinLAMPSectionEnd}                                                          ; <<< [NFT Library]
!endif
!endif ; WINLAMP64

!ifndef WINLAMP64
!ifdef full
	${WinLAMPSection} "mediaLibraryFanzone" $(sec_ML_FANZONE) IDX_SEC_ML_FANZONE                           ; >>> [Fanzone]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_fanzone.dll
	${WinLAMPSectionEnd}                                                          ; <<< [Fanzone]
!endif
!endif ; WINLAMP64

!ifndef WINLAMP64
!ifdef full
	${WinLAMPSection} "mediaLibraryHotmix" $(sec_ML_HOTMIX) IDX_SEC_ML_HOTMIX                           ; >>> [HotmixRadio]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_hotmixradio.dll
	${WinLAMPSectionEnd}                                                          ; <<< [HotmixRadio]
!endif
!endif ; WINLAMP64

!if 0
!ifdef full
	${WinLAMPSection} "mediaLibraryPlaylistGenerator" $(SEC_ML_PLG) IDX_SEC_ML_PLG                         ; >>> [Playlist Generator]
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\ml_plg.dll
	${WinLAMPSectionEnd}                                                            ; <<< [Playlist Generator]
!endif
!endif

	!include ".\sections\grp_walib_pmp.nsh"

SectionGroupEnd
!endif  ;  FULL