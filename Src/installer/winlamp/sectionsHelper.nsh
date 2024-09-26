!ifndef WINLAMP_SECTIONS_HELPER_HEADER
!define WINLAMP_SECTIONS_HELPER_HEADER

; use WinLAMPSection/WinLAMPSectionEnd instead of Section/SectionEnd if you want section
; to store invariant name in install.ini
; Example:
; WinLAMPSection "winlampSection1" "$(IDS_WINLAMPSECTION1_TITLE)" IDX_WINLAMPSECTION1
;	....
;	....
; WinLAMPSectionEnd
;
;

!macro WinLAMPSection __sectionName __sectionTitle __sectionIndex
	!ifdef WINLAMP_SECTION_ACTIVE_ID | WINLAMP_SECTION_ACTIVE_NAME
		!error "Missing WinLAMPSectionEnd"
	!endif
	!define WINLAMP_SECTION_ACTIVE_ID		"${__sectionIndex}"
	!define WINLAMP_SECTION_ACTIVE_NAME		"${__sectionName}"
	Section "${__sectionTitle}" "${__sectionIndex}"
!macroend

!define WinLAMPSection "!insertmacro 'WinLAMPSection'"

!macro SECTIONHEADER_LINE __line
	!appendfile "${WINLAMP_SECTION_HEADER}" "${__line}$\r$\n"
!macroend

!define SECTIONHEADER_LINE	"!insertmacro 'SECTIONHEADER_LINE'"

!macro InitializeGetSectionName
	!ifdef WINLAMP_SECTION_HEADER
		!include "${WINLAMP_SECTION_HEADER}"
		!delfile "${WINLAMP_SECTION_HEADER}"
		SectionGetText $0 $0
		GetSectionName_FunctionEnd:
			Exch $0
		FunctionEnd
	!else
		!define GetSectionName SectionGetText
	!endif
!macroend

!define InitializeGetSectionName	"!insertmacro 'InitializeGetSectionName'"

!macro WinLAMPSectionEnd
	SectionEnd
!ifndef WINLAMP_SECTION_ACTIVE_ID | WINLAMP_SECTION_ACTIVE_NAME
	!error "Missing WinLAMPSection"
!endif

!ifndef WINLAMP_SECTION_HEADER
	!tempfile WINLAMP_SECTION_HEADER
	!delfile "${WINLAMP_SECTION_HEADER}"

	${SECTIONHEADER_LINE} "!define GetSectionName $\"!insertmacro 'GetSectionName'$\""
	${SECTIONHEADER_LINE} "!macro GetSectionName __sectionIndex __outputVar"
	${SECTIONHEADER_LINE} "$\tPush ${__sectionIndex}"
	${SECTIONHEADER_LINE} "$\tCall GetSectionName"
	${SECTIONHEADER_LINE} "$\tPop ${__outputVar}"
		${SECTIONHEADER_LINE} "!macroend"
	${SECTIONHEADER_LINE} "Function GetSectionName"
	${SECTIONHEADER_LINE} "$\tExch $0"
!endif

	${SECTIONHEADER_LINE} "$\tIntCmp $0	${${WINLAMP_SECTION_ACTIVE_ID}} 0 +3 +3"
	${SECTIONHEADER_LINE} "$\t$\tStrCpy $0	${WINLAMP_SECTION_ACTIVE_NAME}"
	${SECTIONHEADER_LINE} "$\t$\tGoto GetSectionName_FunctionEnd"

	!undef WINLAMP_SECTION_ACTIVE_ID
	!undef WINLAMP_SECTION_ACTIVE_NAME
!macroend

!define WinLAMPSectionEnd "!insertmacro 'WinLAMPSectionEnd'"
!endif