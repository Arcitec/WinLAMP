SectionGroup $(IDS_GRP_MMEDIA) IDX_GRP_MMEDIA ; Multimedia Engine
	!include ".\sections\grp_mmedia_decaudio.nsh"
	!include ".\sections\grp_mmedia_decvideo.nsh"
	!include ".\sections\grp_mmedia_encaudio.nsh"
	!include ".\sections\grp_mmedia_output.nsh"

/* To be replaced by MusicBrainz
!ifndef WINLAMP64
	${WinLAMPSection} "cddb" $(secCDDB) IDX_SEC_CDDB                                          ; >>> [CDDB for recognizing CDs]
		${SECTIONIN_LITE}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\in_cdda.dll
	${WinLAMPSectionEnd}                                                   ; <<< [CDDB for recognizing CDs]
!endif ; WINLAMP64
*/
  
!ifndef WINLAMP64
!ifdef FULL
	${WinLAMPSection} "digitalSignalProcessing" $(secDSP) IDX_SEC_DSP                                       ; >>> [Signal Processor Studio Plug-in]
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		; File "..\..\resources\Plugins\dsp_sps.dll"
		File ${FILES_PATH}\Plugins\dsp_sps.dll
		DetailPrint "$(IDS_RUN_EXTRACT) $(IDS_DSP_PRESETS)..." ; Extracting presets...
		SetDetailsPrint none
		SetOutPath $INSTDIR\Plugins\DSP_SPS
		File "..\..\resources\data\dsp_sps\*.sps"
		SetOutPath $INSTDIR\Plugins
		SetDetailsPrint lastused
	${WinLAMPSectionEnd}                                                  ; <<< [Signal Processor Studio Plug-in]
!endif ; full
!endif ; WINLAMP64
SectionGroupEnd ; Multimedia Engine