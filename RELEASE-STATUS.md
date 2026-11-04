# LuckyWheel v2.0.9 發布狀態（2026-11-04）

正式下載與附件雜湊以 [GitHub Release v2.0.9](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.9) 及其 SHA256SUMS.txt 為準。
本版唯一應用程式來源為 Windows 本機 D:\projects\LuckyWheel\native-v2 已完成語言／UI 修正的原始碼，重新 MSVC x64 Release 建置與封裝；不沿用舊 EXE 或舊 dist。
舊 v2.0.8 tag 與附件保留。本版包含 Portable、Setup、自有來源、第三方對應來源及 SHA256 清單。

## 授權與來源交付

自有程式碼 MIT；Qt 動態連結 LGPLv3，FFmpeg LGPLv2.1 或更新版本，Qt 翻譯 BSD-3-Clause。Portable／Setup 各自內含完整第三方來源 ZIP、建置設定與來源證據。
DLL 必須與 licenses/source-provenance.json 中的既有來源雜湊相符。第三方庫未自行修改或升版。
Microsoft SDK／VS 元件原始條款、接收者同意流程與散布者持續義務保留，詳見 licenses/MICROSOFT-REDIST.md。接收者按同意不能代替發布者義務。
再散布時保留完整 sources、licenses 與接受流程；自製素材聲明仍由發布者負責。

## 驗證範圍

建置、必要測試與封裝核對結果見 RELEASE-NOTES-v2.0.9.md。
未實測切換 Windows 語言後重新登入、混合 DPI 多螢幕、乾淨 Windows 安裝／解除安裝、既有安裝原地升級及長時間壓力測試。
未數位簽章，未宣稱第三方函式庫可位元組相同重建。