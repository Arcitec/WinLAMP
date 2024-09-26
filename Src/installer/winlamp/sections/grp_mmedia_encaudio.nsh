!ifndef LITE
SectionGroup $(IDS_GRP_MMEDIA_AUDIO_ENC) IDX_GRP_MMEDIA_AUDIO_ENC ; Audio Encoders
!ifndef WINLAMP64
!ifdef FULL
	${WinLAMPSection} "winlampWmaEncoder" $(secWMAE) IDX_SEC_WMA_ENC
		${SECTIONIN_FULL}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\enc_wma.dll
	${WinLAMPSectionEnd}
!endif ; FULL
!endif ; WINLAMP64

!ifndef WINLAMP64
!ifdef FULL
	${WinLAMPSection} "winlampWavEncoder" $(IDS_SEC_WAV_ENC) IDX_SEC_WAV_ENC
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\enc_wav.dll
	${WinLAMPSectionEnd}
!endif ; FULL
!endif ; WINLAMP64

!ifndef WINLAMP64
!ifdef STD | FULL
	${WinLAMPSection} "winlampMp3Encoder" $(secMP3E) IDX_SEC_MP3_ENC
		${SECTIONIN_STD}
;		SetOutPath $INSTDIR\Shared
;		File ..\..\resources\libraries\lame_enc.dll  ; 5.9 - lame_enc now statically-linked, not a shared dynamic dll
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\enc_lame.dll
	${WinLAMPSectionEnd}
!endif ;  FULL
!endif ; WINLAMP64

; need to fix this section at some point, will probably need to remove it....
!if 0
!ifndef WINLAMP64
!ifdef FULL
	${WinLAMPSection} "encoderAac" $(secAACE) IDX_SEC_AAC_ENC
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		Delete $INSTDIR\Plugins\enc_aac.dll
		Delete $INSTDIR\Plugins\enc_mp4.dll
		File /nonfatal ${FILES_PATH}\Plugins\enc_fhgaac.dll ; missing, as this can no longer be shipped
		SetOutPath $INSTDIR\Shared
		File ${FILES_PATH}\Shared\libmp4v2.dll
	${WinLAMPSectionEnd}
!endif ; FULL
!endif ; WINLAMP64
!endif

!ifndef WINLAMP64
!ifdef FULL
	${WinLAMPSection} "encoderFlac" $(IDS_SEC_FLAC_ENC) IDX_SEC_FLAC_ENC
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\enc_flac.dll
;		SetOutPath $INSTDIR\Shared
;		File ${FILES_PATH}\Shared\libFLAC.dll ; 5.9 - libflac now statically-linked, not a shared dynamic dll
	${WinLAMPSectionEnd}
!endif ; FULL
!endif ; WINLAMP64

!ifndef WINLAMP64
!ifdef FULL
	${WinLAMPSection} "encoderOgg" $(secOGGEnc) IDX_SEC_OGG_ENC
		${SECTIONIN_STD}
		SetOutPath $INSTDIR\Plugins
		File ${FILES_PATH}\Plugins\enc_vorbis.dll
	${WinLAMPSectionEnd}
!endif ; FULL
!endif ; WINLAMP64

SectionGroupEnd ;  Audio Encoders
!endif ; NOT LITE