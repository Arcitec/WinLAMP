SectionGroup $(IDS_GRP_MMEDIA_AUDIO_DEC) IDX_GRP_MMEDIA_AUDIO_DEC ; Audio Playback
	${WinLAMPSection} "decoderMp3" $(IDS_SEC_MP3_DEC) IDX_SEC_MP3_DEC
		SectionIn 1 2 3 4 5 6 7 8 RO
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_mp3.dll
		SetOutPath "$INSTDIR\System"
		File "${FILES_PATH}\System\mp3.w5s"
		; File /nonfatal "${FILES_PATH}\System\mp3.wbm"
!ifndef WINLAMP64
		File "${FILES_PATH}\System\vlb.w5s"
;		File /nonfatal "${FILES_PATH}\System\vlb.wbm"
!endif
		SetOutPath "$INSTDIR\Shared"
		File "${FILES_PATH}\Shared\jnetlib.dll"
		; File "${FILES_PATH}\Shared\libmpg123.dll" ; v5.9 - libmpg123 now statically-linked, not a shared dynamic dll

		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\EventHandlers\PlayMusicFilesOnArrival" "${WINLAMP}PlayMediaOnArrival" ""
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\Handlers\${WINLAMP}PlayMediaOnArrival" "Action" "$(AutoplayHandler)"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\Handlers\${WINLAMP}PlayMediaOnArrival" "DefaultIcon" "$INSTDIR\${WINLAMPEXE},0"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\Handlers\${WINLAMP}PlayMediaOnArrival" "InvokeProgid" "${WINLAMP}.File"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\Handlers\${WINLAMP}PlayMediaOnArrival" "InvokeVerb" "Play"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\Handlers\${WINLAMP}PlayMediaOnArrival" "Provider" "${WINLAMP}"
	${WinLAMPSectionEnd}

	${WinLAMPSection} "decoderWma" $(secWMA) IDX_SEC_WMA_DEC
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_wm.dll
	${WinLAMPSectionEnd}

!ifndef WINLAMP64
	${WinLAMPSection} "decoderMidi" $(secMIDI) IDX_SEC_MIDI_DEC
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		Delete $INSTDIR\Plugins\in_dm.dll
		Delete $INSTDIR\Plugins\in_midi_dm.dll
		File ${FILES_PATH}\Plugins\in_midi.dll
		SetOutPath $INSTDIR\Shared
	${WinLAMPSectionEnd}
!endif ;WINLAMP64

!ifndef WINLAMP64
	${WinLAMPSection} "decoderMod" $(secMOD) IDX_SEC_MOD_DEC
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_mod.dll
		; SetOutPath "$INSTDIR\Microsoft.VC90.CRT" ; vc9 runtime no longer required
		; File ..\..\resources\libraries\msvcp90.dll
	${WinLAMPSectionEnd}
!endif ;WINLAMP64

!ifndef WINLAMP64
	${WinLAMPSection} "decoderOgg" $(secOGGPlay) IDX_SEC_OGG_DEC
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_vorbis.dll
	${WinLAMPSectionEnd}
!endif ; WINLAMP64

	${WinLAMPSection} "decoderMp4" $(secMP4E) IDX_SEC_MP4_DEC
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_mp4.dll
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\libmp4v2.dll
		SetOutPath $INSTDIR\System
		File ${FILES_PATH}\System\alac.w5s
		;File /nonfatal ${FILES_PATH}\System\alac.wbm
	${WinLAMPSectionEnd}

	${WinLAMPSection} "decoderFlac" $(IDS_SEC_FLAC_DEC) IDX_SEC_FLAC_DEC
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Shared
		; File ${FILES_PATH}\Shared\libFLAC_dynamic.dll ; 5.9 - libflac now statically-linked, not a shared dynamic dll
		File ${FILES_PATH}\Shared\nxlite.dll
		File ${FILES_PATH}\Shared\jnetlib.dll
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_flac.dll
	${WinLAMPSectionEnd}

	${WinLAMPSection} "decoderCdda" $(secCDDA) IDX_SEC_CDDA_DEC
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\nde.dll
		File ${FILES_PATH}\Shared\nxlite.dll
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_cdda.dll
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Explorer\AutoplayHandlers\EventHandlers\PlayCDAudioOnArrival" "${WINLAMP}PlayMediaOnArrival" ""
	${WinLAMPSectionEnd}

	${WinLAMPSection} "decoderWav" $(secWAV) IDX_SEC_WAV_DEC
		${SECTIONIN_LITE}
		!ifdef old_in_wave_plugin
		SetOutPath $INSTDIR\Plugins
		File ..\..\resources\Plugins\in_wave.dll
		!else
		; SetOutPath $INSTDIR\Shared
		; File ${FILES_PATH}\Shared\libsndfile.dll ; 5.9 - libsndfile now statically-linked, not a shared dynamic dll
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_wave.dll
		!endif ; old_in_wave_plugin
	${WinLAMPSectionEnd}
SectionGroupEnd ;  Audio Playback