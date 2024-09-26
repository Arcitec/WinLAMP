SectionGroup $(IDS_GRP_MMEDIA_OUTPUT)  IDX_GRP_MMEDIA_OUTPUT ; Output
	${WinLAMPSection} "outputDirectSound" $(secDirectSound) IDX_SEC_OUT_DS
		SectionIn 1 2 3 4 5 6 7 8 RO
		SetOutPath $INSTDIR\Plugins
		File "${FILES_PATH}\Plugins\out_ds.dll"
	${WinLAMPSectionEnd}
	
	${WinLAMPSection} "outputWasapi" $(secWasapi) IDX_SEC_OUT_WASAPI
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		File "${FILES_PATH}\Plugins\out_wasapi.dll"
	${WinLAMPSectionEnd}
  
!ifndef WINLAMP64
	${WinLAMPSection} "outputDisk" $(secWriteWAV) IDX_SEC_OUT_DISK
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		File "${FILES_PATH}\Plugins\out_disk.dll"
	${WinLAMPSectionEnd}
!endif ; WINLAMP64

	${WinLAMPSection} "outputWav" $(IDS_SEC_OUT_WAV) IDX_SEC_OUT_WAV
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		File "${FILES_PATH}\Plugins\out_wave.dll"
	${WinLAMPSectionEnd}
SectionGroupEnd ; Output