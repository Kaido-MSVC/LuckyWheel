# LuckyWheel v2.1.0 — Windows x64 Portable

v2.1.0 提供 Windows x64 免安裝版、應用程式與第三方對應原始碼，以及 SHA-256 清單。

## 繁體中文

- 側邊導覽欄可完全收合並記住上次狀態。九種語言切換後會立即調整導覽寬度，小視窗會讓較長的譯文換行。
- 轉盤重新設計配色、輪框、軸心與頂部指標。抽獎控制台整合輪次、抽獎狀態、結果、獎品機率與售罄選項。
- 獎品設定、歷史紀錄、設定及多個對話方塊共用新版介面樣式，支援淺色、深色與跟隨系統。
- Windows x64 版本採 C++23、Qt Widgets 6.12.1，使用官方 QSQLITE 內嵌的 SQLite 3.54.1。沿用 v2.0.9 的抽獎規則、SQLite 結構、JSON schema 1 與既有 CSV 格式。跟隨系統外觀保存在本機 `ui.ini`。
- 備份完成後才以目的檔名出現。還原會核對備份的資料表及歷史紀錄數值，檢查資料讀取是否完成，並在單一交易中寫入設定與歷史。

使用免安裝版時請完整解壓縮 ZIP，執行根目錄的 `LuckyWheel.exe`，並保留 `licenses/` 與 `sources/`。沿用既有資料前，請先關閉舊版並使用程式的備份功能。

本次驗證在本機 Windows x64 進行，包含 Windows 原生 UI 與 Qt 100%、125%、150%、200% 縮放測試。乾淨 Windows VM、混合 DPI 多螢幕及 NSIS 安裝流程尚未驗收。

## English

This release includes a Windows x64 Portable ZIP, corresponding application and third-party source archives, and SHA-256 checksums.

- The sidebar collapses completely and remembers its state. Its width updates immediately when switching among the nine supported languages. Long labels wrap in compact windows.
- The wheel has updated colors, rim, hub, and pointer. The draw console brings together the current round, draw status, last result, prize probabilities, and the sold-out display option.
- Prize management, history, settings, and in-app dialogs share the updated design. Appearance can be set to light, dark, or follow the OS.
- The Windows x64 build uses C++23, Qt Widgets 6.12.1, and the official QSQLITE driver with SQLite 3.54.1. It retains the v2.0.9 drawing rules, SQLite schema, JSON schema 1, and CSV formats. The follow-system setting is stored locally in `ui.ini`.
- A backup appears at its destination when complete. Restoration validates the backup tables, history values, and read results, then writes settings and history in a single transaction.

Extract the complete Portable ZIP, run `LuckyWheel.exe` from its root, and retain the `licenses/` and `sources/` folders. Before switching versions, close the old app and back up its data with the built-in backup command.

Validation ran on a local Windows x64 machine, including native UI tests and Qt scaling checks at 100%, 125%, 150%, and 200%. A clean Windows VM, mixed-DPI monitors, and the NSIS installer remain untested for this release.
