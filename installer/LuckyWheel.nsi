Unicode true
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"
!ifndef PAYLOAD
  !error "Define PAYLOAD with the deployed Release directory"
!endif
!ifndef OUTPUT
  !error "Define OUTPUT with the installer output path"
!endif
!ifndef VERSION
  !error "Define VERSION from CMake generated version.txt"
!endif
!ifdef VALIDATION
  !define APPKEY "LuckyWheelValidation"
  !define SHORTCUTDIR "LuckyWheel Validation"
!else
  !define APPKEY "LuckyWheel"
  !define SHORTCUTDIR "LuckyWheel"
!endif
Name "LuckyWheel ${VERSION}"
OutFile "${OUTPUT}"
InstallDir "$LOCALAPPDATA\Programs\LuckyWheel"
InstallDirRegKey HKCU "Software\${APPKEY}" "InstallDir"
RequestExecutionLevel user
SetCompressor /SOLID lzma
VIProductVersion "${VERSION}.0"
VIAddVersionKey /LANG=1033 "ProductName" "LuckyWheel"
VIAddVersionKey /LANG=1033 "FileDescription" "LuckyWheel Windows x64 Installer"
VIAddVersionKey /LANG=1033 "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=1033 "ProductVersion" "${VERSION}"
VIAddVersionKey /LANG=1033 "CompanyName" "kaido-MSVC"
VIAddVersionKey /LANG=1033 "LegalCopyright" "Copyright (c) 2026 kaido-MSVC. MIT License."
!define MUI_ICON "..\assets\luckywheel.ico"
!define MUI_UNICON "..\assets\luckywheel.ico"
!define MUI_ABORTWARNING
!insertmacro MUI_PAGE_WELCOME
!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "${PAYLOAD}\licenses\END-USER-TERMS.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\LuckyWheel.exe"
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "TradChinese"
!insertmacro MUI_LANGUAGE "English"

Function .onInit
  SetShellVarContext current
  ; Silent mode cannot skip the license page without explicit acceptance.
  IfSilent 0 terms_ready
  ${GetParameters} $0
  ${GetOptions} $0 "/ACCEPTMS=" $2
  ${If} $2 != "1"
    SetErrorLevel 4
    Abort
  ${EndIf}
  terms_ready:
  ${GetParameters} $0
  ${GetOptions} $0 "/DESKTOP=" $1
  ${If} $1 == "1"
    SectionSetFlags 1 1 ; optional desktop component is the second install section
  ${EndIf}
FunctionEnd
Function un.onInit
  SetShellVarContext current
FunctionEnd

Section "LuckyWheel 程式與開始選單 / Application and Start menu" SEC_APP
  SectionIn RO
  IfFileExists "$INSTDIR\LuckyWheel.exe" 0 ready
  System::Call 'kernel32::CreateFileW(w "$INSTDIR\LuckyWheel.exe", i 0x40000000, i 0, p 0, i 3, i 0, p 0) p.r0'
  ${If} $0 == -1
    MessageBox MB_OK|MB_ICONSTOP "請先關閉 LuckyWheel，再進行升級。 / Close LuckyWheel before upgrading." /SD IDOK
    SetErrorLevel 2
    Abort
  ${EndIf}
  System::Call 'kernel32::CloseHandle(p r0)'
  ready:
  SetOutPath "$INSTDIR"
  SetOverwrite on
  ClearErrors
  File /r "${PAYLOAD}\*.*"
  ${If} ${Errors}
    MessageBox MB_OK|MB_ICONSTOP "安裝失敗，請先關閉 LuckyWheel 再重試。 / Close LuckyWheel and retry." /SD IDOK
    SetErrorLevel 1
    Abort
  ${EndIf}
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  WriteRegStr HKCU "Software\${APPKEY}" "InstallDir" "$INSTDIR"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "DisplayName" "LuckyWheel"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "Publisher" "kaido-MSVC"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "DisplayIcon" "$INSTDIR\LuckyWheel.exe,0"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "UninstallString" '$\"$INSTDIR\Uninstall.exe$\"'
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "NoRepair" 1
  CreateDirectory "$SMPROGRAMS\${SHORTCUTDIR}"
  CreateShortcut "$SMPROGRAMS\${SHORTCUTDIR}\LuckyWheel.lnk" "$INSTDIR\LuckyWheel.exe"
  CreateShortcut "$SMPROGRAMS\${SHORTCUTDIR}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
SectionEnd

Section /o "桌面捷徑 / Desktop shortcut" SEC_DESKTOP
  CreateShortcut "$DESKTOP\${SHORTCUTDIR}.lnk" "$INSTDIR\LuckyWheel.exe"
SectionEnd

Section "Uninstall"
  IfFileExists "$INSTDIR\LuckyWheel.exe" 0 unready
  System::Call 'kernel32::CreateFileW(w "$INSTDIR\LuckyWheel.exe", i 0x40000000, i 0, p 0, i 3, i 0, p 0) p.r0'
  ${If} $0 == -1
    MessageBox MB_OK|MB_ICONSTOP "請先關閉 LuckyWheel，再解除安裝。 / Close LuckyWheel before uninstalling." /SD IDOK
    SetErrorLevel 2
    Abort
  ${EndIf}
  System::Call 'kernel32::CloseHandle(p r0)'
  unready:
  ; Delete only packaged files. Never recurse through the install directory or AppData.
  !include "${REMOVE_MANIFEST}"
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"
  Delete "$SMPROGRAMS\${SHORTCUTDIR}\LuckyWheel.lnk"
  Delete "$SMPROGRAMS\${SHORTCUTDIR}\Uninstall.lnk"
  RMDir "$SMPROGRAMS\${SHORTCUTDIR}"
  Delete "$DESKTOP\${SHORTCUTDIR}.lnk"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}"
  DeleteRegKey HKCU "Software\${APPKEY}"
SectionEnd
