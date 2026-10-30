# LuckyWheel v2.0.8 — Windows x64

幸運轉盤桌面程式；自有程式與音效採 MIT。本次完成第三方授權與來源交付修正，版本維持 2.0.8。

## 發布附件

- `LuckyWheel-2.0.8-Windows-x64-Portable.zip`：完整解壓，執行 LuckyWheel.exe。
- `LuckyWheel-2.0.8-Windows-x64-Setup.exe`：目前使用者安裝；可選桌面捷徑。
- `LuckyWheel-2.0.8-source.zip`：自有程式、建置腳本、素材及授權。
- `LuckyWheel-2.0.8-third-party-sources.zip`：Qt 6.12.0、FFmpeg 9.0.1、zlib、對應建置腳本與驗證證據。
- `SHA256SUMS.txt`：以上四份附件的雜湊。

Portable 和 Setup **各自內含同一份完整第三方來源 ZIP**，解壓或安裝後位於 `sources/`。
本目錄的獨立第三方來源 ZIP 與包內版本完全相同，可直接免費取得。下載頁應同時列出以上五個附件。
發布者不得只上傳二進位而抽掉包內來源。轉交友人時傳送完整 Portable 或 Setup 即包含 LGPL 來源。

本程式動態使用 Qt 與 FFmpeg；FFmpeg 採 LGPL v2.1 或更新版本，Qt 採 LGPL v3。
對應來源、配置與重建／替換說明位於 `licenses/SOURCE-DELIVERY.md` 及來源 ZIP。
允許替換相容的 LGPL 函式庫並為修改函式庫進行除錯及逆向工程。
Microsoft 元件另依限於該元件的條款，安裝／首次執行會要求明確同意，不限制 LGPL 權利。

主程式及安裝包未簽章；本輪未作乾淨 Windows 或既有安裝的原地升級測試。
升級前關閉程式並用內建功能備份 SQLite 資料。資料位置及解除安裝行為見 INSTALL.md。

## 發布操作（尚未執行）

Repo 只採用本次 source ZIP 的明確清單，勿上傳 build、runtime、packages、私人資料或舊包。
GitHub Release 上傳上述同一套五個附件，使用本頁前述內容作發布說明並連結來源附件。
發布者須遵守 licenses/MICROSOFT-REDIST.md 記錄的 SDK／VS 持續散布義務。
本機套件就緒不代表已公開；此輪沒有建立 Release，也沒有推送 GitHub。
