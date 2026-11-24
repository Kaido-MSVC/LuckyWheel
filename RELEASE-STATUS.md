# LuckyWheel 發布狀態

## v2.1.0 Windows x64 Portable（2026-11-24）

Portable 採用 Qt 官方 6.12.1 Windows x64 kit。QSQLITE 內嵌 SQLite 3.54.1，包含上游於 3.54.0 提供的安全修正。備份還原會拒絕無法有效表示歷史紀錄的數值欄位。 [SQLite 官方公告](https://www.sqlite.org/cves.html) · [Qt 內嵌 SQLite 版本](https://doc.qt.io/qt-6.12/qtsql-attribution-sqlite.html)

正式附件含 Windows x64 Portable ZIP、應用程式來源、第三方對應來源及 SHA-256 清單。發布者應履行 Microsoft 元件的再散布義務。 [正式發布頁](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.1.0) · [Microsoft 再散布說明](licenses/MICROSOFT-REDIST.md)

## v2.0.9 已發布版本（2026-11-04）

正式下載與附件雜湊以 [GitHub Release v2.0.9](https://github.com/Kaido-MSVC/LuckyWheel/releases/tag/v2.0.9) 及其 SHA256SUMS.txt 為準。
本版應用程式的語言與介面修正可在 [v2.0.9 Tag](https://github.com/Kaido-MSVC/LuckyWheel/tree/v2.0.9) 查閱，建置用的應用程式原始碼見 [來源 ZIP](https://github.com/Kaido-MSVC/LuckyWheel/releases/download/v2.0.9/LuckyWheel-2.0.9-source.zip)。本版由 MSVC x64 Release 重新建置與封裝。
舊 v2.0.8 tag 與附件保留。本版包含 Portable、Setup、自有來源、第三方對應來源及 SHA256 清單。

## 授權與來源交付

自有程式碼 MIT；Qt 動態連結 LGPLv3，FFmpeg LGPLv2.1 或更新版本，Qt 翻譯 BSD-3-Clause。Portable／Setup 各自內含完整第三方來源 ZIP、建置設定與來源證據。
DLL 必須與 licenses/source-provenance.json 中的來源雜湊相符。Qt 與 FFmpeg 採用 Qt 官方 Windows kit 二進位檔。
Microsoft SDK／VS 元件原始條款、接收者同意流程與散布者持續義務保留，詳見 licenses/MICROSOFT-REDIST.md。接收者按同意不能代替發布者義務。
再散布時保留完整 sources、licenses 與接受流程；自製素材聲明仍由發布者負責。

## 驗證範圍

v2.1.0 已驗證本機 Windows x64 Release 建置、CTest、Windows 原生 UI、Qt 100% 至 200% 縮放，以及 v2.0.9 SQLite 備份還原。乾淨 Windows VM、混合 DPI 多螢幕、NSIS 安裝及解除安裝尚未驗收。LuckyWheel 主程式目前未簽章。 [v2.1.0 版本說明](RELEASE-NOTES-v2.1.0.md)
