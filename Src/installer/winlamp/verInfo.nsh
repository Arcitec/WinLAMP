!ifndef _DEBUG

!include "buildConstants.nsh"

VIProductVersion "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MINOR_SECOND_SHORT}.${BUILD_NUM}"
VIAddVersionKey "ProductName" "${WINLAMP} Installer"
VIAddVersionKey "Comments" "Visit http://www.winlamp.com/ for updates."
VIAddVersionKey "CompanyName" "WinLAMP SA"
VIAddVersionKey "LegalTrademarks" "Nullsoft and WinLAMP are trademarks of WinLAMP SA"
VIAddVersionKey "LegalCopyright" "Copyright © 1997-2023 WinLAMP SA"
VIAddVersionKey "FileDescription" "${WINLAMP} Installer"
VIAddVersionKey "FileVersion" "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MINOR_SECOND_SHORT}.${BUILD_NUM}"
VIAddVersionKey "ProductVersion" "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MINOR_SECOND_SHORT} Build ${BUILD_NUM}"
VIAddVersionKey "SpecialBuild" "${VERSION_ADDITIONALINFO}"

!ifndef InstallType
!define InstallType "Final"
!endif

!else ; _DEBUG

!define VERSION_MAJOR                "Debug"
!define VERSION_MINOR                ""
!define VERSION_MINOR_SECOND         ""
!define VERSION_MINOR_SECOND_SHORT   ""
!define BUILD_NUM                    ""
!define InstallType                  "Internal"

!endif ; _DEBUG